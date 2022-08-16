// Fill out your copyright notice in the Description page of Project Settings.


#include "MeasurementTools/CosmosDistanceMeasureTool.h"
#include "MeasurementTools/CosmosMeasureToolSphereComponent.h"
// #include "CosmosMeasureTools.h"

// DECLARE_LOG_CATEGORY_EXTERN(CosmosMeasurementTools, Log, All);

// Sets default values
ACosmosDistanceMeasureTool::ACosmosDistanceMeasureTool()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACosmosDistanceMeasureTool::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACosmosDistanceMeasureTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACosmosDistanceMeasureTool::ClearAll_Implementation()
{
	Super::ClearAll_Implementation();
	MeasuredDistance.Empty();
}

void ACosmosDistanceMeasureTool::GetMeasureResult()
{
	Super::GetMeasureResult();
	// 测量中 && 不是开始测量后的第一个点 && 存在至少两个点
	if (bMeasuring && !bIsFirstPointAfterStartMeasuring && MeasuringLocation.IsValidIndex(1))
	{
		const FVector LastTwoPoint = MeasuringLocation.Last() - MeasuringLocation.Last(1);
		MeasuredDistance.Add(LastTwoPoint.Size() / 100.0f);
		UE_LOG(LogTemp, Log, TEXT("MeasuredDistance %f"), LastTwoPoint.Size() / 100.0f);
	}
}
