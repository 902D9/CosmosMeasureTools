// Fill out your copyright notice in the Description page of Project Settings.


#include "MeasurementTools/CosmosAreaMeasureTool.h"

#include "MeasurementTools/CosmosMeasureToolCableComponent.h"
#include "MeasurementTools/CosmosMeasureToolSphereComponent.h"

ACosmosAreaMeasureTool::ACosmosAreaMeasureTool()
{
}

void ACosmosAreaMeasureTool::BeginPlay()
{
	Super::BeginPlay();
}

void ACosmosAreaMeasureTool::PreviewLastPointAndCable()
{
	// Super::PreviewLastPointAndCable();

	FHitResult HitResult;
	if (GetHitResultUnderMouse(HitResult))
	{
		// 限制在一个平面
		PreviewPointLocation = FVector(HitResult.Location.X, HitResult.Location.Y,
		                               MeasuringLocation.Num() == 0 ? HitResult.Location.Z : MeasurePlaneZ);
		PreviewPointRelativeLocation = GetActorTransform().InverseTransformPosition(PreviewPointLocation);
		PreviewSphere->SetWorldLocation(PreviewPointLocation); // 球体位置
		if (MeasuringLocation.Num() < 3)
		{
			if (PreviewCable)
			{
				PreviewCable->EndLocation = PreviewPointRelativeLocation;
			}
		}
	}
}

void ACosmosAreaMeasureTool::CreateCable()
{
	// 前两根线正常绘制
	if (MeasuringLocation.Num() < 3)
	{
		Super::CreateCable();
	}
	// 第三根线连接第一个点
	else if (MeasuringLocation.Num() == 3)
	{
		Super::CreateCable();
		if (PreviewCable)
		{
			PreviewCable->EndLocation = MeasuringLocation[0];
			MeasuringCables.Emplace(PreviewCable);
			PreviewCable = nullptr;
		}
	}
}

void ACosmosAreaMeasureTool::PickAndPlacePointByMouse()
{
	FHitResult HitResult;
	// 判断是否拾取到点
	if (PickedSphere)
	{
		if (GetHitResultUnderMouse(HitResult))
		{
			const FVector NewLocation = FVector(HitResult.Location.X, HitResult.Location.Y, MeasurePlaneZ);
			PickedSphere->SetWorldLocation(FVector(HitResult.Location.X, HitResult.Location.Y, MeasurePlaneZ));
			const int32 Index = MeasuringPoints.Find(PickedSphere);
			MeasuringCables[Index]->SetWorldLocation(NewLocation);
			MeasuringCables[Index - 1 >= 0 ? Index - 1 : MeasuringCables.Num() - 1]->EndLocation = NewLocation;
		}
		PickedSphere = nullptr; // 清除
	}
	else
	{
		if (GetHitResultUnderMouse(HitResult))
		{
			PickedSphere = Cast<UCosmosMeasureToolSphereComponent>(HitResult.GetComponent());
			if (!PickedSphere) // 没有点击到点
			{
				// todo add new point
			}
		}
	}
}

void ACosmosAreaMeasureTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACosmosAreaMeasureTool::ClearAll_Implementation()
{
	Super::ClearAll_Implementation();
}

void ACosmosAreaMeasureTool::AddMeasuringPoint_Implementation()
{
	// Super::AddMeasuringPoint_Implementation();
	if (bMeasuring)
	{
		// 前三个点正常绘制
		if (MeasuringLocation.Num() < 3)
		{
			PreviewPointRelativeTransform = FTransform(
				FRotator::ZeroRotator,
				PreviewPointRelativeLocation,
				FVector(0.1f)
			);
			UCosmosMeasureToolSphereComponent* Point = NewObject<UCosmosMeasureToolSphereComponent>(this);
			Point->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			Point->SetRelativeTransform(PreviewPointRelativeTransform);
			Point->RegisterComponent();
			Point->SetMaxScaleDistance(TraceDistance);
			if (Material)
			{
				Point->SetMaterial(0, Material);
			}
			MeasuringPoints.Emplace(Point); // 数组保存
			MeasuringLocation.Emplace(Point->K2_GetComponentLocation());
			CreateCable();
		}
		else
		{
			PickAndPlacePointByMouse();
		}


		GetMeasureResult();
		// 不是开始测量后的第一个点 , 添加UI
		if (!bIsFirstPointAfterStartMeasuring)
		{
			AddDisplayUI();
		}
		else
		{
			bIsFirstPointAfterStartMeasuring = false;
		}
	}
}

void ACosmosAreaMeasureTool::GetMeasureResult()
{
	Super::GetMeasureResult();
	if (bMeasuring)
	{
		switch (MeasuringLocation.Num())
		{
		case 1:
			if (bIsFirstPointAfterStartMeasuring)
			{
				MeasurePlaneZ = MeasuringLocation[0].Z;
			}
		case 2:
		case 3:
		default: ;
		}
	}
}
