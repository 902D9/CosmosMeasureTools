// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CosmosMeasureTool.generated.h"

UENUM(BlueprintType)
enum class EMeasureType:uint8
{
	// 距离测量
	Distance UMETA(DisplayName="Distance"),
	// 面积测量
	Area UMETA(DisplayName="Area"),
};

// 测量结果展现形式
UENUM(BlueprintType)
enum class EMeasureResultDisplayType:uint8
{
	// 世界内展现
	World UMETA(DisplayName="World"),
	// 使用 UI 展现
	Screen UMETA(DisplayName="Screen"),
};

class UCosmosMeasureToolSphereComponent;
class UCosmosMeasureToolCableComponent;

UCLASS(Abstract, Blueprintable, ClassGroup="Cosmos Measurement Tools")
class COSMOSMEASURETOOLS_API ACosmosMeasureTool : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACosmosMeasureTool(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void ApplyWorldOffset(const FVector& InOffset, bool bWorldShift) override;

	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	APlayerController* PlayerController;

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
	// 测量时射线检测通道
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	TEnumAsByte<ECollisionChannel> TraceChannel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	UMaterialInterface* Material;

	bool GetHitResultUnderMouse(FHitResult& HitResult);

	// 预览鼠标位置的点
	UFUNCTION(Category = "Cosmos Measurement Tools")
	virtual void PreviewLastPointAndCable();
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
	// 测量位置
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	TArray<FVector> MeasuringLocation;

	// 标记是否是开始测量后添加的第一个点
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	bool bIsFirstPointAfterStartMeasuring;

	/* 添加测量点间连线 */
	UFUNCTION(Category = "Cosmos Measurement Tools")
	virtual void CreateCable();
	// 预览测量点间连线
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	UCosmosMeasureToolCableComponent* PreviewCable;
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	TArray<UCosmosMeasureToolCableComponent*> MeasuringCables;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cosmos Measurement Tools", meta=(ExposeOnSpawn))
	EMeasureResultDisplayType MeasureResultDisplayType = EMeasureResultDisplayType::Screen;
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools")
	EMeasureType MeasureType;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** 开始测量
	 * @param bMeasureComplex 测量是否使用复杂射线检测
	 * @param Distance 测量时射线检测距离
	 * @param ChannelToTrace 测量时射线检测通道
	 **/
	UFUNCTION(BlueprintCallable, Category = "Cosmos Measurement Tools")
	virtual void StartMeasuring(bool bMeasureComplex, float Distance = 2000.0f,
	                            TEnumAsByte<ECollisionChannel> ChannelToTrace = ECC_Visibility);
	/* 结束测量 */
	UFUNCTION(BlueprintCallable, Category = "Cosmos Measurement Tools")
	virtual void StopMeasuring();

	/* 清除 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cosmos Measurement Tools")
	void ClearAll();

	/* 添加测量点 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cosmos Measurement Tools")
	void AddMeasuringPoint();

	/* 获得测量结果 */
	UFUNCTION(BlueprintCallable, Category = "Cosmos Measurement Tools")
	virtual void GetMeasureResult();

	/* 添加测量结果 UI */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Cosmos Measurement Tools")
	void AddDisplayUI();

	/* 获取某一点测量线段的世界位置 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cosmos Measurement Tools")
	virtual FVector GetMeasuringLocationAtIndex(float Index);

	/* 获取最后一测量线段的世界位置 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Cosmos Measurement Tools")
	virtual FVector GetLastMeasuringLocation();
};
