#version 450

#define INDIRECT_BUFFER 0
#define BOUNDING_BUFFER 1
#define TRANSLATION_BUFFER 2
#define FRUSTUM_BUFFER 3

struct Indirect
{
	uint vertexCount;
	uint instanceCount;
	uint firstVertex;
	uint firstInstance;
};

struct Bounding
{
	vec4 pos;
	vec4 extents;
	uint containedVertices;
	uint pad;
	uint pad2;
	uint pad3;
};

struct Frustum
{
	vec4 leftPlane;
	vec4 rightPlane;
	vec4 bottomPlane;
	vec4 topPlane;
	vec4 nearPlane;
	vec4 farPlane;
};


layout(set = 0, binding = INDIRECT_BUFFER) buffer IndirectBuffer {
	Indirect g_IndirectCalls[];
};

layout(set = 0, binding = BOUNDING_BUFFER) buffer BoundingBuffer {
	Bounding g_BoundingBoxes[];
};

layout(set = 0, binding = TRANSLATION_BUFFER) uniform Translation {
	mat4 g_Translations[];
};

layout(set = 0, binding = FRUSTUM_BUFFER) uniform FrustumConstants {
	Frustum g_Frustum;
	uint g_TotalObjects;
};

void main() {

}