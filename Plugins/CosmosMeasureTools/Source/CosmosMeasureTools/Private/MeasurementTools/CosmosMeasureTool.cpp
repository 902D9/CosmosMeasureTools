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
	if (Material)
	{
		PreviewSphere->SetMaterial(0, Material);
	}
}

// Called when the game starts or when spawned
void ACosmosMeasureTool::BeginPlay()
{
	Super::BeginPlay();
}

void ACosmosMeasureTool::PreviewLastPoint()
{
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (PlayerController)
	{
		FVector WorldLocation;
		FVector WorldDirection;
		// 射线检测 更新 预览位置
		if (PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
		{
			FHitResult HitResult;
			static const FName LineTraceSingleName(TEXT("LineTraceSingle"));
			// FCollisionQueryParams Params = ConfigureCollisionParams(LineTraceSingleName, bTraceComplex, ActorsToIgnore, bIgnoreSelf, WorldContextObject);
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(this); // 忽略自身
			Params.bTraceComplex = bTraceComplex;
			if (GetWorld()->LineTraceSingleByChannel(HitResult,
			                                         WorldLocation, WorldLocation + (WorldDirection * TraceDistance),
			                                         ECC_Visibility,
			                                         Params))
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
	}
}

void ACosmosMeasureTool::CreateCable()
{
	// 如果存在连线，则先保存
	if (PreviewCable)
	{
		MeasuringCables.Add(PreviewCable);
	}
	PreviewCable = NewObject<UCosmosMeasureToolCableComponent>(this);
	PreviewCable->SetRelativeTransform(PreviewPointRelativeTransform); // 设置起始位置
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
		PreviewLastPoint();
	}
}

void ACosmosMeasureTool::StartMeasuring(bool bMeasureComplex, float Distance)
{
	bMeasuring = true;
	bTraceComplex = bMeasureComplex;
	TraceDistance = Distance;
	// UE_LOG(LogTemp, Log, TEXT("Trace Distance %f"), TraceDistance);
	PreviewSphere->SetVisibility(bMeasuring);
	SetActorTickEnabled(bMeasuring);

	// 更新已存在Cable缩放距离
	for (auto& MeasuringCable : MeasuringCables)
	{
		MeasuringCable->SetMaxScaleDistance(TraceDistance);
	}

	PreviewSphere->SetMaxScaleDistance(TraceDistance);
	for (auto& MeasuringPoint : MeasuringPoints)
	{
		MeasuringPoint->SetMaxScaleDistance(TraceDistance);
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

FVector ACosmosMeasureTool::GetMeasuringLocationAtIndex(float Index)
{
	if (MeasuringLocation.IsValidIndex(1) && Index != MeasuringLocation.Num() - 1)
	{
		const FVector PointOne = MeasuringLocation[Index];
		const FVector PointTwo = MeasuringLocation[Index + 1];
		return FVector(
			(PointOne.X + PointTwo.X)/2,
			(PointOne.Y + PointTwo.Y)/2,
			(PointOne.Z + PointTwo.Z)/2
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
	for (const auto& Point : MeasuringPoints)
	{
		Point->DestroyComponent();
	}
	MeasuringPoints.Empty();
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
		MeasuringPoints.Add(Point); // 数组保存
		MeasuringLocation.Add(Point->K2_GetComponentLocation());
		CreateCable();
	}
}
