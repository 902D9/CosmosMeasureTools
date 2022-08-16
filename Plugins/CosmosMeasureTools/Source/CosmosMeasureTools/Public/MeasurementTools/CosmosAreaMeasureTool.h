// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeasurementTools/CosmosDistanceMeasureTool.h"
#include "CosmosAreaMeasureTool.generated.h"

/**
 * 
 */
UCLASS()
class COSMOSMEASURETOOLS_API ACosmosAreaMeasureTool : public ACosmosDistanceMeasureTool
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACosmosAreaMeasureTool();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 由第一个点确定得到测量平面 - Z值
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	float MeasurePlaneZ;

	virtual void PreviewLastPointAndCable() override;
	virtual void CreateCable() override;

	// 未拾取到点则尝试拾取，若已经拾取到点则更新位置
	void PickAndPlacePointByMouse();
	UPROPERTY()
	UCosmosMeasureToolSphereComponent* PickedSphere;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ClearAll_Implementation() override;
	virtual void AddMeasuringPoint_Implementation() override;
	virtual void GetMeasureResult() override;
};
