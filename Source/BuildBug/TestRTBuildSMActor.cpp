#include"TestRTBuildSMActor.h"

#include "MeshDescription.h"
#include "MeshDescriptionBuilder.h"
#include "StaticMeshAttributes.h"



ATestRTBuildSMActor::ATestRTBuildSMActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//���� StaticMeshComponent ����
	_smComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("smComp"), false);

	//�趨��Ϊ�������
	SetRootComponent(_smComp);
}

void ATestRTBuildSMActor::BeginPlay()
{
	// ����һ���򵥵Ľ�����

	// ���� StaticMesh ��Դ
	UStaticMesh* staticMesh = NewObject<UStaticMesh>(this);
	staticMesh->GetStaticMaterials().Add(FStaticMaterial());

	// MeshDescription ��������StaticMesh����Ϣ���������Σ�UV������ ��
	FMeshDescription meshDesc;

	FStaticMeshAttributes Attributes(meshDesc);
	Attributes.Register();

	//Ϊ MeshDescription ����һ�� MeshDescriptionBuilder Э����������
	FMeshDescriptionBuilder meshDescBuilder;
	meshDescBuilder.SetMeshDescription(&meshDesc);
	meshDescBuilder.EnablePolyGroups();
	meshDescBuilder.SetNumUVLayers(1);

	//ʹ�� MeshDescriptionBuilder ���������ݣ�
	{
		// ���� 5 ������ 
		TArray< FVertexID > vertexIDs;
		vertexIDs.SetNum(5);

		//ָ�� 5 ��������Ե�λ�ã�
		vertexIDs[0] = meshDescBuilder.AppendVertex(FVector(0.0f, 0.0f, 100.0f));		// ����
		vertexIDs[1] = meshDescBuilder.AppendVertex(FVector(-50.0f, 50.0f, 0.0f));		// �� 1
		vertexIDs[2] = meshDescBuilder.AppendVertex(FVector(-50.0f, -50.0f, 0.0f));	// �� 2
		vertexIDs[3] = meshDescBuilder.AppendVertex(FVector(50.0f, -50.0f, 0.0f));		// �� 3
		vertexIDs[4] = meshDescBuilder.AppendVertex(FVector(50.0f, 50.0f, 0.0f));		// �� 4

		// ����һ�� polygon group
		FPolygonGroupID polygonGroup = meshDescBuilder.AppendPolygonGroup();

		//Ϊ�˷��㣬����һ���ṹ������װ��������һ���������Ϣ
		struct VertexInfo
		{
			int ID;					//��Ӧ�Ķ���ID
			FVector InstanceNormal;	//���������ж���ķ���
			FVector2D InstanceUV;	//���������ж����UV
			VertexInfo(int InID, FVector InInstanceNormal, FVector2D InInstanceUV)
				:ID(InID), InstanceNormal(InInstanceNormal), InstanceUV(InInstanceUV)
			{
			}
		};

		//Ϊ�˷��㣬����һ���ֲ������������ٸ��ݸ����Ķ�����Ϣ����������
		auto AppendTriangle = [&meshDescBuilder, &vertexIDs, polygonGroup](TArray<VertexInfo> vertex)
		{
			TArray< FVertexInstanceID > vertexInsts;//�������е�ÿ������
			for (int i = 0; i < 3; i++)
			{
				FVertexInstanceID instance = meshDescBuilder.AppendInstance(vertexIDs[vertex[i].ID]);
				meshDescBuilder.SetInstanceNormal(instance, vertex[i].InstanceNormal);			//����
				meshDescBuilder.SetInstanceUV(instance, vertex[i].InstanceUV, 0);				//UV
				meshDescBuilder.SetInstanceColor(instance, FVector4f(1.0f, 1.0f, 1.0f, 1.0f));	//����ɫ
				vertexInsts.Add(instance);
			}
			//���Ӵ�������
			meshDescBuilder.AppendTriangle(vertexInsts[0], vertexInsts[1], vertexInsts[2], polygonGroup);
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
	}

	// MeshDescription ����
	UStaticMesh::FBuildMeshDescriptionsParams mdParams;
	mdParams.bBuildSimpleCollision = true;
	mdParams.bFastBuild = true;

	// ����һ���б����� MeshDescription������Ϊ BuildFromMeshDescriptions �����Ĳ�����Ҫ�б�
	TArray<const FMeshDescription*> meshDescPtrs;
	meshDescPtrs.Emplace(&meshDesc);

	//������
	staticMesh->BuildFromMeshDescriptions(meshDescPtrs, mdParams);

	// �� StaticMesh ָ���� StaticMeshComponent���
	_smComp->SetStaticMesh(staticMesh);
}
