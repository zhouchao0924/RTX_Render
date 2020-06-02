
#pragma once

#include "kMathBase.h"
#include "kBox.h"
#include "kVector3D.h"
#include "kLine.h"
#include "kPlane.h"

template <class T>
class Triangle3DTemp
{
public:

	//! Determinates if the triangle is totally inside a bounding box.
	//! \param box: Box to check.
	//! \return Returns true if the triangle is withing the box,
	//! and false if it is not.
	bool IsTotalInsideBox(const TBoxTemp<float>& box) const
	{
		return (box.IsPointInside(pointA) &&
				box.IsPointInside(pointB) &&
				box.IsPointInside(pointC));
	}

	bool operator==(const Triangle3DTemp<T>& other) const { return other.pointA == pointA && other.pointB == pointB && other.pointC == pointC; }
	bool operator!=(const Triangle3DTemp<T>& other) const { return other.pointA != pointA || other.pointB != pointB || other.pointC != pointC; }

	//! Returns the closest point on a triangle to a point on the same plane.
	//! \param p: Point which must be on the same plane as the triangle.
	
	
	bool ClosestPointOnTriangle(const kVector3D& p) const
	{
		kVector3D rab = TLineTemp<float>(pointA, pointB).GetClosestPoint(p);
		kVector3D rbc = TLineTemp<float>(pointB, pointC).GetClosestPoint(p);
		kVector3D rca = TLineTemp<float>(pointC, pointA).GetClosestPoint(p);

		T d1 = (T)rab.GetDistanceFrom(p);
		T d2 = (T)rbc.GetDistanceFrom(p);
		T d3 = (T)rca.GetDistanceFrom(p);

		if (d1 < d2)
			return d1 < d3 ? rab : rca;

		return d2 < d3 ? rbc : rca;
	}

	//Calculate the closed distance from this triangle to the given point
	//if p is in this triangle,return 0.0;
	//! \param p: Point which must be on the same plane as the triangle.
	double DistanceToPoint(const TVector3DTemp<T> &p) const
	{
		if (IsPointInside(p))
			return 0.0;

		TVector3DTemp<T> rab = TLineTemp<T>(pointA, pointB).GetClosestPoint(p);
		TVector3DTemp<T> rbc = TLineTemp<T>(pointB, pointC).GetClosestPoint(p);
		TVector3DTemp<T> rca = TLineTemp<T>(pointC, pointA).GetClosestPoint(p);

		double d1 = rab.GetDistanceFrom(p);
		double d2 = rbc.GetDistanceFrom(p);
		double d3 = rca.GetDistanceFrom(p);

		if (d1>d2)
			d1 = d2;
		if (d1 > d3)
			d1 = d3;
		return d1;
	}

	//! Returns if a point is inside the triangle
	//! \param p: Point to test. Assumes that this point is already on the plane
	//! of the triangle.
	//! \return Returns true if the point is inside the triangle, otherwise false.
	bool IsPointInside(const TVector3DTemp<T>& p) const
	{
		return (IsOnSameSide(p, pointA, pointB, pointC) &&
				IsOnSameSide(p, pointB, pointA, pointC) &&
				IsOnSameSide(p, pointC, pointA, pointB));
	}

	//! Returns if a point is inside the triangle. This method is an implementation
	//! of the example used in a paper by Kasper Fauerby original written
	//! by Keidy from Mr-Gamemaker.
	//! \param p: Point to test. Assumes that this point is already on the plane
	//! of the triangle.
	//! \return Returns true if the point is inside the triangle, otherwise false.
	bool IsPointInsideFast(const TVector3DTemp<T>& p) const
	{
		TVector3DTemp<T> f = pointB - pointA;
		TVector3DTemp<T> g = pointC - pointA;

		float a = f.Dot(f);
		float b = f.Dot(g);
		float c = g.Dot(g);

		float ac_bb = (a*c) - (b*b);
		TVector3DTemp<T> vp = p - pointA;

		float d = vp.Dot(f);
		float e = vp.Dot(g);
		float x = (d*c) - (e*b);
		float y = (e*a) - (d*b);
		float z = x + y - ac_bb;

		return ((((DWORD&)z) & ~(((DWORD&)x) | ((DWORD&)y))) & 0x80000000) != 0;
	}


	bool IsOnSameSide(const TVector3DTemp<T>& p1, const TVector3DTemp<T>& p2, const TVector3DTemp<T>& a, const TVector3DTemp<T>& b) const
	{
		TVector3DTemp<T> bminusa = b - a;
		TVector3DTemp<T> cp1 = bminusa.CrossProduct(p1 - a);
		TVector3DTemp<T> cp2 = bminusa.CrossProduct(p2 - a);
		return (cp1.Dot(cp2) >= 0.0f);
	}


	//! Returns an intersection with a 3d line.
	//! \param lineVect: Vector of the line to intersect with.
	//! \param linePoint: Point of the line to intersect with.
	//! \param outIntersection: Place to store the intersection point, if there is one.
	//! \return Returns true if there was an intersection, false if there was not.
	bool GetIntersectionWithLimitedLine(const TLineTemp<T>& line,
		TVector3DTemp<T>& outIntersection) const
	{
		return GetIntersectionWithLine(line.start,
			line.Vector(), outIntersection) &&
			outIntersection.IsBetweenPoints(line.start, line.end);
	}


	//! Returns an intersection with a 3d line.
	//! Please note that also points are returned as intersection, which
	//! are on the line, but not between the start and end point of the line.
	//! If you want the returned point be between start and end, please
	//! use getIntersectionWithLimitedLine().
	//! \param lineVect: Vector of the line to intersect with.
	//! \param linePoint: Point of the line to intersect with.
	//! \param outIntersection: Place to store the intersection point, if there is one.
	//! \return Returns true if there was an intersection, false if there was not.
	bool GetIntersectionWithLine(const TVector3DTemp<T>& linePoint, const TVector3DTemp<T>& lineVect, TVector3DTemp<T>& outIntersection) const
	{
		if (GetIntersectionOfPlaneWithLine(linePoint, lineVect, outIntersection))
			return IsPointInside(outIntersection);

		return false;
	}

	//meaning of "safe":if the intersection is very close to the triangle,though a littile outer,we think it's in the triangle
	//by now,the safe range is 0.001
	bool GetSafeIntersectionWithLine(const TVector3DTemp<T>& linePoint, const TVector3DTemp<T>& lineVect, TVector3DTemp<T>& outIntersection) const
	{
		if (GetIntersectionOfPlaneWithLine(linePoint, lineVect, outIntersection))
		{
			if (DistanceToPoint(outIntersection) <= 0.001)
				return true;
			return false;
		}
		return false;
	}

	//meaning of "safe":if the intersection is very close to the triangle,though a littile outer,we think it's in the triangle
	//by now,the safe range is 0.001
	bool GetSafeIntersectionWithLimitedLine(const TLineTemp<T>& line, TVector3DTemp<T>& outIntersection) const
	{
		return GetSafeIntersectionWithLine(line.start, line.GetVector(), outIntersection) && outIntersection.IsBetweenPoints(line.start, line.end);
	}



	//! Calculates the intersection between a 3d line and 
	//! the plane the triangle is on.
	//! \param lineVect: Vector of the line to intersect with.
	//! \param linePoint: Point of the line to intersect with.
	//! \param outIntersection: Place to store the intersection point, if there is one.
	//! \return Returns true if there was an intersection, false if there was not.
	bool GetIntersectionOfPlaneWithLine(const TVector3DTemp<T>& linePoint, const TVector3DTemp<T>& lineVect, TVector3DTemp<T>& outIntersection) const
	{
		TVector3DTemp<T> normal = GetNormal();
		T t2 = normal.Dot(lineVect);

		if (t2 == 0.0f)
			return false;

		T d = pointA.Dot(normal);
		T t = -(normal.Dot(linePoint) - d) / t2;
		outIntersection = linePoint + (lineVect * t);
		
		return true;
	}

	//check whether the aabb of this triangle intersects with the given aabb
	bool IntersectsWithAABBbyAABB(const TBoxTemp<T>& box)
	{
		TBoxTemp<T> boxMe;
		boxMe.Reset(pointA);
		boxMe.Add(pointB);
		boxMe.Add(pointC);
		return boxMe.IntersectsWithBox(box);
	}

	bool IntersectsWithAABB(const TBoxTemp<T>& box) const
	{
		if (box.IsPointInside(pointA) || box.IsPointInside(pointB) || box.IsPointInside(pointC))
			return true;

		TBoxTemp<T> boxTriangle;
		boxTriangle.reset(pointA);
		boxTriangle.Add(pointB);
		boxTriangle.Add(pointC);

		if (!box.IntersectsWithBox(boxTriangle))
			return false;

		TVector3DTemp<T> v;
		if (true)//second pass
		{
			TLineTemp<T> lines[3];
			lines[0].Set(pointA, pointB);
			lines[1].Set(pointB, pointC);
			lines[2].Set(pointC, pointA);

			for (int i = 0; i < 3; i++)
			{
				if (box.IntersectsWithLine(lines[i]))
					return true;
			}
		}

		TLineTemp<T> lines[4];
		box.GetDiagonals(lines);
		SHORT i;
		for (i = 0; i < 4; i++)
		{
			if (GetSafeIntersectionWithLimitedLine(lines[i], v))
				return true;
		}
		return false;
	}


	//! Returns the normal of the triangle.
	//! Please note: The normal is not normalized.
	TVector3DTemp<T> GetNormal() const
	{
		return (pointB - pointA).CrossProduct(pointC - pointA);
	}

	//! Returns if the triangle is front of backfacing.
	//! \param lookDirection: Look direction.
	//! \return Returns true if the plane is front facing, which mean it would
	//! be visible, and false if it is backfacing.
	bool IsFrontFacing(const TVector3DTemp<T>& lookDirection) const
	{
		TVector3DTemp<T> n = GetNormal();
		n.Normalize();
		return n.Dot(lookDirection) <= 0.0f;
	}

	bool IsZeroArea()const
	{
		TVector3DTemp<T> d1, d2;
		d1 = pointA - pointB;
		d2 = pointC - pointB;
		d1.Normalize();
		d2.Normalize();

		if (fabs(d1.Dot(d2)) >= (1.0 - 0.0001f))
			return true;

		return false;
	}

	//! Returns the plane of this triangle.
	TPlaneTemp<T> GetPlane() const
	{
		return TPlaneTemp<T>(pointA, pointB, pointC);
	}

	void Set(const TVector3DTemp<T>& a, const TVector3DTemp<T>& b, const TVector3DTemp<T>& c)
	{
		pointA = a;
		pointB = b;
		pointC = c;
	}

	//���׹�ʽ
	float GetArea()
	{
		double a, b, c, p;
		a = (pointB - pointA).Size();
		b = (pointB - pointC).Size();
		c = (pointA - pointC).Size();
		p = (a + b + c) / 2.0f;
		return (float)sqrt(p*(p - a)*(p - b)*(p - c));
	}

	//! the three points of the triangle
	TVector3DTemp<T> pointA;
	TVector3DTemp<T> pointB;
	TVector3DTemp<T> pointC;
};

typedef Triangle3DTemp<float>  kTriangle;
typedef Triangle3DTemp<double> DTriangle;
typedef Triangle3DTemp<short>  STriangle;



