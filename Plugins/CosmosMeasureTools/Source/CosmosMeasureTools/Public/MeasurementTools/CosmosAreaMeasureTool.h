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
	/**
	* 插入到数组，返回Index
	* 1.找到数组内最近点A
	* 2.找到与最近点A相邻的最近点B
	*/
	void SaveNewPoint(UCosmosMeasureToolSphereComponent* NewPoint, FVector NewLocation,
	                  int32& PreIndex, int32& NextIndex);
	/**
	 * NewLocation 最近的现有点相邻两点中离 NewLocation 最近一点的下标
	 */
	int32 GetClosestConnectPointIndex(FVector NewLocation, int32 ClosestIndex);
	/**
	* 判断两点组成的线段是否和 已存在的线段 相交（2D）, 忽略与自身（PointIndex）相连的线段
	* @param NewPointLocation 新增点的位置
	* @param PointIndex 已有点的位置
	*/
	bool IsIntersectAnExistingLine(FVector NewPointLocation, int32 PointIndex);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void StopMeasuring() override;
	virtual void AddMeasuringPoint_Implementation() override;
	virtual void GetMeasureResult() override;
};
