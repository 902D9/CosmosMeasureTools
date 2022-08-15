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

	virtual void BeginPlay() override;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	APlayerController* PlayerController;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	bool bEnableScaleByDistance;
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	float MinScaleDistance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cosmos Measurement Tools",
		meta=(EditCondition="bEnableScaleByDistance"))
	float MaxScaleDistance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cosmos Measurement Tools",
		meta=(EditCondition="bEnableScaleByDistance"))
	float BaseScale;
	
private:
	UPROPERTY()
	UStaticMesh* MeshAsset;

	// 根据距摄像头距离缩放
	void ScaleByDistanceToCamera();

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	// 设置新的缩放范围Max值
	UFUNCTION()
	void SetMaxScaleDistance(float NewMaxScaleDistance);

};
