// Fill out your copyright notice in the Description page of Project Settings.


#include "MeasurementTools/CosmosMeasureTool.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetArrayLibrary.h"
#include "MeasurementTools/CosmosMeasureToolCableComponent.h"
#include "MeasurementTools/CosmosMeasureToolSphereComponent.h"

// Sets default values
ACosmosMeasureTool::ACosmosMeasureTool(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	PreviewSphere = CreateDefaultSubobject<UCosmosMeasureToolSphereComponent>(TEXT("PreviewSphere"));
	PreviewSphere->SetupAttachment(RootComponent);
	PreviewSphere->SetVisibility(false);
	PreviewSphere->SetRelativeScale3D(FVector(0.1f));
	PreviewSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (Material)
	{
		PreviewSphere->SetMaterial(0, Material);
	}

	TraceChannel = ECC_Visibility;
}

// Called when the game starts or when spawned
void ACosmosMeasureTool::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
}

void ACosmosMeasureTool::ApplyWorldOffset(const FVector& InOffset, bool bWorldShift)
{
	Super::ApplyWorldOffset(InOffset, bWorldShift);

	// UE_LOG(LogTemp, Log, TEXT("ApplyWorldOffset %f  %f  %f %hhd"), InOffset.X, InOffset.Y, InOffset.Z, bWorldShift);
	// if (!bWorldShift && !InOffset.IsZero())
	// {
	for (int i = 0; i < MeasuringLocation.Num(); ++i)
	{
		// const FVector NewStartLocation = GetMeasuringLocationAtIndex(i) + InOffset;
		const FVector NewStartLocation = MeasuringPoints[i]->GetComponentLocation();
		// const FVector NewStartLocation = MeasuringLocation[i] + InOffset;
		MeasuringLocation[i] = NewStartLocation;
		// UE_LOG(LogTemp, Log, TEXT("NewStartLocation %f  %f  %f"), NewStartLocation.X, NewStartLocation.Y,
		//        NewStartLocation.Z);
		if (MeasuringCables.IsValidIndex(i))
		{
			UCosmosMeasureToolCableComponent* Cable = MeasuringCables[i];
			if (Cable)
			{
				Cable->SetWorldLocation(NewStartLocation); // 设置起始位置
			}
		}
		// FVector EndLocation = Cable->EndLocation;
		// Cable->EndLocation = EndLocation + InOffset; // 设置结束位置
	}
	if (PreviewCable)
	{
		PreviewCable->SetWorldLocation(PreviewCable->GetComponentLocation() + InOffset);
	}
	// }
}

bool ACosmosMeasureTool::GetHitResultUnderMouse(FHitResult& HitResult)
{
	if (PlayerController)
	{
		FVector WorldLocation;
		FVector WorldDirection;
		// 射线检测 更新 预览位置
		if (PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
		{
			static const FName LineTraceSingleName(TEXT("LineTraceSingle"));
			// FCollisionQueryParams Params = ConfigureCollisionParams(LineTraceSingleName, bTraceComplex, ActorsToIgnore, bIgnoreSelf, WorldContextObject);
			FCollisionQueryParams Params;
			// Params.AddIgnoredActor(this); // 忽略自身
			Params.bTraceComplex = bTraceComplex;
			return (GetWorld()->LineTraceSingleByChannel(HitResult,
			                                             WorldLocation,
			                                             WorldLocation + (WorldDirection * TraceDistance),
			                                             TraceChannel,
			                                             Params));
		}
		return false;
	}
	return false;
}

void ACosmosMeasureTool::PreviewLastPointAndCable()
{
	FHitResult HitResult;
	if (GetHitResultUnderMouse(HitResult))
	{
		PreviewPointLocation = HitResult.Location;
		PreviewPointRelativeLocation = GetActorTransform().InverseTransformPosition(PreviewPointLocation),
			PreviewSphere->SetWorldLocation(PreviewPointLocation); // 球体位置
		if (PreviewCable)
		{
			PreviewCable->EndLocation = PreviewPointRelativeLocation;
		}
	}
}

void ACosmosMeasureTool::CreateCable()
{
	// 如果存在连线，则先保存
	if (PreviewCable)
	{
		MeasuringCables.Emplace(PreviewCable);
	}
	PreviewCable = NewObject<UCosmosMeasureToolCableComponent>(this);
	PreviewCable->SetWorldLocation(PreviewPointLocation); // 设置起始位置
	PreviewCable->SetMaxScaleDistance(TraceDistance); // 设置缩放生效范围
	if (Material)
	{
		PreviewCable->SetMaterial(0, Material);
	}
	PreviewCable->RegisterComponent();
}

// Called every frame
void ACosmosMeasureTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bMeasuring)
	{
		PreviewLastPointAndCable();
	}
}

void ACosmosMeasureTool::StartMeasuring(bool bMeasureComplex, float Distance,
                                        TEnumAsByte<ECollisionChannel> ChannelToTrace)
{
	bMeasuring = true;
	bTraceComplex = bMeasureComplex;
	TraceDistance = Distance;
	TraceChannel = ChannelToTrace;
	// UE_LOG(LogTemp, Log, TEXT("Trace Distance %f"), TraceDistance);
	PreviewSphere->SetVisibility(bMeasuring);
	SetActorTickEnabled(bMeasuring);

	bIsFirstPointAfterStartMeasuring = true; // 标记这是开始测量后的第一次点击

	// 更新已存在Cable缩放距离
	for (auto& MeasuringCable : MeasuringCables)
	{
		if (MeasuringCable)
		{
			MeasuringCable->SetMaxScaleDistance(TraceDistance);
		}
	}

	PreviewSphere->SetMaxScaleDistance(TraceDistance);
	for (auto& MeasuringPoint : MeasuringPoints)
	{
		if (MeasuringPoint)
		{
			MeasuringPoint->SetMaxScaleDistance(TraceDistance);
		}
	}
}

void ACosmosMeasureTool::StopMeasuring()
{
	bMeasuring = false;
	PreviewSphere->SetVisibility(bMeasuring);
	SetActorTickEnabled(bMeasuring);
	if (PreviewCable)
	{
		PreviewCable->DestroyComponent();
	}
}

void ACosmosMeasureTool::GetMeasureResult()
{
	// do in subject 
}

FVector ACosmosMeasureTool::GetMeasuringLocationAtIndex(float Index)
{
	if (MeasuringLocation.IsValidIndex(1) && Index != MeasuringLocation.Num() - 1)
	{
		const FVector PointOne = MeasuringLocation[Index];
		const FVector PointTwo = MeasuringLocation[Index + 1];
		return FVector(
			(PointOne.X + PointTwo.X) / 2,
			(PointOne.Y + PointTwo.Y) / 2,
			(PointOne.Z + PointTwo.Z) / 2
		);
	}
	return FVector::ZeroVector;
}

FVector ACosmosMeasureTool::GetLastMeasuringLocation()
{
	return GetMeasuringLocationAtIndex(MeasuringLocation.Num() - 2);
}

void ACosmosMeasureTool::ClearAll_Implementation()
{
	StopMeasuring();
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

void ACosmosMeasureTool::AddMeasuringPoint_Implementation()
{
	if (bMeasuring)
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

		GetMeasureResult();

		AddDisplayUI();

		// 标记是否是第一个点,最后执行
		if (bIsFirstPointAfterStartMeasuring)
		{
			bIsFirstPointAfterStartMeasuring = false;
		}
	}
}

void ACosmosMeasureTool::AddDisplayUI_Implementation()
{
	// do in blueprint
}
