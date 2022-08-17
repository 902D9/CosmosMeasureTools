// Fill out your copyright notice in the Description page of Project Settings.


#include "MeasurementTools/CosmosAreaMeasureTool.h"

#include "CosmosMeasureToolsBPLibrary.h"
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
			// 更新点位置
			PickedSphere->SetWorldLocation(NewLocation);
			const int32 Index = MeasuringPoints.Find(PickedSphere);
			MeasuringLocation[Index] = NewLocation; // 保存新位置
			// 更新相连线位置
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
			if (!PickedSphere) // 没有点击到已存在的点
			{
				// 添加点和线段
				const FVector NewLocation = FVector(HitResult.Location.X, HitResult.Location.Y, MeasurePlaneZ);
				UCosmosMeasureToolSphereComponent* Point = NewObject<UCosmosMeasureToolSphereComponent>(this);
				Point->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
				Point->SetWorldLocation(NewLocation);
				Point->RegisterComponent();
				Point->SetMaxScaleDistance(TraceDistance);
				if (Material)
				{
					Point->SetMaterial(0, Material);
				}
				/**
				 * 插入到数组
				 * 1.找到数组内最近且不相交其他线的点A
				 * 2.找到与最近点A相邻的最近不相交其他相邻线的点B
				 */
				int32 PreIndex;
				int32 NextIndex;
				SaveNewPoint(Point, NewLocation, PreIndex, NextIndex);
				// UE_LOG(LogTemp, Log, TEXT("PreIndex %d ; NextIndex %d"), PreIndex, NextIndex);
				if (PreIndex != -1 && NextIndex != -1)
				{
					// 更新相连线位置
					MeasuringCables[PreIndex]->EndLocation = NewLocation;
					// 添加一条连线
					UCosmosMeasureToolCableComponent* NewCable = NewObject<UCosmosMeasureToolCableComponent>(this);
					NewCable->SetWorldLocation(NewLocation); // 设置起始位
					NewCable->EndLocation = MeasuringLocation[NextIndex]; // 设置结束位置
					NewCable->SetMaxScaleDistance(TraceDistance); // 设置缩放生效范围
					if (Material)
					{
						NewCable->SetMaterial(0, Material);
					}
					NewCable->RegisterComponent();
					// MeasuringCables.Insert(NewCable, NextIndex - 1); // 插入数组
					MeasuringCables.Insert(NewCable, PreIndex + 1);
				}
			}
		}
	}
}

void ACosmosAreaMeasureTool::SaveNewPoint(UCosmosMeasureToolSphereComponent* NewPoint, FVector NewLocation,
                                          int32& PreIndex, int32& NextIndex)
{
	// 距离新增点最近的现有点下标
	int32 ClosestIndex = 0;
	// 距离新增点最近的现有点距离（2D）
	float MinDistance = (NewLocation - MeasuringLocation[ClosestIndex]).Size2D();
	// 获得最近的现有点下标
	for (int i = 0; i < MeasuringLocation.Num(); i++)
	{
		// 不与任何已存在的连线相交 才作数
		if (!IsIntersectAnExistingLine(NewLocation, i))
		{
			const float CurDistance = (NewLocation - MeasuringLocation[i]).Size2D();
			if (MinDistance > CurDistance)
			{
				MinDistance = CurDistance;
				ClosestIndex = i;
			}
		}
	}

	const int32 ClosestConnectPointIndex = GetClosestConnectPointIndex(NewLocation, ClosestIndex);
	// UE_LOG(LogTemp, Log, TEXT("ClosestIndex %d ; ClosestConnectPointIndex %d"),
	//        ClosestIndex, ClosestConnectPointIndex);

	if (ClosestConnectPointIndex != -1)
	{
		PreIndex = FMath::Min(ClosestIndex, ClosestConnectPointIndex);
		NextIndex = FMath::Max(ClosestIndex, ClosestConnectPointIndex);
		// UE_LOG(LogTemp, Log, TEXT("Between %d %d"), PreIndex, NextIndex);
		// 特殊情况：一点为0 一点为last，即在Loop的位置
		if (PreIndex == 0 && NextIndex == MeasuringLocation.Num() - 1)
		{
			PreIndex = NextIndex;
			NextIndex = 0;
			// 插入数组
			MeasuringPoints.Insert(NewPoint, PreIndex + 1);
			MeasuringLocation.Insert(NewLocation, PreIndex + 1);
		}
		else
		{
			// 插入数组
			MeasuringPoints.Insert(NewPoint, NextIndex);
			MeasuringLocation.Insert(NewLocation, NextIndex);
			// 继续指向下一个
			NextIndex = NextIndex + 1 <= MeasuringLocation.Num() - 1 ? NextIndex + 1 : 0;
		}
	}
	else
	{
		PreIndex = NextIndex = -1;
	}
}

int32 ACosmosAreaMeasureTool::GetClosestConnectPointIndex(FVector NewLocation, int32 ClosestIndex)
{
	// 获得距 最近的现有点 相邻两点距离，考虑到Loop
	const int32 ClosestNextIndex = ClosestIndex + 1 <= MeasuringLocation.Num() - 1 ? ClosestIndex + 1 : 0;
	const int32 ClosestPreIndex = ClosestIndex - 1 >= 0 ? ClosestIndex - 1 : MeasuringCables.Num() - 1;

	// 新增点与最近的现有点的下一点连线 是否与 现存线段存在相交
	const bool bNextIndexCross = IsIntersectAnExistingLine(NewLocation, ClosestNextIndex);
	// 新增点与最近的现有点的上一点连线 是否与 现存线段存在相交
	const bool bPreIndexCross = IsIntersectAnExistingLine(NewLocation, ClosestPreIndex);
	// UE_LOG(LogTemp, Log, TEXT("bNextIndexCrossPre %hs ; bPreIndexCrossNext %hs"),
	//        bNextIndexCross? "True" : "False", bPreIndexCross? "True" : "False");
	if (!bNextIndexCross && !bPreIndexCross) // 都不相交
	{
		const float NextDistance = (NewLocation - MeasuringLocation[ClosestNextIndex]).Size2D();
		const float PreDistance = (NewLocation - MeasuringLocation[ClosestPreIndex]).Size2D();
		// 获得距相邻两点较近的一个
		return NextDistance < PreDistance ? ClosestNextIndex : ClosestPreIndex;
	}
	if (!bPreIndexCross)
	{
		return ClosestPreIndex;
	}
	if (!bNextIndexCross)
	{
		return ClosestNextIndex;
	}
	return -1;
}

bool ACosmosAreaMeasureTool::IsIntersectAnExistingLine(FVector NewPointLocation, int32 PointIndex)
{
	for (int i = 0; i < MeasuringLocation.Num(); i++)
	{
		const int NextIndex = i + 1 < MeasuringLocation.Num() ? i + 1 : 0;
		if (i != PointIndex && NextIndex != PointIndex)
		{
			if (UCosmosMeasureToolsBPLibrary::CheckWhetherTwoLineSegmentsIntersect2D(
				{FVector2D(NewPointLocation), FVector2D(MeasuringLocation[PointIndex])},
				{
					FVector2D(MeasuringLocation[i]),
					FVector2D(MeasuringLocation[NextIndex])
				}))
			{
				return true;
			}
		}
	}
	return false;
}

void ACosmosAreaMeasureTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACosmosAreaMeasureTool::StopMeasuring()
{
	Super::StopMeasuring();
	// 不足三个点，清空
	if (MeasuringLocation.Num() < 3)
	{
		// 清空存储的位置
		MeasuringLocation.Empty();
		// 销毁所有点
		for (const auto& Point : MeasuringPoints)
		{
			if (Point)
			{
				Point->DestroyComponent();
			}
		}
		MeasuringPoints.Empty();
		// 销毁所有线
		for (const auto& Cable : MeasuringCables)
		{
			if (Cable)
			{
				Cable->DestroyComponent();
			}
		}
		MeasuringCables.Empty();
	}
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
		if (MeasuringLocation.Num() == 1)
		{
			if (bIsFirstPointAfterStartMeasuring)
			{
				MeasurePlaneZ = MeasuringLocation[0].Z;
			}
		}
		if (MeasuringLocation.Num() > 2)
		{
			MeasuredArea = UCosmosMeasureToolsBPLibrary::MeasurePolyArea(MeasuringLocation) / 10000.0f;
			UE_LOG(LogTemp, Log, TEXT("MeasuredArea %f"), MeasuredArea);
		}
	}
}
