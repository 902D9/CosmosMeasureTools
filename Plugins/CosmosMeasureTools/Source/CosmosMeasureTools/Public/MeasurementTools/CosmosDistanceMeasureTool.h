// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CosmosMeasureTool.h"
#include "CosmosDistanceMeasureTool.generated.h"

UCLASS()
class COSMOSMEASURETOOLS_API ACosmosDistanceMeasureTool : public ACosmosMeasureTool
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACosmosDistanceMeasureTool();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
