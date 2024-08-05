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

/**
 * 将多边形分割成三角形
 * 
 * @param InVertices 输入的多边形顶点数组
 * @param Triangles 输出的三角形顶点索引数组
 */
void UCosmosMeasureToolsBPLibrary::PolygonSplitsTriangles(const TArray<FVector>& InVertices,
                                                          TArray<FIntVector>& Triangles)
{
	// 清空输出数组
	Triangles.Empty();

	// 获取输入顶点的数量
	const int VerticesNum = InVertices.Num();
	// 如果顶点数量大于等于3，即至少能构成一个三角形
	if (VerticesNum >= 3)
	{
		// @todo:传入顶点未按照多边形的顺序，需要先对顶点进行排序。但是实际不应该有这一步，传入时必须排好序
		// @todo: 5.1 FGeometryScriptVectorList
		// 创建一个二维多边形对象
		TPolygon2<float> Polygon;
		// 遍历输入的三维顶点，将其转换为二维顶点并添加到多边形中
		for (const auto Point : InVertices)
		{
			FVector2D TmpPoint = FVector2D(Point.X, Point.Y);
			// 获取当前点与多边形最后一个点的距离，如果距离过近则忽略该点
			FVector2D LastPointOfPolygon = Polygon.VertexCount() == 0
				                               ? FVector2D(99999, 99999)
				                               : FVector2D(Polygon[Polygon.VertexCount() - 1]);
			if (FVector2D::Distance(LastPointOfPolygon, TmpPoint) > 1.0f)
			{
				Polygon.AppendVertex(TmpPoint);
			}
		}
		// 初始化失败次数
		int FailedTimes = 0;
		// 当多边形的顶点数量大于3时，尝试将其分割成三角形
		while (Polygon.VertexCount() > 3)
		{
			// 遍历多边形的每个顶点
			// for (int i = 0; i < Polygon.VertexCount(); i++)
			for (int i = Polygon.VertexCount() - 1; i >= 0; --i) // 逆序遍历多边形顶点，删除顶点时，循环索引不会受到影响。
			{
				// 获取当前顶点的前一个和后一个顶点的索引
				const int Previous = (i + Polygon.VertexCount() - 1) % Polygon.VertexCount();
				const int Next = (i + 1) % Polygon.VertexCount();
				// 创建当前顶点与前后顶点构成的线段
				TSegment2<float> Line(FVector2D(Polygon[Previous].X, Polygon[Previous].Y),
				                      FVector2D(Polygon[Next].X, Polygon[Next].Y));

				// **注释**: 为了确保线段完全位于多边形内部，减少因浮点数精度问题导致的误判，
				// 将线段缩短5%，以提高算法的鲁棒性。
				Line = TSegment2<float>(Line.Center, Line.Direction, Line.Extent * 0.95);

				// 如果线段完全在多边形内部，或者尝试次数过多，则将三个顶点索引添加到输出数组，并移除当前顶点
				if (Polygon.Contains(Line) || FailedTimes > VerticesNum * 5)
				{
					FIntVector T;
					T = FIntVector(Previous, i, Next);
					Triangles.Add(T);
					Polygon.RemoveVertex(i);
					break;
				}
				// 增加失败次数
				++FailedTimes;
			}
		}
		// 对于剩余的三个顶点，直接构成一个三角形并添加到输出数组
		const FIntVector T = FIntVector(0, 1, 2);
		Triangles.Add(T);
	}
}

void UCosmosMeasureToolsBPLibrary::PolygonSplitsTrianglesV2(const TArray<FVector>& InVertices,
                                                            TArray<FIntVector>& Triangles)
{
	// 清空输出数组
	Triangles.Empty();

	const int VerticesNum = InVertices.Num();
	if (VerticesNum >= 3)
	{
		TArray<FVector2D> Vertices2D;
		for (const auto& Point : InVertices)
		{
			Vertices2D.Add(FVector2D(Point.X, Point.Y));
		}

		// 分割多边形
		TArray<TArray<FVector2D>> ConvexPolygons;
		SplitPolygonIntoConvex(Vertices2D, ConvexPolygons);

		// 三角化每个凸多边形
		for (const auto& ConvexPolygon : ConvexPolygons)
		{
			TArray<FIntVector> TriangleIndices;
			TriangulateConvexPolygon(ConvexPolygon, TriangleIndices);
			Triangles.Append(TriangleIndices);
		}
	}
}

void UCosmosMeasureToolsBPLibrary::SplitPolygonIntoConvex(const TArray<FVector2D>& InVertices,
														  TArray<TArray<FVector2D>>& ConvexPolygons)
{
	// 识别凹点
	TArray<bool> IsConcave;
	for (int i = 0; i < InVertices.Num(); ++i)
	{
		FVector2D A = InVertices[(i + InVertices.Num() - 1) % InVertices.Num()];
		FVector2D B = InVertices[i];
		FVector2D C = InVertices[(i + 1) % InVertices.Num()];
		FVector2D AB = B - A;
		FVector2D BC = C - B;
		FVector2D NormalAB = FVector2D(-AB.Y, AB.X).GetSafeNormal();
		FVector2D NormalBC = FVector2D(-BC.Y, BC.X).GetSafeNormal();
		float Angle = FVector2D::DotProduct(NormalAB, NormalBC);
		IsConcave.Add(Angle < 0);
	}

	// 分割多边形
	TArray<FVector2D> CurrentPolygon = InVertices;
	while (CurrentPolygon.Num() > 3)
	{
		bool FoundConcave = false;
		for (int i = 0; i < CurrentPolygon.Num(); ++i)
		{
			if (IsConcave[i])
			{
				// 连接凹点与其相邻的凸点
				TArray<FVector2D> NewPolygon1, NewPolygon2;
				// ... 分割多边形的逻辑
				ConvexPolygons.Add(NewPolygon1);
				CurrentPolygon = NewPolygon2;
				FoundConcave = true;
				break;
			}
		}
		if (!FoundConcave)
		{
			// 如果没有找到凹点，说明已经是凸多边形
			ConvexPolygons.Add(CurrentPolygon);
			break;
		}
	}
}

void UCosmosMeasureToolsBPLibrary::TriangulateConvexPolygon(const TArray<FVector2D>& InVertices,
															TArray<FIntVector>& TriangleIndices)
{
	for (int i = 0; i < InVertices.Num() - 2; ++i)
	{
		TriangleIndices.Add(FIntVector(0, i + 1, i + 2));
	}
}

