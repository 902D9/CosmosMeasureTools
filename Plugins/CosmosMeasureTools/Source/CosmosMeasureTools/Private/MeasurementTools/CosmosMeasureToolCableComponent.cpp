// Fill out your copyright notice in the Description page of Project Settings.


#include "MeasurementTools/CosmosMeasureToolCableComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UCosmosMeasureToolCableComponent::UCosmosMeasureToolCableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	bEnableScaleByDistance = true;
	MinScaleDistance = 1000.0f;
	MaxScaleDistance = 3000.0f;
	BaseCableScale = 40.0f;

	EndLocation = FVector::ZeroVector;
	CableLength = 1.0f;
	NumSegments = 1;
	CableWidth = BaseCableScale;
	SetGenerateOverlapEvents(false);
	UPrimitiveComponent::SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UCosmosMeasureToolCableComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
}

void UCosmosMeasureToolCableComponent::ScaleCableByDistanceToCamera()
{
	if (bEnableScaleByDistance)
	{
		FVector ViewLocation;
		FRotator ViewRotation; // Not Used, but required for Function call
		/* Get the player cameras location. */
		PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);
		const float DistanceToCamera = FVector(this->GetComponentLocation() - ViewLocation).Size();
		const float Scale = UKismetMathLibrary::MapRangeClamped(
			DistanceToCamera, MinScaleDistance,
			MaxScaleDistance, BaseCableScale, MaxScaleDistance / MinScaleDistance * BaseCableScale);
		// SetRelativeScale3D(WidgetScale * FVector::OneVector);
		CableWidth = Scale;
		// UE_LOG(LogTemp, Log, TEXT("Cable Width %f"), CableWidth);
		SendRenderDynamicData_Concurrent();
	}
}

void UCosmosMeasureToolCableComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                     FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ScaleCableByDistanceToCamera();
}

void UCosmosMeasureToolCableComponent::SetMaxScaleDistance(float NewMaxScaleDistance)
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
