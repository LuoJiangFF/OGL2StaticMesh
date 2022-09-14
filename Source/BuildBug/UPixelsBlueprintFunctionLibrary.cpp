// Fill out your copyright notice in the Description page of Project Settings.


#include "UPixelsBlueprintFunctionLibrary.h"
#include "Utility.h"
#include <AssetRegistryModule.h>
#include "UObject/ConstructorHelpers.h"


UStaticMesh* UPixelsBlueprintFunctionLibrary::NewStaticMesh(const FString& oglPath,const FStaticMaterial& sMat,  int64 geometryId, const uint8& oglBuffer, int32 length)
{
	FString meshName = FString::Printf(TEXT("%d"), geometryId);
	FString packageName = "/Game/Geometry/" + meshName;

	UPackage* package = CreatePackage(nullptr, *packageName);
	UStaticMesh* mesh = NewObject<UStaticMesh>(package, FName(*meshName), RF_Public | RF_Standalone| RF_Transactional);

	uint8* bufferPtr = const_cast<uint8*>(&oglBuffer);
	int subMeshes = FillStaticMeshWithOGL(*mesh, bufferPtr, length);
	mesh->GetStaticMaterials().Add(sMat);

	package->SetDirtyFlag(true);
	FAssetRegistryModule::AssetCreated(mesh);
	return mesh;
}
