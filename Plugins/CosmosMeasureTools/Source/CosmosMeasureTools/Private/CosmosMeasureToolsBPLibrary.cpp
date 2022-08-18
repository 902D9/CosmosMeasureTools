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

float UCosmosMeasureToolsBPLibrary::MeasurePolyArea(const TArray<FVector> Locations)
{
	// 只能计算凸多边形
	// FPoly NewPoly;
	// NewPoly.Init();
	// for (int i = 0; i < Locations.Num(); i++)
	// {
	// 	NewPoly.Vertices.Add(Locations[i]);
	// }
	// return NewPoly.Area();

	if (Locations.Num() < 3)
	{
		return 0;
	}
	double Area = 0.0f;
	for (int i = 0; i < Locations.Num(); ++i)
	{
		Area += Locations[i].X * Locations[(i + 1) % Locations.Num()].Y -
			Locations[i].Y * Locations[(i + 1) % Locations.Num()].X;
	}
	return FMath::Abs(Area) / 2;
}

bool UCosmosMeasureToolsBPLibrary::GetCenterOfVector(const TArray<FVector>& Points, FVector& Center)
{
	if (Points.Num() == 1)
	{
		Center = Points[0];
		return true;
	}
	if (Points.Num() >= 2)
	{
		float MinX = Points[0].X, MaxX = Points[0].X;
		float MinY = Points[0].Y, MaxY = Points[0].Y;
		float MinZ = Points[0].Z, MaxZ = Points[0].Z;
		for (const FVector& Point : Points)
		{
			MinX = Point.X < MinX ? Point.X : MinX;
			MaxX = Point.X > MaxX ? Point.X : MaxX;
			MinY = Point.Y < MinY ? Point.Y : MinY;
			MaxY = Point.Y > MaxY ? Point.Y : MaxY;
			MinZ = Point.Z < MinZ ? Point.Z : MinZ;
			MaxZ = Point.Z > MaxZ ? Point.Z : MaxZ;
		}
		Center = 0.5 * FVector(MaxX + MinX, MaxY + MinY, MaxZ + MinZ);
		return true;
	}
	return false;
}
