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
};

struct Frustum
{
	vec4 leftPlane;
	vec4 rightPlane;
	vec4 topPlane;
	vec4 bottomPlane;
	vec4 nearPlane;
	vec4 farPlane;
	
};


layout(set = 0, binding = INDIRECT_BUFFER) buffer IndirectBuffer {
	Indirect g_IndirectCalls[];
};

layout(set = 0, binding = BOUNDING_BUFFER) buffer BoundingBuffer {
	Bounding g_BoundingBoxes[];
};

layout(set = 0, binding = TRANSLATION_BUFFER) buffer Translation {
	mat4 g_Translations[];
};

layout(set = 0, binding = FRUSTUM_BUFFER) uniform FrustumConstants {
	Frustum g_Frustum;
};


int intersectionTest(uint index)
{
	vec4 plane;
	vec4 absPlane;
	float d;
	float r;
	mat4 world = g_Translations[index];
	vec4 pos = g_BoundingBoxes[index].pos;
	pos.w = 1.0;
	pos = pos * world;
	int i;

/*	for(i = 0; i < 6; i++)
	{
		plane = g_Frustum.fplane[i];
		plane.w = 0.0f;
		absPlane = abs(plane);
		d = dot(pos.xyz, plane.xyz);
		r = dot(g_BoundingBoxes[index].extents.xyz, absPlane.xyz);
		if(!(d - r >= -g_Frustum.fplane[i].w))
			return 0;
	}
*/
	vec3 minpoint = pos.xyz - abs(g_BoundingBoxes[index].extents.xyz);
	vec3 maxpoint = pos.xyz + abs(g_BoundingBoxes[index].extents.xyz);

	vec4 planes[] = {g_Frustum.leftPlane,
	g_Frustum.rightPlane, 
	g_Frustum.topPlane, 
	g_Frustum.bottomPlane, 
	g_Frustum.nearPlane, 
	g_Frustum.farPlane};

	for(i = 0; i < 6; i++)
	{
		int px = planes[i].x > 0.0 ? 1 : 0;
		int py = planes[i].y > 0.0 ? 1 : 0;
		int pz = planes[i].z > 0.0 ? 1 : 0;

		float dp;
		if(px == 1)
			dp = planes[i].x * minpoint.x;
		else
			dp = planes[i].x * maxpoint.x;
		if(py == 1)
			dp += planes[i].y * minpoint.y;
		else
			dp += planes[i].y * maxpoint.y;
		if(pz == 1)
			dp += planes[i].z * minpoint.z;
		else
			dp += planes[i].z * maxpoint.z;

		if(dp < -planes[i].w)
			return 0;
	}
	return 1;

/*
	plane = g_Frustum.leftPlane;
	plane.w = 0.0f;
	absPlane = abs(plane);
	d = dot(pos.xyz, plane.xyz);
	r = dot(g_BoundingBoxes[index].extents.xyz, absPlane.xyz);
	if(!(d - r >= -g_Frustum.leftPlane.w))
		return 0;

	plane = g_Frustum.rightPlane;
	plane.w = 0.0f;
	absPlane = abs(plane);
	d = dot(pos.xyz, plane.xyz);
	r = dot(g_BoundingBoxes[index].extents.xyz, absPlane.xyz);
	if(!(d - r >= -g_Frustum.rightPlane.w))
		return 0;

	plane = g_Frustum.bottomPlane;
	plane.w = 0.0f;
	absPlane = abs(plane);
	d = dot(pos.xyz, plane.xyz);
	r = dot(g_BoundingBoxes[index].extents.xyz, absPlane.xyz);
	if(!(d - r >= -g_Frustum.bottomPlane.w))
		return 0;
	
	plane = g_Frustum.topPlane;
	plane.w = 0.0f;
	absPlane = abs(plane);
	d = dot(pos.xyz, plane.xyz);
	r = dot(g_BoundingBoxes[index].extents.xyz, absPlane.xyz);
	if(!(d - r >= -g_Frustum.topPlane.w))
		return 0;

	plane = g_Frustum.nearPlane;
	plane.w = 0.0f;
	absPlane = abs(plane);
	d = dot(pos.xyz, plane.xyz);
	r = dot(g_BoundingBoxes[index].extents.xyz, absPlane.xyz);
	if(!(d - r >= -g_Frustum.nearPlane.w))
		return 0;
	
	plane = g_Frustum.farPlane;
	plane.w = 0.0f;
	absPlane = abs(plane);
	d = dot(pos.xyz, plane.xyz);
	r = dot(g_BoundingBoxes[index].extents.xyz, absPlane.xyz);
	if(!(d - r >= -g_Frustum.farPlane.w))
		return 0;
	
	return 1;*/
}

void main() {
	uint index =  gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * gl_NumWorkGroups.x * gl_WorkGroupSize.x;

	g_IndirectCalls[index].instanceCount = intersectionTest(index);

	
}