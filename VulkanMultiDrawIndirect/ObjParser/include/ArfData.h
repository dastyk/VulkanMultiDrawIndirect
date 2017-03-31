#ifndef __cplusplus
typedef nullptr NULL;
#endif


#ifndef _ARF_DATA_H_
#define _ARF_DATA_H_
#include <stdint.h>
#include <vector>
#include <stdint.h>

namespace ArfData
{
#define POSITION_INDEX 0
#define TEXCOORD_INDEX 1
#define NORMAL_INDEX 2
#define INDEX_NULL UINT32_MAX
#define SUBMESH_NAME_MAX_LENGHT 20
	struct Position
	{
		float x, y, z;
		Position() : x(0.0f), y(0.0f), z(0.0f)
		{}
		Position(float x, float y, float z) : x(x), y(y), z(z)
		{}
	};

	struct TexCoord
	{
		float u, v;
		TexCoord() : u(0.0f), v(0.0f)
		{}
		TexCoord(float u, float v) : u(u), v(v)
		{}
	};

	struct Normal
	{
		float x, y, z;
		Normal() : x(0.0f), y(0.0f), z(0.0f)
		{}
		Normal(float x, float y, float z) : x(x), y(y), z(z)
		{}
	};

	struct Indices
	{
		uint8_t indexCount;
		uint32_t index[3];// array of size 4.
	};

	struct Face
	{
		uint8_t indexCount;
		Indices indices[4]; // Array of size 4.
		Face(std::vector<std::vector<uint32_t>>& face) : indexCount(0)
		{
			indexCount = face.size();

			for (uint8_t i = 0; i < indexCount; i++)
			{
				indices[i].indexCount = face[i].size();
				for (uint8_t j = 0; j < face[i].size(); j++)
				{
					indices[i].index[j] = face[i][j];
				}
			}
		}

	};

	struct SubMesh
	{
		char name[SUBMESH_NAME_MAX_LENGHT];
		uint32_t faceStart;
		uint32_t faceCount;
	};

	struct Data
	{
		size_t allocated = 0;

		
		uint32_t NumPos = 0;
		uint32_t PosCap = 0;

		
		uint32_t NumTex = 0;
		uint32_t TexCap = 0;

	
		uint32_t NumNorm = 0;
		uint32_t NormCap = 0;

		
		uint32_t NumFace = 0;
		uint32_t FaceCap = 0;

	
		uint8_t NumSubMesh = 0;
		uint8_t SubMeshCap = 0;

	};

	struct DataPointers
	{
		void* buffer = nullptr;

		Position* positions;
		TexCoord* texCoords;
		Normal* normals;
		Face* faces;
		SubMesh* subMesh;
	};
}

#endif