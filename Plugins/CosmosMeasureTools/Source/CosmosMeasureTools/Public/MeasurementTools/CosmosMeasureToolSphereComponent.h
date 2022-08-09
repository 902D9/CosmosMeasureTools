// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "CosmosMeasureToolSphereComponent.generated.h"

/**
 * 球体根据距离缩放
 */
UCLASS(Blueprintable, ClassGroup="Cosmos Measurement Tools")
class COSMOSMEASURETOOLS_API UCosmosMeasureToolSphereComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UCosmosMeasureToolSphereComponent();

protected:
private:
	UPROPERTY()
	UStaticMesh* MeshAsset;
};
