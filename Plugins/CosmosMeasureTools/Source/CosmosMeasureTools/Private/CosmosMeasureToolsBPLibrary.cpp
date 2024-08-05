// Copyright Epic Games, Inc. All Rights Reserved.

#include "CosmosMeasureToolsBPLibrary.h"
#include "CosmosMeasureTools.h"
#include "Polygon2.h"

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

bool UCosmosMeasureToolsBPLibrary::GetBoundOfPolygon(const TArray<FVector>& Points, FVector& Origin, FVector& BoxExtent)
{
	if (Points.Num() == 1)
	{
		Origin = Points[0];
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
		Origin = 0.5 * FVector(MaxX + MinX, MaxY + MinY, MaxZ + MinZ);
		BoxExtent = 0.5 * FVector(MaxX - MinX, MaxY - MinY, MaxZ - MinZ);
		return true;
	}
	return false;
}

bool UCosmosMeasureToolsBPLibrary::IsPointInPolygon2D(const FVector2D& TestPoint,
                                                      const TArray<FVector2D>& PolygonPoints)
{
	const int NumPoints = PolygonPoints.Num();
	float AngleSum = 0.0f;
	for (int PointIndex = 0; PointIndex < NumPoints; ++PointIndex)
	{
		const FVector2D& VecAB = PolygonPoints[PointIndex] - TestPoint;
		const FVector2D& VecAC = PolygonPoints[(PointIndex + 1) % NumPoints] - TestPoint;
		const float Angle = FMath::Sign(FVector2D::CrossProduct(VecAB, VecAC)) * FMath::Acos(
			FMath::Clamp(FVector2D::DotProduct(VecAB, VecAC) / (VecAB.Size() * VecAC.Size()), -1.0f, 1.0f));
		AngleSum += Angle;
	}
	return (FMath::Abs(AngleSum) > 0.001f);
}

bool UCosmosMeasureToolsBPLibrary::IsPointInPolygon(const FVector& TestPoint, const TArray<FVector>& PolygonPoints)
{
	const int NumPoints = PolygonPoints.Num();
	TArray<FVector2D> PolygonPoints2D;
	for (int PointIndex = 0; PointIndex < NumPoints; ++PointIndex)
	{
		PolygonPoints2D.Emplace(FVector2D(PolygonPoints[PointIndex]));
	}
	return IsPointInPolygon2D(FVector2D(TestPoint), PolygonPoints2D);
}

FVector UCosmosMeasureToolsBPLibrary::VectorMapRangeClamped(const FVector Value, const FVector MinRange,
                                                            const FVector MaxRange,
                                                            const FVector MinOutput, const FVector MaxOutput)
{
	const float X = FMath::GetMappedRangeValueClamped(
		FVector2D(MinRange.X, MaxRange.X), FVector2D(MinOutput.X, MaxOutput.X), Value.X);
	const float Y = FMath::GetMappedRangeValueClamped(
		FVector2D(MinRange.Y, MaxRange.Y), FVector2D(MinOutput.Y, MaxOutput.Y), Value.Y);
	const float Z = FMath::GetMappedRangeValueClamped(
		FVector2D(MinRange.Z, MaxRange.Z), FVector2D(MinOutput.Z, MaxOutput.Z), Value.Z);
	return FVector(X, Y, Z);
}

void UCosmosMeasureToolsBPLibrary::PolygonSplitsTriangles(const TArray<FVector>& InVertices,
                                                          TArray<FIntVector>& Triangles)
{
	Triangles.Empty();

	const int VerticesNum = InVertices.Num();
	if (VerticesNum >= 3) //至少存在一个三角形
	{
		// @todo: 5.1 FGeometryScriptVectorList
		TPolygon2<float> Polygon;
		for (const auto Point : InVertices)
		{
			FVector2D TmpPoint = FVector2D(Point.X, Point.Y);
			FVector2D LastPointOfPolygon = Polygon.VertexCount() == 0
				                               ? FVector2D(99999, 99999)
				                               : FVector2D(Polygon[Polygon.VertexCount() - 1]);
			if (FVector2D::Distance(LastPointOfPolygon, TmpPoint) > 1.0f) // 忽略与上一个点距离过近的点
			{
				Polygon.AppendVertex(TmpPoint);
			}
		}
		int FailedTimes = 0;
		while (Polygon.VertexCount() > 3)
		{
			for (int i = 0; i < Polygon.VertexCount(); i++)
			{
				const int Previous = (i + Polygon.VertexCount() - 1) % Polygon.VertexCount();
				const int Next = (i + 1) % Polygon.VertexCount();
				TSegment2<float> Line(FVector2D(Polygon[Previous].X, Polygon[Previous].Y),
				                      FVector2D(Polygon[Next].X, Polygon[Next].Y));
				Line = TSegment2<float>(Line.Center, Line.Direction, Line.Extent * 0.95); //短一点
				if (Polygon.Contains(Line) || FailedTimes > VerticesNum * 5)
				{
					FIntVector T;
					T = FIntVector(Previous, i, Next);
					Triangles.Add(T);
					Polygon.RemoveVertex(i);
					break;
				}
				++FailedTimes;
			}
		}

		const FIntVector T = FIntVector(0, 1, 2);
		Triangles.Add(T);
	}
}
