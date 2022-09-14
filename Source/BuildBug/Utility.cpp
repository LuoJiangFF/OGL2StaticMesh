#include "Utility.h"

#include <Engine/StaticMesh.h>

#include <MeshDescription.h>
#include <MeshConversion/Public/MeshDescriptionBuilder.h>
#include <StaticMeshAttributes.h>
#include "C:/Program Files/Epic Games/UE_5.0/Engine/Plugins/Runtime/ProceduralMeshComponent/Source/ProceduralMeshComponent/Public/ProceduralMeshComponent.h"

int32 AppendOGLVertices(FMeshDescriptionBuilder& builder, uint8* buffer, int32& vertexCount, int32 length)
{
	int32* intBuffer = reinterpret_cast<int32*>(buffer);
	int32 version = *(intBuffer + 0), byteOffset = sizeof(int32) * 2;
	int32 normalCount = 0, uvCount = 0;
	vertexCount = *(intBuffer + 1) / 3;

	FVector3f* vertices = reinterpret_cast<FVector3f*>(buffer + byteOffset);
	byteOffset += sizeof(FVector3f) * vertexCount;

	intBuffer = reinterpret_cast<int32*>(buffer + byteOffset);
	normalCount = *(intBuffer + 0) / 3;
	byteOffset += sizeof(int32);

	FVector3f* normals = reinterpret_cast<FVector3f*>(buffer + byteOffset);
	byteOffset += sizeof(FVector3f) * normalCount;

	intBuffer = reinterpret_cast<int32*>(buffer + byteOffset);
	uvCount = *(intBuffer + 0) / 2;
	byteOffset += sizeof(int32);

	FVector2f* uvs = reinterpret_cast<FVector2f*>(buffer + byteOffset);
	byteOffset += sizeof(FVector2f) * uvCount;

	FVector2D uv(0.0);
	FVector vertex(0.0), normal(0.0);
	for (int i = 0; i < vertexCount; i++)
	{
		FVector3f& vertexF = *(vertices + i);
		FVector3f& normalF = *(normals + i);
		FVector2f& uvF = *(uvs + i);

		vertex.X = vertexF.X; vertex.Y = vertexF.Y; vertex.Z = vertexF.Z;
		normal.X = normalF.X; normal.Y = normalF.Y; normal.Z = normalF.Z;
		uv.X = uvF.X; uv.Y = uvF.Y;

		FVertexID vertexID = builder.AppendVertex(vertex);
		FVertexInstanceID instanceID = builder.AppendInstance(vertexID);
		builder.SetInstanceNormal(instanceID, normal);
		builder.SetInstanceUV(instanceID, uv);
	}

	return byteOffset;
}


int32 AppendOGLTriangles(FMeshDescriptionBuilder& builder, FMeshDescription& description, uint8* buffer, int32 byteOffset, int32 vertexCount, int32 length)
{
	int32* intBuffer = reinterpret_cast<int32*>(buffer + byteOffset);
	int32 indexLength = *(intBuffer + 1);

	bool useShort = *(intBuffer + 0) == 0;
	byteOffset += sizeof(int32) * 2;

	int32* indices = reinterpret_cast<int32*>(buffer + byteOffset);
	byteOffset += useShort ? sizeof(uint16) * indexLength : sizeof(int32) * indexLength;
	byteOffset += useShort && (indexLength & 0x01) == 0x01 ? sizeof(uint16) : 0;

	intBuffer = reinterpret_cast<int32*>(buffer + byteOffset);
	byteOffset += sizeof(int32);

	int32 subMeshCount = *(intBuffer + 0);
	int32* subMeshes = reinterpret_cast<int32*>(buffer + byteOffset);

	if (useShort)
	{
		uint16* shortIndices = reinterpret_cast<uint16*>(indices);
		for (int i = 0; i < subMeshCount; i++)
		{
			int32 baseIndex = (i << 1) + i;	// i * 3
			int32 indexStart = *(subMeshes + baseIndex + 0);
			int32 indexCount = *(subMeshes + baseIndex + 1);
			int32 materialId = *(subMeshes + baseIndex + 2);

			FPolygonGroupID groupID = builder.AppendPolygonGroup();
			int indexEnd = indexStart + indexCount;

			for (int j = indexStart; j < indexEnd; j += 3)
			{
				FVertexInstanceID index0 = static_cast<FVertexInstanceID>(*(shortIndices + j + 0));
				FVertexInstanceID index1 = static_cast<FVertexInstanceID>(*(shortIndices + j + 2));
				FVertexInstanceID index2 = static_cast<FVertexInstanceID>(*(shortIndices + j + 1));
				FTriangleID triangleID = builder.AppendTriangle(index0, index1, index2, groupID);
			}
		}
	}
	else
	{
		for (int i = 0; i < subMeshCount; i++)
		{
			int32 baseIndex = (i << 1) + i;
			int32 indexStart = *(subMeshes + baseIndex + 0);
			int32 indexCount = *(subMeshes + baseIndex + 1);
			int32 materialId = *(subMeshes + baseIndex + 2);

			FPolygonGroupID groupID = builder.AppendPolygonGroup();
			int indexEnd = indexStart + indexCount;

			for (int j = indexStart; j < indexEnd; j += 3)
			{
				FVertexInstanceID index0 = static_cast<FVertexInstanceID>(*(indices + j + 0));
				FVertexInstanceID index1 = static_cast<FVertexInstanceID>(*(indices + j + 2));
				FVertexInstanceID index2 = static_cast<FVertexInstanceID>(*(indices + j + 1));
				FTriangleID triangleID = builder.AppendTriangle(index0, index1, index2, groupID);
			}
		}
	}

	return subMeshCount;
}

int FillStaticMeshWithOGL(UStaticMesh& mesh, uint8* buffer, int32 length)
{
	if (buffer == nullptr)
	{
		return 0;
	}

	// MeshDescription 将会描述StaticMesh的信息，包括几何，UV，法线 等
	FMeshDescription meshDescription;
	FStaticMeshAttributes attributes(meshDescription);
	attributes.Register();

	FMeshDescriptionBuilder descriptionBuilder;
	descriptionBuilder.SetMeshDescription(&meshDescription);
	descriptionBuilder.EnablePolyGroups();
	descriptionBuilder.SetNumUVLayers(1);

	int vertexCount = 0;
	int byteOffset = AppendOGLVertices(descriptionBuilder, buffer, vertexCount, length);
	int subMeshCount = AppendOGLTriangles(descriptionBuilder, meshDescription, buffer, byteOffset, vertexCount, length);

	UStaticMesh::FBuildMeshDescriptionsParams params;
	params.bBuildSimpleCollision = true;
	params.bFastBuild = true;

	TArray<const FMeshDescription*> meshDescriptionPtrs;
	meshDescriptionPtrs.Emplace(&meshDescription);
	mesh.BuildFromMeshDescriptions(meshDescriptionPtrs, params);
	return subMeshCount;
}
