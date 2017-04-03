#version 450

layout(location = 0) in vec2 i_TexC;

layout(location = 0) out vec4 o_Albedo;

layout(set = 0, binding = 4) uniform texture2D tex;
layout(set = 0, binding = 5) uniform sampler samp;

void main()
{
	o_Albedo = texture(sampler2D(tex, samp), i_TexC);
}
