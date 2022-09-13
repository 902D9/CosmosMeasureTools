// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CableComponent.h"
#include "CosmosMeasureToolCableComponent.generated.h"

/**
 * 
 */
UCLASS()
class COSMOSMEASURETOOLS_API UCosmosMeasureToolCableComponent : public UCableComponent
{
	GENERATED_BODY()

public:
	UCosmosMeasureToolCableComponent();

	virtual void BeginPlay() override;

	virtual void ApplyWorldOffset(const FVector& InOffset, bool bWorldShift) override;

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
	float BaseCableScale;

private:
	// 根据距摄像头距离缩放
	void ScaleCableByDistanceToCamera();

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// 设置新的缩放范围Max值
	UFUNCTION()
	void SetMaxScaleDistance(float NewMaxScaleDistance);
};

