// Copyright Epic Games, Inc. All Rights Reserved.

#include "CosmosMeasureToolsBPLibrary.h"
#include "CosmosMeasureTools.h"
#include "Engine/Polys.h"

UCosmosMeasureToolsBPLibrary::UCosmosMeasureToolsBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UCosmosMeasureToolsBPLibrary::CheckWhetherTwoLineSegmentsIntersect2D(TArray<FVector2D> A, TArray<FVector2D> B)
{
	// 参数验证 - 长度
	if (A.Num() < 2 || B.Num() < 2)
	{
		return false; // 长度不足
	}

	// 快速排斥实验
	if (FMath::Max(B[0].X, B[1].X) < FMath::Min(A[0].X, A[1].X) ||
		FMath::Max(B[0].Y, B[1].Y) < FMath::Min(A[0].Y, A[1].Y) ||
		FMath::Max(A[0].X, A[1].X) < FMath::Min(B[0].X, B[1].X) ||
		FMath::Max(A[0].Y, A[1].Y) < FMath::Min(B[0].Y, B[1].Y)
	)
	{
		return false;
	}

	// 跨立实验
	// if (((A[0].X - B[0].X) * (B[1].Y - B[0].Y) - (A[0].Y - B[0].Y) * (B[1].X - B[0].X)) *
	// 	((A[1].X - B[0].X) * (B[1].Y - B[0].Y) - (A[1].Y - B[0].Y) * (B[1].Y - B[0].Y)) > 0 ||
	// 	((B[0].X - A[0].X) * (A[1].Y - A[0].Y) - (B[0].Y - A[0].Y) * (A[1].X - A[0].X)) *
	// 	((B[1].X - A[0].X) * (A[1].Y - A[0].Y) - (B[1].Y - A[0].Y) * (A[1].X - A[0].X)) > 0)
	// {
	// 	return false;
	// }
	if (FVector2D::CrossProduct(A[0] - B[1], B[0] - B[1]) * FVector2D::CrossProduct(A[1] - B[1], B[0] - B[1]) > 0 ||
		FVector2D::CrossProduct(B[0] - A[1], A[0] - A[1]) * FVector2D::CrossProduct(B[1] - A[1], A[0] - A[1]) > 0)
	{
		return false;
	}

	return true;
}

float UCosmosMeasureToolsBPLibrary::MeasurePolyArea(const TArray<FVector>& Locations)
{
	FPoly NewPoly;
	NewPoly.Init();
	for (int i = 0; i < Locations.Num(); i++)
	{
		NewPoly.Vertices.Add(Locations[i]);
	}
	return NewPoly.Area();
}
