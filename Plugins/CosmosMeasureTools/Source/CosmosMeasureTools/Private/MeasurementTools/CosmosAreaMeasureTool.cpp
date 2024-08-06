// Fill out your copyright notice in the Description page of Project Settings.


#include "MeasurementTools/CosmosAreaMeasureTool.h"

#include "CosmosMeasureToolsBPLibrary.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "MeasurementTools/CosmosMeasureToolCableComponent.h"
#include "MeasurementTools/CosmosMeasureToolSphereComponent.h"

ACosmosAreaMeasureTool::ACosmosAreaMeasureTool(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeasureType = EMeasureType::Area;
	CanvasMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CanvasMesh"));
	CanvasMesh->SetupAttachment(RootComponent);
	CanvasMesh->SetVisibility(false);
	CanvasMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	UStaticMesh* MeshAsset = LoadObject<UStaticMesh>(nullptr,TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	CanvasMesh->SetStaticMesh(MeshAsset);
}

void ACosmosAreaMeasureTool::BeginPlay()
{
	Super::BeginPlay();

	// 创建动态材质示例
	UMaterial* MaterialAsset = LoadObject<UMaterial>(
		nullptr,TEXT("Material'/CosmosMeasureTools/Materials/Master/M_MeasureAreaMask.M_MeasureAreaMask'"));
	CanvasMaterial = UMaterialInstanceDynamic::Create(MaterialAsset, this);
	CanvasMesh->SetMaterial(0, CanvasMaterial);
	CanvasRenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, 1024, 1024, RTF_RGBA16f,
	                                                                   FLinearColor(0, 0, 0, 0));
	CanvasMaterial->SetTextureParameterValue("CanvasTexture", CanvasRenderTarget);
	CanvasMaterial->SetVectorParameterValue("Color", FLinearColor::Red);
}

void ACosmosAreaMeasureTool::ApplyWorldOffset(const FVector& InOffset, bool bWorldShift)
{
	Super::ApplyWorldOffset(InOffset, bWorldShift);

	// todo: 修复世界坐标偏移导致 世界 cable 显示错误
	UE_LOG(LogTemp, Log, TEXT("MeasuredArea BEGIN ApplyWorldOffset [%s], [%s], [%s], [%s], [%s]"),
	       *PickAndPlacePointByMouseHitResult.Location.ToString(), *PickedLocation.ToString(),
	       *NewSaveNewPointLocation.ToString(), *IsIntersectAnExistingLineNewPointLocation.ToString(),
	       *NewPointLocation.ToString());
	MeasurePlaneZ += InOffset.Z;
	PickAndPlacePointByMouseHitResult.Location += InOffset;
	PickedLocation += InOffset;
	NewSaveNewPointLocation += InOffset;
	IsIntersectAnExistingLineNewPointLocation += InOffset;
	NewPointLocation += InOffset;
	UE_LOG(LogTemp, Log, TEXT("MeasuredArea END ApplyWorldOffset [%s], [%s], [%s], [%s], [%s]"),
	       *PickAndPlacePointByMouseHitResult.Location.ToString(), *PickedLocation.ToString(),
	       *NewSaveNewPointLocation.ToString(), *IsIntersectAnExistingLineNewPointLocation.ToString(),
	       *NewPointLocation.ToString());
}

void ACosmosAreaMeasureTool::PreviewLastPointAndCable()
{
	// Super::PreviewLastPointAndCable();

	FHitResult HitResult; // 因为是在 Tick 中调用，所以 HitResult.Location 不需要应用世界坐标偏移
	if (GetHitResultUnderMouse(HitResult))
	{
		// 限制在一个平面
		// @todo: 直接修改Z值会让预览点的位置不除在鼠标处
		PreviewPointLocation = FVector(HitResult.Location.X, HitResult.Location.Y,
		                               MeasuringLocation.Num() == 0 ? HitResult.Location.Z : MeasurePlaneZ);
		PreviewSphere->SetWorldLocation(PreviewPointLocation); // 球体位置
		PreviewPointRelativeLocation = GetActorTransform().InverseTransformPosition(PreviewPointLocation);
		// Cable仅预览前三个点
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
	// 判断是否拾取到点
	if (PickedSphere) // 如果有拾取到的点则更新点和连线的位置
	{
		if (GetHitResultUnderMouse(PickAndPlacePointByMouseHitResult))
		{
			PickedLocation = FVector(PickAndPlacePointByMouseHitResult.Location.X,
			                         PickAndPlacePointByMouseHitResult.Location.Y, MeasurePlaneZ);
			// 更新拾取到点的位置
			PickedSphere->SetWorldLocation(PickedLocation);
			const int32 Index = MeasuringPoints.Find(PickedSphere);
			if (Index > -1)
			{
				MeasuringLocation[Index] = PickedLocation; // 保存新位置
				// 更新相连线位置
				switch (MeasureResultDisplayType)
				{
				case EMeasureResultDisplayType::World:
					MeasuringCables[Index]->SetWorldLocation(PickedLocation);
					MeasuringCables[Index - 1 >= 0 ? Index - 1 : MeasuringCables.Num() - 1]->EndLocation =
						PickedLocation; // 世界更新相连线位置
					break;
				case EMeasureResultDisplayType::Screen:
					// UI 更新相连线位置
					break;
				default: ;
				}
			}
		}
		PickedSphere = nullptr; // 清除
	}
	else
	{
		if (GetHitResultUnderMouse(PickAndPlacePointByMouseHitResult))
		{
			PickedSphere = Cast<UCosmosMeasureToolSphereComponent>(PickAndPlacePointByMouseHitResult.GetComponent());
			if (!PickedSphere) // 没有点击到已存在的点
			{
				// 添加点
				PickedLocation = FVector(PickAndPlacePointByMouseHitResult.Location.X,
				                         PickAndPlacePointByMouseHitResult.Location.Y, MeasurePlaneZ);
				UCosmosMeasureToolSphereComponent* Point = NewObject<UCosmosMeasureToolSphereComponent>(this);
				Point->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
				Point->SetWorldLocation(PickedLocation);
				Point->RegisterComponent();
				Point->SetMaxScaleDistance(TraceDistance);
				if (Material)
				{
					Point->SetMaterial(0, Material);
				}
				/**
				 * 插入到数组
				 * 1.找到数组内最近且不相交其他线的 点A
				 * 2.找到与最近 点A 相邻的最近不相交其他相邻线的点B
				 */
				int32 PreIndex;
				int32 NextIndex;
				SaveNewPoint(Point, PickedLocation, PreIndex, NextIndex);
				// UE_LOG(LogTemp, Log, TEXT("PreIndex %d ; NextIndex %d"), PreIndex, NextIndex);
				switch (MeasureResultDisplayType)
				{
				case EMeasureResultDisplayType::World:
					if (PreIndex != -1 && NextIndex != -1)
					{
						// 更新相连线位置
						MeasuringCables[PreIndex]->EndLocation = PickedLocation;
						// 添加一条连线
						UCosmosMeasureToolCableComponent* NewCable = NewObject<UCosmosMeasureToolCableComponent>(this);
						NewCable->SetWorldLocation(PickedLocation); // 设置起始位
						NewCable->EndLocation = MeasuringLocation[NextIndex]; // 设置结束位置
						NewCable->SetMaxScaleDistance(TraceDistance); // 设置缩放生效范围
						if (Material)
						{
							NewCable->SetMaterial(0, Material);
						}
						NewCable->RegisterComponent();
						MeasuringCables.Insert(NewCable, PreIndex + 1); // 插入数组
					}
					break;
				case EMeasureResultDisplayType::Screen:
					// UI 更新相连线位置
					break;
				default: ;
				}
			}
		}
	}
}

void ACosmosAreaMeasureTool::SaveNewPoint(UCosmosMeasureToolSphereComponent* NewPoint, FVector NewLocation,
                                          int32& PreIndex, int32& NextIndex)
{
	NewSaveNewPointLocation = NewLocation;
	// 距离新增点最近的现有点下标
	int32 ClosestIndex = 0;
	// 距离新增点最近的现有点距离（2D）
	float MinDistance = (NewSaveNewPointLocation - MeasuringLocation[ClosestIndex]).Size2D();
	// 获得最近的现有点下标
	for (int i = 0; i < MeasuringLocation.Num(); i++)
	{
		// 不与任何已存在的连线相交 才作数
		if (!IsIntersectAnExistingLine(NewSaveNewPointLocation, i))
		{
			const float CurDistance = (NewSaveNewPointLocation - MeasuringLocation[i]).Size2D();
			if (MinDistance > CurDistance)
			{
				MinDistance = CurDistance;
				ClosestIndex = i;
			}
		}
	}

	const int32 ClosestConnectPointIndex = GetClosestConnectPointIndex(NewSaveNewPointLocation, ClosestIndex);
	// UE_LOG(LogTemp, Log, TEXT("ClosestIndex %d ; ClosestConnectPointIndex %d"),
	//        ClosestIndex, ClosestConnectPointIndex);

	if (ClosestConnectPointIndex != -1)
	{
		PreIndex = FMath::Min(ClosestIndex, ClosestConnectPointIndex);
		NextIndex = FMath::Max(ClosestIndex, ClosestConnectPointIndex);
		UE_LOG(LogTemp, Log, TEXT("Between %d %d"), PreIndex, NextIndex);
		// 特殊情况：一点为0 一点为last，即在Loop的位置
		if (PreIndex == 0 && NextIndex == MeasuringLocation.Num() - 1)
		{
			PreIndex = NextIndex;
			NextIndex = 0;
			// 插入数组
			MeasuringPoints.Insert(NewPoint, PreIndex + 1);
			MeasuringLocation.Insert(NewSaveNewPointLocation, PreIndex + 1);
		}
		else
		{
			// 插入数组
			MeasuringPoints.Insert(NewPoint, NextIndex);
			MeasuringLocation.Insert(NewSaveNewPointLocation, NextIndex);
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
	NewPointLocation = NewLocation;
	// 获得距 最近的现有点 相邻两点距离，考虑到Loop
	const int32 ClosestNextIndex = ClosestIndex + 1 <= MeasuringLocation.Num() - 1 ? ClosestIndex + 1 : 0;
	const int32 ClosestPreIndex = ClosestIndex - 1 >= 0 ? ClosestIndex - 1 : MeasuringLocation.Num() - 1;

	// 新增点与最近的现有点的下一点连线 是否与 现存线段存在相交
	const bool bNextIndexCross = IsIntersectAnExistingLine(NewPointLocation, ClosestNextIndex);
	// 新增点与最近的现有点的上一点连线 是否与 现存线段存在相交
	const bool bPreIndexCross = IsIntersectAnExistingLine(NewPointLocation, ClosestPreIndex);
	// UE_LOG(LogTemp, Log, TEXT("bNextIndexCrossPre %hs ; bPreIndexCrossNext %hs"),
	//        bNextIndexCross? "True" : "False", bPreIndexCross? "True" : "False");
	if (!bNextIndexCross && !bPreIndexCross) // 都不相交
	{
		const float NextDistance = (NewPointLocation - MeasuringLocation[ClosestNextIndex]).Size2D();
		const float PreDistance = (NewPointLocation - MeasuringLocation[ClosestPreIndex]).Size2D();
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

bool ACosmosAreaMeasureTool::IsIntersectAnExistingLine(FVector InNewPointLocation, int32 PointIndex)
{
	IsIntersectAnExistingLineNewPointLocation = InNewPointLocation;
	for (int i = 0; i < MeasuringLocation.Num(); i++)
	{
		const int NextIndex = i + 1 < MeasuringLocation.Num() ? i + 1 : 0;
		if (i != PointIndex && NextIndex != PointIndex)
		{
			if (UCosmosMeasureToolsBPLibrary::DoSegmentsIntersect(
				{FVector2D(IsIntersectAnExistingLineNewPointLocation), FVector2D(MeasuringLocation[PointIndex])},
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

void ACosmosAreaMeasureTool::StartMeasuring(bool bMeasureComplex, float Distance,
                                            TEnumAsByte<ECollisionChannel> ChannelToTrace)
{
	Super::StartMeasuring(bMeasureComplex, Distance, ChannelToTrace);

	AddDisplayUI(); // UI
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

void ACosmosAreaMeasureTool::ClearAll_Implementation()
{
	Super::ClearAll_Implementation();
	CanvasMesh->SetVisibility(false);
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
			// @todo: 尝试使用 Instanced
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
			MeasuringLocation.Emplace(Point->GetComponentLocation());

			switch (MeasureResultDisplayType)
			{
			case EMeasureResultDisplayType::World:
				CreateCable(); // 世界创建连线
				break;
			case EMeasureResultDisplayType::Screen:
				// UI 创建连线
				break;
			default: ;
			}
		}
		else
		{
			PickAndPlacePointByMouse();
		}
		GetMeasureResult();
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
			// 绘制面积填充
			FVector Origin, BoxExtent;
			UCosmosMeasureToolsBPLibrary::GetBoundOfPolygon(MeasuringLocation, Origin, BoxExtent);
			CanvasMesh->SetWorldLocation(Origin);
			const FVector Scale = BoxExtent * 0.01f * 2.0f;
			CanvasMesh->SetWorldScale3D(FVector(Scale.X, Scale.Y, 1.0f));
			CanvasMesh->SetVisibility(true);
			UKismetRenderingLibrary::ClearRenderTarget2D(this, CanvasRenderTarget, FLinearColor(0, 0, 0, 0));
			UCanvas* Canvas;
			FVector2D Size;
			FDrawToRenderTargetContext Context;
			UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this, CanvasRenderTarget,
			                                                       Canvas, Size, Context);
			TArray<FIntVector> Triangles;
			UCosmosMeasureToolsBPLibrary::PolygonSplitsTrianglesV2(MeasuringLocation, Triangles);
			TArray<FCanvasUVTri> CanvasUVTriangles;
			for (int i = 0; i < Triangles.Num(); i++)
			{
				const FIntVector& Triangle = Triangles[i];
				UE_LOG(LogTemp, Log, TEXT("Triangle %d %d %d"), Triangle.X, Triangle.Y, Triangle.Z)
				FCanvasUVTri CanvasUVTriangle;
				CanvasUVTriangle.V0_Pos = FVector2D(
					UCosmosMeasureToolsBPLibrary::VectorMapRangeClamped(MeasuringLocation[Triangle.X],
					                                                    Origin - BoxExtent, Origin + BoxExtent,
					                                                    FVector(0.0f), FVector(Size, 0)));
				CanvasUVTriangle.V1_Pos = FVector2D(
					UCosmosMeasureToolsBPLibrary::VectorMapRangeClamped(MeasuringLocation[Triangle.Y],
					                                                    Origin - BoxExtent, Origin + BoxExtent,
					                                                    FVector(0.0f), FVector(Size, 0)));
				CanvasUVTriangle.V2_Pos = FVector2D(
					UCosmosMeasureToolsBPLibrary::VectorMapRangeClamped(MeasuringLocation[Triangle.Z],
					                                                    Origin - BoxExtent, Origin + BoxExtent,
					                                                    FVector(0.0f), FVector(Size, 0)));
				const FLinearColor TriangleColor = FLinearColor::MakeRandomColor();
				// const FLinearColor TriangleColor = FLinearColor::White;
				CanvasUVTriangle.V0_Color = TriangleColor;
				CanvasUVTriangle.V1_Color = TriangleColor;
				CanvasUVTriangle.V2_Color = TriangleColor;
				CanvasUVTriangles.Emplace(CanvasUVTriangle);
			}
			Canvas->K2_DrawTriangle(nullptr, CanvasUVTriangles);
		}
	}
}

float ACosmosAreaMeasureTool::GetMeasuredArea()
{
	return MeasuredArea;
}
