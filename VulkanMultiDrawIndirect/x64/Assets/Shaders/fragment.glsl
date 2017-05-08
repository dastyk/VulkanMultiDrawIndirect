#version 450

#define TEXTURE 0
#define SAMPLER 1
#define CONSTANTBUFFER 2
#define POSITION 3
#define TEXCOORD 4
#define NORMAL 5
#define TRANSLATION 6
//#define INDIRECTBUFFER 7
#define INDEX 8


layout(location = 0) in vec2 i_TexC;
layout(location = 1) in flat uint i_TextureID;

layout(location = 0) out vec4 o_Albedo;

layout(set = 0, binding = TEXTURE) uniform texture2D tex[2];
layout(set = 0, binding = SAMPLER) uniform sampler samp;

void main()
{
	o_Albedo = texture(sampler2D(tex[i_TextureID], samp), i_TexC);
}
