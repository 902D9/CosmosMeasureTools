// Fill out your copyright notice in the Description page of Project Settings.


#include "MeasurementTools/CosmosMeasureToolSphereComponent.h"

UCosmosMeasureToolSphereComponent::UCosmosMeasureToolSphereComponent()
{
	MeshAsset = LoadObject<UStaticMesh>(nullptr,TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (MeshAsset)
	{
		UStaticMeshComponent::SetStaticMesh(MeshAsset);
	}
}
