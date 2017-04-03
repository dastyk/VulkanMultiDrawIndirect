#version 450

#define TEXTURE 0
#define SAMPLER 1
#define CONSTANTBUFFER 2
#define POSITION 3
#define TEXCOORD 4
#define NORMAL 5
#define TRANSLATION 6



layout(set = 0, binding = POSITION, rgba32f) uniform imageBuffer g_Positions;

//layout(set = 0, binding = TEXCOORD) buffer Texcoords {
//	vec2 g_Texcoords[];
//};
//
//layout(set = 0, binding = NORMAL) buffer Normals {
//	vec3 g_Normals[];
//};
//
layout(set = 0, binding = TRANSLATION) buffer Translations {
	mat4 g_Translations[];
};

layout(set = 0, binding = CONSTANTBUFFER) uniform CameraConstants {
	mat4 g_View;
	mat4 g_Proj;
};

layout(push_constant) uniform VertexOffsets {
	uint Position;
	uint Texcoord;
	uint Normal;
	uint Translation;
} g_VertexOffsets;

out gl_PerVertex
{
	vec4 gl_Position;
};

//layout(location = 0) out vec2 o_TexC;

void main()
{
	mat4 world = g_Translations[g_VertexOffsets.Translation];
	gl_Position = g_Proj * g_View * world * vec4(imageLoad(g_Positions, int(g_VertexOffsets.Position) + gl_VertexIndex).xyz, 1.0f);

	//o_TexC = g_Texcoords[gl_VertexIndex];
}
