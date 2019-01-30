#version 330
//structure of material infromation.
struct Material
{
	vec3 ambientColour;//ambient colour of the material.
	vec3 diffuseColour;//diffuse colour of the material.
	vec3 specularColour;//specular colour fo the material.
	float shininess;//shininess of the material. 0 if not shiny.
	sampler2D diffuseTexture;//diffuse texture of material.
	sampler2D specularTexture;//specular texture of material.
	bool hasDiffuseTexture;//check to see if material has a diffuse texture.
	bool hasSpecularTexture;//check to see if material has a specular texture.
	bool isShiny;//check to see if material is shiny.
};
uniform Material mat;//instance of the material.

//structure of light information.
struct Light
{
	vec3 intensity;//intensity per light.
	vec3 position;//position of light.
	float range;//range of how far light can be seen.
};
uniform int numLights;//number of lights.
uniform Light lights[22];//vector of light instances.
uniform vec3 ambientIntensity;//the intensity of the ambient light.
uniform vec3 cameraPos;//the position of the camera.

in vec3 varying_normal;//the normal of each vertex varyed for each fragment.
in vec3 varying_position;//the position of each vertex varyed for each fragment.
in vec2 varying_textureCoOrd;//the texture Coordinates of each vertex varyed for each fragment.

out vec4 fragment_colour;

void main(void)
{
	vec3 normal = normalize(varying_normal);
	vec3 viewDir = normalize(cameraPos - varying_position); //the view direction.
	vec3 ambient = ambientIntensity * mat.ambientColour; //the final ambient component.
	vec3 diff_colour; //a variable to store the diffuse colour temporarily.
	vec3 spec_colour; //a variable to store the specular texture temporarily.
	vec3 diffSum = vec3(0, 0, 0); // to store the diffuse part of the phong shading.
	vec3 specSum = vec3(0, 0, 0); //to store the specular part of the phong shading.
	//for loop to loop thorough all of the lights.
	for (int i = 0; i < numLights; i++)
	{
		//attenuation is used to smoothly reduce the amount of light over the last 50% of the lights range.
		float attenuation = smoothstep(lights[i].range, lights[i].range/2.0, distance(varying_position, lights[i].position));
		if (mat.isShiny)//a check to see if the material is shiny
		{	
			if (mat.hasDiffuseTexture)//a check to see if the material has a diffuse texture.
			{
				//if it has a texture multiply it into the diffuse colour.
				diff_colour = mat.diffuseColour * texture(mat.diffuseTexture, varying_textureCoOrd).rgb;
			}
			else
			{
				//otherwise just use the diffuse colour
				diff_colour = mat.diffuseColour;
			}
			if (mat.hasSpecularTexture)//a check to see if the material has a specular texture.
			{
				//if it has a texture multiply it into the specular colour.
				spec_colour = mat.specularColour * texture(mat.specularTexture, varying_textureCoOrd).rgb;
			}
			else
			{
				//otherwise just use the specular colour
				spec_colour = mat.specularColour;
			}
			vec3 lightDir = normalize(lights[i].position - varying_position);//the direction of the light
			vec3 diffuse = max(0.0, dot(normal, lightDir)) * diff_colour;
			diffSum += lights[i].intensity * diffuse * attenuation;//adding this lights diffuse component to the whole diffuse component.
			vec3 reflectDir = reflect(-lightDir, normal);//the reflected light direction.
			vec3 specular = pow(max(dot(viewDir, reflectDir), 0.0), mat.shininess) * spec_colour;
			specSum += lights[i].intensity * specular * attenuation;//adding this lights specular component to the whole specular component.
		}
		else
		{
			if (mat.hasDiffuseTexture)
			{
				diff_colour = mat.diffuseColour * texture(mat.diffuseTexture, varying_textureCoOrd).rgb;
			}
			else
			{
				diff_colour = mat.diffuseColour;
			}
			vec3 lightDir = normalize(lights[i].position - varying_position);
			vec3 diffuse = max(0.0, dot(normal, lightDir)) * diff_colour;
			diffSum += lights[i].intensity * diffuse * attenuation;
		}
	}

	//finding the total phong reflection for the scene.
	vec3 Phong = (ambient + diffSum + specSum);

    fragment_colour = vec4(Phong, 1.0);
}
