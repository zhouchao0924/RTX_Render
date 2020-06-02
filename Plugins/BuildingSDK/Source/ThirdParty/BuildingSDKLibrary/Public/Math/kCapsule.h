

#pragma once

#include "kMathBase.h"
#include "kLine.h"
#include "kPlane.h"

template<class T>
class TCapsuleTemp
{
public:
	void Set(const TVector3DTemp<T> &s, const TVector3DTemp<T>& e, T r)
	{
		start = s;
		end = e;
		radius = r;
	}
	void SetRadius(T r)
	{
		radius = r;
	}
	void SetHeight(T h)
	{
		TVector3DTemp<T> dir = end - start;
		dir.Normalize();
		end = start + h*dir;
	}

	void SetDirection(const TVector3DTemp<T> & dir)
	{
		float len = (start - end).Size();
		end = start + len*dir;
	}

	TVector3DTemp<T> GetCenter() const
	{
		return (start + end) / 2.0f;
	}

	T GetHeight() const
	{
		return (start - end).Size();
	}

	void SetCenter(const TVector3DTemp<T>& c)
	{
		TVector3DTemp<T> center = GetCenter();
		TVector3DTemp<T> delta = c - center;
		start += delta;
		end += delta;
	}

	bool GetIntersectionWithLine(const TLineTemp<T>& line) const
	{
		bool bintersect = false;

		TVector3DTemp<T> dirLine = line.end - line.start;
		dirLine.normalize();

		TVector3DTemp<T> r0, r1;
		r0 = line.GetClosestPoint(start);
		r1 = line.GetClosestPoint(end);

		float d0, d1;
		d0 = r0.GetDistanceFrom(start);
		d1 = r1.GetDistanceFrom(end);

		if (d0 < radius || d1 < radius)
			return true;

		TVector3DTemp<T> capDir = end - start;
		capDir.Normalize();

		float f = dirLine.Dot(capDir);

		if (abs(f) < 0.999f)
		{
			TVector3DTemp<T> normal = dirLine.CrossProduct(capDir);
			TPlaneTemp<T> plane;
			plane.Set(line.start, normal);

			float dist = plane.GetDistanceTo(start);

			TVector3DTemp<T> projV0, projV1;

			projV0 = start + dist*normal;
			projV1 = end + dist*normal;

			float k = (line.end.y - line.start.y) / (line.end.X - line.start.X);
			float b = k*line.end.X - line.end.y;

			float f0 = projV0.y - k*projV0.X + b;
			float f1 = projV1.y - k*projV1.X + b;

			bool bInside = (f0 > 0 && f1 > 0) || (f0 < 0 && f1 < 0);

			if (bInside)
			{
				return false;
			}

			if (abs(dist) < radius)
			{
				bintersect = true;
			}
		}

		return bintersect;
	}

	bool GetCylinderLineIntersection(TLineTemp<T> &line, T&dist) const
	{
		//from link: http://www.gamedev.net/community/forums/topic.asp?topic_id=467789
		//--------------------------------------------------------------------------
		// Ray : P(t) = O + V * t
		// Cylinder [O, D, r].
		// point Q on cylinder if ((Q - O) x D)^2 = r^2
		//
		// Cylinder [A, B, r].
		// Point P on infinite cylinder if ((P - A) x (B - A))^2 = r^2 * (B - A)^2
		// expand : ((O - A) x (B - A) + t * (V x (B - A)))^2 = r^2 * (B - A)^2
		// equation in the form (X + t * Y)^2 = d
		// where : 
		//  X = (O - A) x (B - A)
		//  Y = V x (B - A)
		//  d = r^2 * (B - A)^2
		// expand the equation :
		// t^2 * (Y . Y) + t * (2 * (X . Y)) + (X . X) - d = 0
		// => second order equation in the form : a*t^2 + b*t + c = 0 where
		// a = (Y . Y)
		// b = 2 * (X . Y)
		// c = (X . X) - d
		//--------------------------------------------------------------------------

		//pseudo code
		// 			Vector AB = (B - A);
		// 			Vector AO = (O - A);
		// 			Vector AOxAB = (AO ^ AB); // cross product
		// 			Vector VxAB  = (V ^ AB); // cross product
		// 			float  ab2   = (AB * AB); // dot product
		// 			float a      = (VxAB * VxAB); // dot product
		// 			float b      = 2 * (VxAB * AOxAB); // dot product
		// 			float c      = (AOxAB * AOxAB) - (r*r * ab2);

		T lengthLine;

		TVector3DTemp<T> O, V;
		TVector3DTemp<T> A, B;
		T r;

		O = line.start;
		V = line.Vector();
		lengthLine = V.Size();
		V /= lengthLine;;

		A = start;
		B = end;
		r = radius;

		TVector3DTemp<T> AB = B - A;
		TVector3DTemp<T> AO = O - A;
		TVector3DTemp<T> AOxAB = AO.CrossProduct(AB);
		TVector3DTemp<T> VxAB = V.CrossProduct(AB);

		T ab2 = AB.Dot(AB);
		T a = VxAB.Dot(VxAB);
		T b = 2 * VxAB.Dot(AOxAB);
		T c = AOxAB.Dot(AOxAB) - r*r*ab2;

		T v = b*b - 4 * a*c;
		if (v < 0)
			return false;

		v = sqrt(v);

		T t, root1, root2;
		root1 = (-b + v) / (2 * a);
		root2 = (-b - v) / (2 * a);

		dist = (T)1000000;

		if (root1 > 0)
			dist = root1;
		if (root2 > 0)
		{
			if (root2 < dist)
				dist = root2;
		}
		if (dist >= lengthLine)
			return false;

		TVector3DTemp<T> pos = line.start + V*dist;

		//cylinder
		T h = (end - start).Size();
		TVector3DTemp<T> dir = AB;
		dir /= h;

		T hPos = pos.Dot(dir);

		if ((hPos > h + (T)0.001) || (hPos < (T)-0.001))
		{
			return false;
		}

		return true;
	}
public:
	T  radius;						// radius.
	TVector3DTemp<T> end;			// end  sphere center
	TVector3DTemp<T> start;			// start sphere center
};

typedef TCapsuleTemp<float> kCapsule;


