#version 450
//#extension SPV_KHR_shader_draw_parameters : require
#extension GL_ARB_shader_draw_parameters : require

layout(constant_id = 0) const uint INDIRECT_RENDERING = 0;

#define TEXTURE 0
#define SAMPLER 1
#define CONSTANTBUFFER 2
#define POSITION 3
#define TEXCOORD 4
#define NORMAL 5
#define TRANSLATION 6
//#define INDIRECTBUFFER 7
#define INDEX 8


layout(set = 0, binding = POSITION, rgba32f) uniform imageBuffer g_Positions;
layout(set = 0, binding = TEXCOORD, rg32f) uniform imageBuffer g_Texcoords;

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

struct IndexStruct {
	uint Position;
	uint Texcoord;
	uint Normal;
	uint Translation;
};

layout(set = 0, binding = INDEX) buffer Index {
	IndexStruct g_Indices[];
};

out gl_PerVertex
{
	vec4 gl_Position;
};

layout(location = 0) out vec2 o_TexC;

void main()
{
	IndexStruct indices = g_Indices[INDIRECT_RENDERING == 1 ? gl_DrawIDARB : gl_BaseInstanceARB];
	mat4 world = g_Translations[indices.Translation];
	gl_Position = g_Proj * g_View * world * vec4(imageLoad(g_Positions, int(indices.Position) + gl_VertexIndex).xyz, 1.0f);

	
	o_TexC = vec2(imageLoad(g_Texcoords, int(indices.Texcoord) + gl_VertexIndex).xy);
}
