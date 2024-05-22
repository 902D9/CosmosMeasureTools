// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MeasureResultDisplayWidget.generated.h"

enum class EMeasureType : uint8;
class ACosmosMeasureTool;
/**
 * 
 */
UCLASS()
class COSMOSMEASURETOOLS_API UMeasureResultDisplayWidget : public UUserWidget
{
	GENERATED_BODY()

	
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools", meta=(ExposeOnSpawn))
	ACosmosMeasureTool* OwningMeasureTool;
	UPROPERTY(BlueprintReadOnly, Category = "Cosmos Measurement Tools", meta=(ExposeOnSpawn))
	EMeasureType MeasureType;
};
