#version 450

#define POSITION 0
#define TEXCOORD 1
#define NORMAL 2
#define TRANSLATION 3
#define TEXTURE 4
#define SAMPLER 5
#define CONSTANTBUFFER 6

layout(set = 0, binding = POSITION) buffer Positions {
	vec3 g_Positions[];
};

layout(set = 0, binding = TEXCOORD) buffer Texcoords {
	vec2 g_Texcoords[];
};

layout(set = 0, binding = NORMAL) buffer Normals {
	vec3 g_Normals[];
};

layout(set = 0, binding = TRANSLATION) buffer Translations {
	mat4 g_Translations[];
};

layout(set = 0, binding = CONSTANTBUFFER) uniform CameraConstants {
	mat4 g_View;
	mat4 g_Proj;
};

out gl_PerVertex
{
	vec4 gl_Position;
};

layout(location = 0) out vec2 o_TexC;

void main()
{
	gl_Position = g_Proj * g_View * vec4(g_Positions[gl_VertexIndex], 1.0f);

	o_TexC = g_Texcoords[gl_VertexIndex];
}
