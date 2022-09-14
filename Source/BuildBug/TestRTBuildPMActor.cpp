#include "TestRTBuildPMActor.h"
#include "ProceduralMeshComponent.h"


ATestRTBuildPMActor::ATestRTBuildPMActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//���� ProceduralMeshComponent ����
	PMC = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("myPMC"), false);

	//�趨��Ϊ�������
	SetRootComponent(PMC);
}

void ATestRTBuildPMActor::BeginPlay()
{
	UE_LOG(LogTemp, Log, TEXT("Startpmc"));
	//ָ�� 5 ��������Ե�λ�ã�
	TArray<FVector> VertexPositions;
	VertexPositions.Add(FVector(0.0, 0.0, 100.0));		// ����
	VertexPositions.Add(FVector(-50.0, 50.0, 0.0));		// �� 1
	VertexPositions.Add(FVector(-50.0, -50.0, 0.0));	// �� 2
	VertexPositions.Add(FVector(50.0, -50.0, 0.0));		// �� 3
	VertexPositions.Add(FVector(50.0, 50.0, 0.0));		// �� 4

	//Ϊ�˷��㣬����һ���ṹ������װ��������һ���������Ϣ
	struct VertexInfo
	{
		int ID;				//��Ӧ�Ķ���ID
		FVector Normal;		//����
		FVector2D UV;		//UV
		VertexInfo(int InID, FVector InNormal, FVector2D InUV)
			:ID(InID), Normal(InNormal), UV(InUV)
		{
		}
	};

	//��ǰMeshSection����Ŀ
	int MeshSectionCount = 0;

	//Ϊ�˷��㣬����һ���ֲ������������ٸ��ݸ����Ķ�����Ϣ����������
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
			Vertices.Add(VertexPositions[data[i].ID]);			//λ��
			Indices.Add(i);										//����
			Normals.Add(data[i].Normal);						//����
			UV0.Add(data[i].UV);								//UV
			Tangents.Add(FProcMeshTangent(0, 1, 0));			//������ʱ����Ҫ�����Թ̶�һ�����õ�ֵ
			VertexColors.Add(FLinearColor(1.0, 1.0, 1.0, 1.0));	//����ɫ��ʱ����Ҫ�����Թ̶��ɰ�ɫ
		}

		//���� ProceduralMeshComponent �Ľӿڴ��������Σ�
		PMC->CreateMeshSection_LinearColor(
			MeshSectionCount,	//SectionIndex
			Vertices, Indices, Normals, UV0, VertexColors, Tangents, //���������б�
			true);				//bCreateCollision

		MeshSectionCount++;//����MeshSection
	};

	//�ĸ�������ݣ�

	// �� 1 (���� -X) �����͵Ķ������ݣ�
	AppendTriangle({
		VertexInfo(0,FVector(-0.7071, 0, 0.7071),FVector2D(0, 1)),
		VertexInfo(2,FVector(-0.7071, 0, 0.7071),FVector2D(1, 0)),
		VertexInfo(1,FVector(-0.7071, 0, 0.7071),FVector2D(0, 0)) });
	// �� 2 (���� -Y) �����͵Ķ������ݣ�
	AppendTriangle({
		VertexInfo(0,FVector(0, -0.7071, 0.7071),FVector2D(0, 1)),
		VertexInfo(3,FVector(0, -0.7071, 0.7071),FVector2D(1, 0)),
		VertexInfo(2,FVector(0, -0.7071, 0.7071),FVector2D(0, 0)) });
	// �� 3 (���� +X) �����͵Ķ������ݣ�
	AppendTriangle({
		VertexInfo(0,FVector(0.7071, 0, 0.7071),FVector2D(0, 1)),
		VertexInfo(4,FVector(0.7071, 0, 0.7071),FVector2D(1, 0)),
		VertexInfo(3,FVector(0.7071, 0, 0.7071),FVector2D(0, 0)) });
	// �� 4 (���� +Y) �����͵Ķ������ݣ�
	AppendTriangle({
		VertexInfo(0,FVector(0, 0.7071, 0.7071),FVector2D(0, 1)),
		VertexInfo(1,FVector(0, 0.7071, 0.7071),FVector2D(1, 0)),
		VertexInfo(4,FVector(0, 0.7071, 0.7071),FVector2D(0, 0)) });

	// Enable collision data
	PMC->ContainsPhysicsTriMeshData(true);
}