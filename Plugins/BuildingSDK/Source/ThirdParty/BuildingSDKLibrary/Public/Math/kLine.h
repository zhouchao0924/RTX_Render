
#pragma once

#include "kMathBase.h"
#include "kVector3D.h"
#include "kLine2D.h"

//! 3D line between two points with intersection methods.
template <class T>
class TLineTemp
{
public:
	// Constructors
	TLineTemp() : start(0, 0, 0), end(1, 1, 1) {};
	TLineTemp(T xa, T ya, T za, T xb, T yb, T zb) : start(xa, ya, za), end(xb, yb, zb) {};
	TLineTemp(const TVector3DTemp<T>& start, const TVector3DTemp<T>& end) : start(start), end(end) {};
	TLineTemp(const TLineTemp<T>& other) :start(other.start), end(other.end) {};

	// operators

	TLineTemp<T> operator+(const TVector3DTemp<T>& point) const { return TLineTemp<T>(start + point, end + point); };
	TLineTemp<T>& operator+=(const TVector3DTemp<T>& point) { start += point; end += point; return *this; };

	TLineTemp<T> operator-(const TVector3DTemp<T>& point) const { return TLineTemp<T>(start - point, end - point); };
	TLineTemp<T>& operator-=(const TVector3DTemp<T>& point) { start -= point; end -= point; return *this; };

	bool operator==(const TLineTemp<T>& other) const { return (start == other.start && end == other.end) || (end == other.start && start == other.end); };
	bool operator!=(const TLineTemp<T>& other) const { return !(start == other.start && end == other.end) || (end == other.start && start == other.end); };

	// functions

	void Set(const T& xa, const T& ya, const T& za, const T& xb, const T& yb, const T& zb) { start.Set(xa, ya, za); end.Set(xb, yb, zb); }
	void Set(const TVector3DTemp<T>& nstart, const TVector3DTemp<T>& nend) { start.Set(nstart); end.Set(nend); }
	void Set(const TLineTemp<T>& line) { start.set(line.start); end.Set(line.end); }

	//! Returns length of line
	//! \return Returns length of line.
	T Size() const { return start.GetDistanceFrom(end); };

	//! Returns sqared length of line
	//! \return Returns sqared length of line.
	T SizeSquared() const { return start.GetDistanceFromSQ(end); };

	//! Returns middle of line
	TVector3DTemp<T> Middle() const
	{
		return (start + end) * (T)0.5;
	}

	//! Returns vector of line
	TVector3DTemp<T> Vector() const
	{
		return end - start;
	}

	//! Returns if the overgiven point is between start and end of the
	//! line. Assumes that the point is already somewhere on the line.
	bool IsPointBetweenStartAndEnd(const TVector3DTemp<T>& point) const
	{
		return point.IsBetweenPoints(start, end);
	}

	//! Returns the closest point on this line to a point
	TVector3DTemp<T> GetClosestPoint(const TVector3DTemp<T>& point) const
	{
		TVector3DTemp<T> c = point - start;
		TVector3DTemp<T> v = end - start;
		T d = (T)v.Size();
		v /= d;
		T t = v.Dot(c);

		if (t < (T)0.0) return start;
		if (t > d) return end;
		v *= t;
		return start + v;
	}

	//get the projection on this line for the point
	void GetProjectionPoint(const TVector3DTemp<T>& point, TVector3DTemp<T>&pointProj) const
	{
		TVector3DTemp<T> c = point - start;
		TVector3DTemp<T> v = end - start;
		T d = (T)v.Size();
		v /= d;
		T t = v.Dot(c);
		v *= t;
		pointProj = start + v;
	}

	TLineTemp<T> GetInverseLine() const 
	{
		return TLineTemp<T>(end, start);
	}

	TLine2DTemp<T> XY() const
	{
		return TLine2DTemp<T>(start.XY(), end.XY());
	}

	TVector3DTemp<T> start;
	TVector3DTemp<T> end;
};

typedef TLineTemp<float> kLine;



