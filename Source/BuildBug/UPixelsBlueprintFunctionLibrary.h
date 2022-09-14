// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UPixelsBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UPixelsBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "PixelBlueprint")
		static UStaticMesh* NewStaticMesh(const FString& oglPath, const FStaticMaterial& sMat,  int64 geometryId, const uint8& oglBuffer, int32 length);

};
