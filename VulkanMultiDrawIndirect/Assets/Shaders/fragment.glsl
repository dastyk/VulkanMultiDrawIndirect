#version 450

layout(location = 0) in vec2 i_TexC;

layout(location = 0) out vec4 o_Albedo;

void main()
{
	o_Albedo = vec4(0.6f, 0.4f, 0.2f, 1.0f);
}
