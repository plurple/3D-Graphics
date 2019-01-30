#pragma once

#include <sponza/sponza_fwd.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <memory>

class MyView : public tygra::WindowViewDelegate
{
public:
    
    MyView();
    
    ~MyView();
    
    void setScene(const sponza::Context * scene);

private:

    void windowViewWillStart(tygra::Window * window) override;
    
    void windowViewDidReset(tygra::Window * window,
                            int width,
                            int height) override;

    void windowViewDidStop(tygra::Window * window) override;
    
    void windowViewRender(tygra::Window * window) override;

private:
    const sponza::Context * scene_;
	
	// Me from here down
	GLuint shader_program_{ 0 };

	const static GLuint kNullId = 0;

	//Vertex attributes defined.
	enum VertexAttribIndexes
	{
		kVertexPosition = 0,
		kVertexNormal = 1,
		kVertexTextureCoOrd = 2
	};

	//mesh structure to hold vbo's, vao's and id's
	struct MeshGL
	{
		GLuint position_vbo{ 0 };
		GLuint normal_vbo{ 0 };
		GLuint textureCoOrd_vbo{ 0 };

		GLuint element_vbo{ 0 };

		GLuint vao{ 0 };
		int element_count{ 0 };
		int meshID{ 0 };
	};

	//vector to hold the mesh's and maps to hold the diffuse and specular textures.
	std::vector<MeshGL> meshVector;
	std::unordered_map<std::string, GLuint> diffuseTextures;
	std::unordered_map<std::string, GLuint> specularTextures;
};
