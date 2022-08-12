// Fill out your copyright notice in the Description page of Project Settings.


#include "MeasurementTools/CosmosMeasureToolCableComponent.h"

UCosmosMeasureToolCableComponent::UCosmosMeasureToolCableComponent()
{
	EndLocation = FVector::ZeroVector;
	CableLength = 1.0f;
	NumSegments = 1;
	CableWidth = 20.0f;
	SetGenerateOverlapEvents(false);
	UPrimitiveComponent::SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
