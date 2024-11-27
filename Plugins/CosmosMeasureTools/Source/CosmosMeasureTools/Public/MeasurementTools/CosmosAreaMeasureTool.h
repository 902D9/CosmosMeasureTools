// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MeasurementTools/CosmosDistanceMeasureTool.h"
#include "CosmosAreaMeasureTool.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, ClassGroup="Cosmos Measurement Tools")
class COSMOSMEASURETOOLS_API ACosmosAreaMeasureTool : public ACosmosDistanceMeasureTool
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACosmosAreaMeasureTool(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void ApplyWorldOffset(const FVector& InOffset, bool bWorldShift) override;

	// 点击判断是否拾取到点 时的击中结果，需要应用世界坐标偏移
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	FHitResult PickAndPlacePointByMouseHitResult;
	// 判断是否拾取到点时 点击到某一点的世界坐标位置，需要应用世界坐标偏移
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	FVector PickedLocation;
	// 插入到数组点坐标，需要应用世界坐标偏移
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	FVector NewSaveNewPointLocation;
	// 插入到数组点坐标，需要应用世界坐标偏移
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	FVector IsIntersectAnExistingLineNewPointLocation;
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	FVector NewPointLocation;

	// 测量得到的面积
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	float MeasuredArea = 0.0f;

	virtual void PreviewLastPointAndCable() override;
	virtual void CreateCable() override;

	// 未拾取到点则尝试拾取，若已经拾取到点则更新位置
	void PickAndPlacePointByMouse();
	UPROPERTY()
	UCosmosMeasureToolSphereComponent* PickedSphere = nullptr;
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
	* @param InNewPointLocation 新增点的位置
	* @param PointIndex 已有点的位置
	*/
	bool IsIntersectAnExistingLine(FVector InNewPointLocation, int32 PointIndex);

private:
	// 绘制面积填充的画布
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components",
		meta = (AllowPrivateAccess="true"))
	UStaticMeshComponent* CanvasMesh;
	// 绘制面积填充的画布
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components",
		meta = (AllowPrivateAccess="true"))
	UDecalComponent* CanvasDecal;
	// 绘制面积填充的材质
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components",
		meta = (AllowPrivateAccess="true"))
	UMaterialInstanceDynamic* CanvasMaterial;
	// 绘制面积填充的渲染对象
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components",
		meta = (AllowPrivateAccess="true"))
	UTextureRenderTarget2D* CanvasRenderTarget;

	// 填充的颜色
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components",
		meta = (AllowPrivateAccess="true", ExposeOnSpawn))
	FLinearColor CanvasColor = FLinearColor::Red;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void StartMeasuring(bool bMeasureComplex, float Distance,
	                            TEnumAsByte<ECollisionChannel> ChannelToTrace) override;
	virtual void StopMeasuring() override;
	virtual void ClearAll_Implementation() override;
	virtual void AddMeasuringPoint_Implementation() override;
	virtual void GetMeasureResult() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cosmos Measurement Tools")
	float GetMeasuredArea();
};
