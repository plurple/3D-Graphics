#include "MyView.hpp"
#include <sponza/sponza.hpp>
#include <tygra/FileHelper.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
//#include <cassert>

MyView::MyView()
{
}

MyView::~MyView() {
}

void MyView::setScene(const sponza::Context * scene)
{
    scene_ = scene;
}

void MyView::windowViewWillStart(tygra::Window * window)
{
    assert(scene_ != nullptr);

	GLint compile_status = GL_FALSE;

	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	std::string vertex_shader_string
		= tygra::createStringFromFile("resource:///sponza_vs.glsl");
	const char * vertex_shader_code = vertex_shader_string.c_str();
	glShaderSource(vertex_shader, 1,
		(const GLchar **)&vertex_shader_code, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE)
	{
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetShaderInfoLog(vertex_shader, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fragment_shader_string
		= tygra::createStringFromFile("resource:///sponza_fs.glsl");
	const char * fragment_shader_code = fragment_shader_string.c_str();
	glShaderSource(fragment_shader, 1,
		(const GLchar **)&fragment_shader_code, NULL);
	glCompileShader(fragment_shader);
	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_status);
	if (compile_status != GL_TRUE)
	{
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetShaderInfoLog(fragment_shader, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

	// Create shader program & shader in variables
	shader_program_ = glCreateProgram();
	glAttachShader(shader_program_, vertex_shader);

	// TODO: glBindAttribLocation for all shader streamed IN variables e.g.
	glBindAttribLocation(shader_program_, kVertexPosition, "vertex_position");
	glBindAttribLocation(shader_program_, kVertexNormal, "vertex_normal");
	glBindAttribLocation(shader_program_, kVertexTextureCoOrd, "vetex_textureCoOrd");
	glDeleteShader(vertex_shader);
	glAttachShader(shader_program_, fragment_shader);
	glDeleteShader(fragment_shader);
	glLinkProgram(shader_program_);

	GLint link_status = GL_FALSE;
	glGetProgramiv(shader_program_, GL_LINK_STATUS, &link_status);
	if (link_status != GL_TRUE)
	{
		const int string_length = 1024;
		GLchar log[string_length] = "";
		glGetProgramInfoLog(shader_program_, string_length, NULL, log);
		std::cerr << log << std::endl;
	}

	/*
		The framework provides a builder class that allows access to all the mesh data	
	*/

	sponza::GeometryBuilder builder;
	const auto& source_meshes = builder.getAllMeshes();

	// We can loop through each mesh in the scene
	for each (const sponza::Mesh& source in source_meshes)
	{
		// Each mesh has an id that you will need to remember for later use
		// obained by calling source.getId()
		const auto& ids = source.getId();
		// To access the actual mesh raw data we can get the array e.g.
		const auto& positions = source.getPositionArray();
		// TODO: you also need to get the normals, elements and texture coordinates in a similar way
		const auto& normals = source.getNormalArray();
		const auto& textureCoOrds = source.getTextureCoordinateArray();
		const auto& elements = source.getElementArray();
		

		MeshGL mesh;
		// TODO:
		// Create VBOs for position, normals, elements and texture coordinates
		glGenBuffers(1, &mesh.position_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.position_vbo);
		glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3),
			positions.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, kNullId);

		glGenBuffers(1, &mesh.normal_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.normal_vbo);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3),
			normals.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, kNullId);

		glGenBuffers(1, &mesh.textureCoOrd_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.textureCoOrd_vbo);
		glBufferData(GL_ARRAY_BUFFER, textureCoOrds.size() * sizeof(glm::vec2),
			textureCoOrds.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, kNullId);

		glGenBuffers(1, &mesh.element_vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.element_vbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(unsigned int),
			elements.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kNullId);
		mesh.element_count = (int)elements.size();
		mesh.meshID = ids;

		// TODO
		// Create a VAO to wrap all the VBOs
		glGenVertexArrays(1, &mesh.vao);
		glBindVertexArray(mesh.vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.element_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.position_vbo);
		glEnableVertexAttribArray(kVertexPosition);
		glVertexAttribPointer(kVertexPosition, 3, GL_FLOAT, GL_FALSE,
			sizeof(glm::vec3), TGL_BUFFER_OFFSET(0));
		glBindBuffer(GL_ARRAY_BUFFER, mesh.normal_vbo);
		glEnableVertexAttribArray(kVertexNormal);
		glVertexAttribPointer(kVertexNormal, 3, GL_FLOAT, GL_FALSE,
			sizeof(glm::vec3), TGL_BUFFER_OFFSET(0));
		glBindBuffer(GL_ARRAY_BUFFER, mesh.textureCoOrd_vbo);
		glEnableVertexAttribArray(kVertexTextureCoOrd);
		glVertexAttribPointer(kVertexTextureCoOrd, 2, GL_FLOAT, GL_FALSE,
			sizeof(glm::vec2), TGL_BUFFER_OFFSET(0));
		glBindBuffer(GL_ARRAY_BUFFER, kNullId);
		glBindVertexArray(kNullId);


		for (auto &p: scene_->getAllMaterials())
		{
			const auto diffuseName = p.getDiffuseTexture();			
			tygra::Image texture_image = tygra::createImageFromPngFile("resource:///" + diffuseName);
			if (texture_image.doesContainData())
			{
				GLuint texture_{ 0 };
				glGenTextures(1, &texture_);
				glBindTexture(GL_TEXTURE_2D, texture_);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				GLenum pixel_formats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };
				glTexImage2D(GL_TEXTURE_2D,
					0,
					GL_RGBA,
					texture_image.width(),
					texture_image.height(),
					0,
					pixel_formats[texture_image.componentsPerPixel()],
					texture_image.bytesPerComponent() == 1
					? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT,
					texture_image.pixelData());
				glGenerateMipmap(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, kNullId);

				diffuseTextures[diffuseName] = texture_;
			}
		}

		for (auto &p : scene_->getAllMaterials())
		{
			const auto specularName = p.getSpecularTexture();
			tygra::Image texture_image = tygra::createImageFromPngFile("resource:///" + specularName);
			if (texture_image.doesContainData())
			{
				GLuint texture_{ 0 };
				glGenTextures(1, &texture_);
				glBindTexture(GL_TEXTURE_2D, texture_);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				GLenum pixel_formats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };
				glTexImage2D(GL_TEXTURE_2D,
					0,
					GL_RGBA,
					texture_image.width(),
					texture_image.height(),
					0,
					pixel_formats[texture_image.componentsPerPixel()],
					texture_image.bytesPerComponent() == 1
					? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT,
					texture_image.pixelData());
				glGenerateMipmap(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, kNullId);

				specularTextures[specularName] = texture_;
			}
		}
		
		// TODO: store in a mesh structure and add to a container for later use
		meshVector.push_back(mesh);
	}

}

void MyView::windowViewDidReset(tygra::Window * window,
                                int width,
                                int height)
{
    glViewport(0, 0, width, height);
}

void MyView::windowViewDidStop(tygra::Window * window)
{
	glDeleteProgram(shader_program_);
	for (const auto &mesh : meshVector)
	{
		glDeleteBuffers(1, &mesh.position_vbo);
		glDeleteBuffers(1, &mesh.normal_vbo);
		glDeleteBuffers(1, &mesh.textureCoOrd_vbo);
		glDeleteBuffers(1, &mesh.element_vbo);
		glDeleteVertexArrays(1, &mesh.vao);
	}
}

void MyView::windowViewRender(tygra::Window * window)
{
	assert(scene_ != nullptr);

	// Configure pipeline settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Clear buffers from previous frame
	glClearColor(0.f, 0.f, 0.25f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader_program_);
	 
	// Compute viewport
	GLint viewport_size[4];
	glGetIntegerv(GL_VIEWPORT, viewport_size);
	const float aspect_ratio = viewport_size[2] / (float)viewport_size[3];

	// Note: the code above is supplied for you and already works

	// TODO: Compute projection matrix
	// You can get the far plane distance, the near plane distance and the fov from
	float far_plane_dist = scene_->getCamera().getFarPlaneDistance();
	float near_plane_dist = scene_->getCamera().getNearPlaneDistance();
	float fov = scene_->getCamera().getVerticalFieldOfViewInDegrees();

	glm::mat4 projection_xform = glm::perspective(glm::radians(fov), aspect_ratio, near_plane_dist, far_plane_dist);

	// TODO: Compute view matrix
	// You can get the camera position, look at and world up from the scene e.g.
	const auto& camera_pos = (const glm::vec3&)scene_->getCamera().getPosition();
	const auto& world_up = (const glm::vec3&)scene_->getUpDirection();
	const auto& camera_at_pos = camera_pos + (const glm::vec3&)scene_->getCamera().getDirection();

	// Compute camera view matrix and combine with projection matrix
	glm::mat4 view_xform = glm::lookAt(camera_pos, camera_at_pos, world_up);

	// TODO: create combined view * projection matrix and pass to shader as a uniform
	glm::mat4 view_projection_xform = projection_xform * view_xform;

	glUseProgram(shader_program_);

	GLuint view_projection_xform_id = glGetUniformLocation(shader_program_, "view_projection_xform");
	glUniformMatrix4fv(view_projection_xform_id, 1, GL_FALSE, glm::value_ptr(view_projection_xform));

	// TODO: Get light data from scene via scene_->getAllLights()
	// then plug the values into the shader - you may want to leave this until you have a basic scene showing
	const auto& Lights = scene_->getAllLights();
	int numLights = Lights.size();

	GLuint cameraID = glGetUniformLocation(shader_program_, "cameraPos");
	glUniform3fv(cameraID, 1, glm::value_ptr(camera_pos));

	for (int L = 0; L < Lights.size(); L++)
	{
		GLuint intensity = glGetUniformLocation(shader_program_, std::string("lights[" + std::to_string(L) + "].intensity").c_str());
		glUniform3fv(intensity, 1, glm::value_ptr((const glm::vec3&)Lights[L].getIntensity()));

		GLuint position = glGetUniformLocation(shader_program_, std::string("lights[" + std::to_string(L) + "].position").c_str());
		glUniform3fv(position, 1, glm::value_ptr((const glm::vec3&)Lights[L].getPosition()));

		GLuint range = glGetUniformLocation(shader_program_, std::string("lights[" + std::to_string(L) + "].range").c_str());
		glUniform1f(range, Lights[L].getRange());
	}

	GLuint ambientIntensityID = glGetUniformLocation(shader_program_, "ambientIntensity");
	glUniform3fv(ambientIntensityID, 1, glm::value_ptr((const glm::vec3&)scene_->getAmbientLightIntensity()));

	GLuint numberLightsID = glGetUniformLocation(shader_program_, "numLights");
	glUniform1i(numberLightsID, numLights);


	// TODO: Render each mesh
	// Loop through your mesh container e.g.
	for (const auto& mesh : meshVector)
	{
		// Each mesh can be repeated in the scene so we need to ask the scene for all instances of the mesh
		// and render each instance with its own model matrix
		// To get the instances we need to use the meshId we stored earlier e.g.
		const auto& instances = scene_->getInstancesByMeshId(mesh.meshID);
		// then loop through all instances
		for (auto i : instances)
		{
			// for each instance you can call getTransformationMatrix 
			// this then needs passing to the shader as a uniform
			glm::mat4 world_xform = (glm::mat4x3&)scene_->getInstanceById(i).getTransformationMatrix();
			

			GLuint world_xform_id = glGetUniformLocation(shader_program_, "world_xform");
			glUniformMatrix4fv(world_xform_id, 1, GL_FALSE, glm::value_ptr(world_xform));

			// Materials - leave this until you get the main scene showing
			// Each instance of the mesh has its own material accessed like so:
			// Get material for this instance
			const auto& material_id = scene_->getInstanceById(i).getMaterialId();
			const auto& material = scene_->getMaterialById(material_id);
			
			GLuint ambientColourID = glGetUniformLocation(shader_program_, "mat.ambientColour");
			glUniform3fv(ambientColourID, 1, glm::value_ptr((const glm::vec3&)material.getAmbientColour()));

			GLuint diffuseColourID = glGetUniformLocation(shader_program_, "mat.diffuseColour");
			glUniform3fv(diffuseColourID, 1, glm::value_ptr((const glm::vec3&)material.getDiffuseColour()));

			GLuint specularColourID = glGetUniformLocation(shader_program_, "mat.specularColour");
			glUniform3fv(specularColourID, 1, glm::value_ptr((const glm::vec3&)material.getSpecularColour()));

			GLuint isShinyID = glGetUniformLocation(shader_program_, "mat.isShiny");
			glUniform1i(isShinyID, material.isShiny());

			GLuint shininessID = glGetUniformLocation(shader_program_, "mat.shininess");
			glUniform1f(shininessID, (const float&)material.getShininess());

			GLuint hasDiffuseTextureID = glGetUniformLocation(shader_program_, "mat.hasDiffuseTexture");
			GLuint hasSpecularTextureID = glGetUniformLocation(shader_program_, "mat.hasSpecularTexture");

		

			const auto DiffuseName = material.getDiffuseTexture();
			if (diffuseTextures.find(DiffuseName) != diffuseTextures.end())
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, diffuseTextures.at(DiffuseName));

				GLuint diffuseTextureID = glGetUniformLocation(shader_program_, "mat.diffuseTexture");
				glUniform1i(diffuseTextureID, 0);

				glUniform1i(hasDiffuseTextureID, 1);
			}
			else
			{
				glUniform1i(hasDiffuseTextureID, 0);
			}

			const auto SpecularName = material.getSpecularTexture();
			if (specularTextures.find(SpecularName) != specularTextures.end())
			{
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, specularTextures.at(SpecularName));

				GLuint SpecularTextureID = glGetUniformLocation(shader_program_, "mat.specularTexture");
				glUniform1i(SpecularTextureID, 1);

				glUniform1i(hasSpecularTextureID, 1);
			}
			else
			{
				glUniform1i(hasSpecularTextureID, 0);
			}

		

			// Finally you render the mesh e.g.
			glBindVertexArray(mesh.vao);
			glDrawElements(GL_TRIANGLES, mesh.element_count, GL_UNSIGNED_INT, 0);
		}						
	}
}
