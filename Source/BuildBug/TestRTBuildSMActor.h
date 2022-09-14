#pragma once

#include "GameFramework/Actor.h"
#include "TestRTBuildSMActor.generated.h"

UCLASS()
class ATestRTBuildSMActor : public AActor
{
public:
	GENERATED_UCLASS_BODY()

		UPROPERTY()
		UStaticMeshComponent* _smComp;

	/** Overridable native event for when play begins for this actor. */
	virtual void BeginPlay() override;
};
