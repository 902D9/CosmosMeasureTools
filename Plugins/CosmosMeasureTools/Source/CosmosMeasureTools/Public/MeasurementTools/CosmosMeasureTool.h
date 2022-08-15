// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CosmosMeasureTool.generated.h"

class UCosmosMeasureToolSphereComponent;
class UCosmosMeasureToolCableComponent;

UCLASS(Blueprintable, ClassGroup="Cosmos Measurement Tools")
class COSMOSMEASURETOOLS_API ACosmosMeasureTool : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACosmosMeasureTool(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneComponent* Root;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCosmosMeasureToolSphereComponent* PreviewSphere;

	// 是否正在测量
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	bool bMeasuring;
	// 测量是否使用复杂射线检测
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	bool bTraceComplex;
	// 测量时射线检测距离
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	float TraceDistance;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	UMaterialInterface* Material;

	// 预览鼠标位置的点
	UFUNCTION(Category = "Cosmos Measurement Tools")
	void PreviewLastPoint();
	// 预览鼠标位置的点的位置(世界)
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	FVector PreviewPointLocation;
	// 预览鼠标位置的点的位置(相对)
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	FVector PreviewPointRelativeLocation;
	// 预览鼠标位置的点的变换(相对)
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	FTransform PreviewPointRelativeTransform;
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	TArray<UCosmosMeasureToolSphereComponent*> MeasuringPoints;

	/* 添加测量点间连线 */
	UFUNCTION(Category = "Cosmos Measurement Tools")
	void CreateCable();
	// 预览测量点间连线
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	UCosmosMeasureToolCableComponent* PreviewCable;
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	TArray<UCosmosMeasureToolCableComponent*> MeasuringCables;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** 开始测量
	 * @param bMeasureComplex 测量是否使用复杂射线检测
	 * @param Distance 测量时射线检测距离
	 **/
	UFUNCTION(BlueprintCallable, Category = "Cosmos Measurement Tools")
	virtual void StartMeasuring(bool bMeasureComplex, float Distance = 2000.0f);
	/* 结束测量 */
	UFUNCTION(BlueprintCallable, Category = "Cosmos Measurement Tools")
	virtual void StopMeasuring();

	/* 清除 */
	UFUNCTION(BlueprintCallable, Category = "Cosmos Measurement Tools")
	virtual void ClearAll();

	/* 添加测量点 */
	UFUNCTION(BlueprintCallable, Category = "Cosmos Measurement Tools")
	virtual void AddMeasuringPoint();
};
