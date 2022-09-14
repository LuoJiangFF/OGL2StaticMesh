#include "TestRTBuildPMActor.h"
#include "ProceduralMeshComponent.h"


ATestRTBuildPMActor::ATestRTBuildPMActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//创建 ProceduralMeshComponent 对象：
	PMC = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("myPMC"), false);

	//设定其为根组件：
	SetRootComponent(PMC);
}

void ATestRTBuildPMActor::BeginPlay()
{
	UE_LOG(LogTemp, Log, TEXT("Startpmc"));
	//指定 5 个顶点各自的位置：
	TArray<FVector> VertexPositions;
	VertexPositions.Add(FVector(0.0, 0.0, 100.0));		// 顶尖
	VertexPositions.Add(FVector(-50.0, 50.0, 0.0));		// 角 1
	VertexPositions.Add(FVector(-50.0, -50.0, 0.0));	// 角 2
	VertexPositions.Add(FVector(50.0, -50.0, 0.0));		// 角 3
	VertexPositions.Add(FVector(50.0, 50.0, 0.0));		// 角 4

	//为了方便，定义一个结构体来封装三角形中一个顶点的信息
	struct VertexInfo
	{
		int ID;				//对应的顶点ID
		FVector Normal;		//法线
		FVector2D UV;		//UV
		VertexInfo(int InID, FVector InNormal, FVector2D InUV)
			:ID(InID), Normal(InNormal), UV(InUV)
		{
		}
	};

	//当前MeshSection的数目
	int MeshSectionCount = 0;

	//为了方便，创建一个局部函数用来快速根据给定的顶点信息创建三角形
	auto AppendTriangle = [this, &MeshSectionCount, &VertexPositions](TArray<VertexInfo> data)
	{
		TArray<FVector> Vertices;
		TArray<int32> Indices;
		TArray<FVector> Normals;
		TArray<FVector2D> UV0;
		TArray<FLinearColor> VertexColors;
		TArray<FProcMeshTangent> Tangents;

		for (int i = 0; i < 3; i++)
		{
			Vertices.Add(VertexPositions[data[i].ID]);			//位置
			Indices.Add(i);										//索引
			Normals.Add(data[i].Normal);						//法线
			UV0.Add(data[i].UV);								//UV
			Tangents.Add(FProcMeshTangent(0, 1, 0));			//切线暂时不需要，所以固定一个无用的值
			VertexColors.Add(FLinearColor(1.0, 1.0, 1.0, 1.0));	//顶点色暂时不需要，所以固定成白色
		}

		//调用 ProceduralMeshComponent 的接口创建三角形！
		PMC->CreateMeshSection_LinearColor(
			MeshSectionCount,	//SectionIndex
			Vertices, Indices, Normals, UV0, VertexColors, Tangents, //顶点数据列表
			true);				//bCreateCollision

		MeshSectionCount++;//递增MeshSection
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

	// Enable collision data
	PMC->ContainsPhysicsTriMeshData(true);
}
