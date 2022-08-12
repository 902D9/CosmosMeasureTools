// Fill out your copyright notice in the Description page of Project Settings.


#include "MeasurementTools/CosmosMeasureTool.h"
#include "Kismet/GameplayStatics.h"
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
				if(PreviewCable)
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
	PreviewCable->SetRelativeTransform(PreviewPointRelativeTransform);
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
	PreviewSphere->SetVisibility(bMeasuring);
	SetActorTickEnabled(bMeasuring);
}

void ACosmosMeasureTool::StopMeasuring()
{
	bMeasuring = false;
	PreviewSphere->SetVisibility(bMeasuring);
	SetActorTickEnabled(bMeasuring);
	if(PreviewCable)
	{
		PreviewCable->DestroyComponent();
	}
}

void ACosmosMeasureTool::ClearAll()
{
	StopMeasuring();
	for (const auto& Point : MeasuringPoints)
	{
		Point->DestroyComponent();
	}
	MeasuringPoints.Empty();
}

void ACosmosMeasureTool::AddMeasuringPoint()
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
		MeasuringPoints.Add(Point); // 数组保存
		CreateCable();
	}
}
