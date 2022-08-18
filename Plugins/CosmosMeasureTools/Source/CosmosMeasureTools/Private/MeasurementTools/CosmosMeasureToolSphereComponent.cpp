// Fill out your copyright notice in the Description page of Project Settings.


#include "MeasurementTools/CosmosMeasureToolSphereComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UCosmosMeasureToolSphereComponent::UCosmosMeasureToolSphereComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	bEnableScaleByDistance = true;
	MinScaleDistance = 1000.0f;
	MaxScaleDistance = 3000.0f;
	BaseScale = 0.1f;

	CastShadow = false;
	MeshAsset = LoadObject<UStaticMesh>(nullptr,TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (MeshAsset)
	{
		UStaticMeshComponent::SetStaticMesh(MeshAsset);
	}
}

void UCosmosMeasureToolSphereComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
}


void UCosmosMeasureToolSphereComponent::ScaleByDistanceToCamera()
{
	// UE_LOG(LogTemp, Log, TEXT("ScaleByDistanceToCamera()"));
	if (bEnableScaleByDistance)
	{
		FVector ViewLocation;
		FRotator ViewRotation; // Not Used, but required for Function call
		/* Get the player cameras location. */
		PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);
		const float DistanceToCamera = FVector(this->GetComponentLocation() - ViewLocation).Size();
		const float Scale = UKismetMathLibrary::MapRangeClamped(
			DistanceToCamera, MinScaleDistance,
			MaxScaleDistance, BaseScale, BaseScale * MaxScaleDistance / MinScaleDistance);
		SetRelativeScale3D(FVector(Scale));
		// UE_LOG(LogTemp, Log, TEXT("Sphere Scale %f"), Scale);
		SendRenderDynamicData_Concurrent();
	}
}

void UCosmosMeasureToolSphereComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ScaleByDistanceToCamera();
}

void UCosmosMeasureToolSphereComponent::SetMaxScaleDistance(float NewMaxScaleDistance)
{
	if (NewMaxScaleDistance < MinScaleDistance)
	{
		MaxScaleDistance = MinScaleDistance + 1.0f;
	}
	else
	{
		MaxScaleDistance = NewMaxScaleDistance;
	}
}
