#pragma once

#include "GameFramework/Actor.h"
#include "TestRTBuildPMActor.generated.h"

UCLASS()
class ATestRTBuildPMActor : public AActor
{
public:
	GENERATED_UCLASS_BODY()

		UPROPERTY()
		class UProceduralMeshComponent* PMC;

	/** Overridable native event for when play begins for this actor. */
	virtual void BeginPlay() override;
};
