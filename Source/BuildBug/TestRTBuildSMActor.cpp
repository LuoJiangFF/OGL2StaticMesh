#include"TestRTBuildSMActor.h"

#include "MeshDescription.h"
#include "MeshDescriptionBuilder.h"
#include "StaticMeshAttributes.h"



ATestRTBuildSMActor::ATestRTBuildSMActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//创建 StaticMeshComponent 对象：
	_smComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("smComp"), false);

	//设定其为根组件：
	SetRootComponent(_smComp);
}

void ATestRTBuildSMActor::BeginPlay()
{
	// 构建一个简单的金字塔

	// 创建 StaticMesh 资源
	UStaticMesh* staticMesh = NewObject<UStaticMesh>(this);
	staticMesh->GetStaticMaterials().Add(FStaticMaterial());

	// MeshDescription 将会描述StaticMesh的信息，包括几何，UV，法线 等
	FMeshDescription meshDesc;

	FStaticMeshAttributes Attributes(meshDesc);
	Attributes.Register();

	//为 MeshDescription 创建一个 MeshDescriptionBuilder 协助构建数据
	FMeshDescriptionBuilder meshDescBuilder;
	meshDescBuilder.SetMeshDescription(&meshDesc);
	meshDescBuilder.EnablePolyGroups();
	meshDescBuilder.SetNumUVLayers(1);

	//使用 MeshDescriptionBuilder 来构建数据：
	{
		// 创建 5 个顶点 
		TArray< FVertexID > vertexIDs;
		vertexIDs.SetNum(5);

		//指定 5 个顶点各自的位置：
		vertexIDs[0] = meshDescBuilder.AppendVertex(FVector(0.0f, 0.0f, 100.0f));		// 顶尖
		vertexIDs[1] = meshDescBuilder.AppendVertex(FVector(-50.0f, 50.0f, 0.0f));		// 角 1
		vertexIDs[2] = meshDescBuilder.AppendVertex(FVector(-50.0f, -50.0f, 0.0f));	// 角 2
		vertexIDs[3] = meshDescBuilder.AppendVertex(FVector(50.0f, -50.0f, 0.0f));		// 角 3
		vertexIDs[4] = meshDescBuilder.AppendVertex(FVector(50.0f, 50.0f, 0.0f));		// 角 4

		// 分配一个 polygon group
		FPolygonGroupID polygonGroup = meshDescBuilder.AppendPolygonGroup();

		//为了方便，定义一个结构体来封装三角形中一个顶点的信息
		struct VertexInfo
		{
			int ID;					//对应的顶点ID
			FVector InstanceNormal;	//此三角形中顶点的法线
			FVector2D InstanceUV;	//此三角形中顶点的UV
			VertexInfo(int InID, FVector InInstanceNormal, FVector2D InInstanceUV)
				:ID(InID), InstanceNormal(InInstanceNormal), InstanceUV(InInstanceUV)
			{
			}
		};

		//为了方便，创建一个局部函数用来快速根据给定的顶点信息创建三角形
		auto AppendTriangle = [&meshDescBuilder, &vertexIDs, polygonGroup](TArray<VertexInfo> vertex)
		{
			TArray< FVertexInstanceID > vertexInsts;//三角形中的每个顶点
			for (int i = 0; i < 3; i++)
			{
				FVertexInstanceID instance = meshDescBuilder.AppendInstance(vertexIDs[vertex[i].ID]);
				meshDescBuilder.SetInstanceNormal(instance, vertex[i].InstanceNormal);			//法线
				meshDescBuilder.SetInstanceUV(instance, vertex[i].InstanceUV, 0);				//UV
				meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));	//顶点色
				vertexInsts.Add(instance);
			}
			//增加此三角形
			meshDescBuilder.AppendTriangle(vertexInsts[0], vertexInsts[1], vertexInsts[2], polygonGroup);
		};

		//四个面的数据：

		// 面 1 (朝向 -X) 三角型的顶点数据：
		AppendTriangle({
			VertexInfo(0,FVector(-0.7071, 0, 0.7071),FVector2D(0, 1)),
			VertexInfo(2,FVector(-0.7071, 0, 0.7071),FVector2D(1, 0)),
			VertexInfo(1,FVector(-0.7071, 0, 0.7071),FVector2D(0, 0)) });
		// 面 2 (朝向 -Y) 三角型的顶点数据：
		AppendTriangle({
			VertexInfo(0,FVector(0, -0.7071, 0.7071),FVector2D(0, 1)),
			VertexInfo(3,FVector(0, -0.7071, 0.7071),FVector2D(1, 0)),
			VertexInfo(2,FVector(0, -0.7071, 0.7071),FVector2D(0, 0)) });
		// 面 3 (朝向 +X) 三角型的顶点数据：
		AppendTriangle({
			VertexInfo(0,FVector(0.7071, 0, 0.7071),FVector2D(0, 1)),
			VertexInfo(4,FVector(0.7071, 0, 0.7071),FVector2D(1, 0)),
			VertexInfo(3,FVector(0.7071, 0, 0.7071),FVector2D(0, 0)) });
		// 面 4 (朝向 +Y) 三角型的顶点数据：
		AppendTriangle({
			VertexInfo(0,FVector(0, 0.7071, 0.7071),FVector2D(0, 1)),
			VertexInfo(1,FVector(0, 0.7071, 0.7071),FVector2D(1, 0)),
			VertexInfo(4,FVector(0, 0.7071, 0.7071),FVector2D(0, 0)) });
	}

	// MeshDescription 参数
	UStaticMesh::FBuildMeshDescriptionsParams mdParams;
	mdParams.bBuildSimpleCollision = true;
	mdParams.bFastBuild = true;

	// 创建一个列表容纳 MeshDescription。（因为 BuildFromMeshDescriptions 函数的参数需要列表）
	TArray<const FMeshDescription*> meshDescPtrs;
	meshDescPtrs.Emplace(&meshDesc);

	//构建！
	staticMesh->BuildFromMeshDescriptions(meshDescPtrs, mdParams);

	// 将 StaticMesh 指定给 StaticMeshComponent组件
	_smComp->SetStaticMesh(staticMesh);
}
