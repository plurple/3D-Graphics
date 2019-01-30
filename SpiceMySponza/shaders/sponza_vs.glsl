#version 330

//matricies to transform to world space and screen space.
uniform mat4 view_projection_xform; 
uniform mat4 world_xform;

//information to be transformed and/or used in the shaders.
in vec3 vertex_position;
in vec3 vertex_normal;
in vec2 vertex_textureCoOrd;

//information passed to the fragment shader to be varied.
out vec3 varying_normal;
out vec3 varying_position;
out vec2 varying_textureCoOrd;

void main(void)
{
	varying_textureCoOrd = vertex_textureCoOrd;
	varying_normal = (world_xform * vec4(vertex_normal, 0.0f)).xyz;//transfromed to world space.
	varying_position = (world_xform * vec4(vertex_position, 1.0f)).xyz;//transfromed to world space.
	vec4 position = view_projection_xform * world_xform * vec4(vertex_position, 1.0f);//transformed to screen space to be drawn.
	gl_Position = position;
}
