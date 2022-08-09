// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CosmosMeasureTool.generated.h"

class UCosmosMeasureToolSphereComponent;

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

	// 预览鼠标位置的点
	UFUNCTION(Category = "Cosmos Measurement Tools")
	void PreviewLastPoint();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** 开始测量
	 * @param bMeasureComplex 测量是否使用复杂射线检测
	 * @param Distance 测量时射线检测距离
	 **/
	UFUNCTION(BlueprintCallable, Category = "Cosmos Measurement Tools")
	void StartMeasuring(bool bMeasureComplex, float Distance = 2000.0f);
	/* 结束测量 */
	UFUNCTION(BlueprintCallable, Category = "Cosmos Measurement Tools")
	void StopMeasuring();
};
