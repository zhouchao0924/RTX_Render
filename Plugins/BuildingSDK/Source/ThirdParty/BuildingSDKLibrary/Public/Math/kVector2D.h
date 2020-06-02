
#pragma once

#include "kMathBase.h"

template <class T>
class TVector2DTemp
{
public:

	TVector2DTemp() : X(0), Y(0) {};
	TVector2DTemp(T v) :X(v), Y(v) {};
	TVector2DTemp(T nx, T ny) : X(nx), Y(ny) {};
	TVector2DTemp(const TVector2DTemp<T>& other) :X(other.X), Y(other.Y) {};

	// operators

	TVector2DTemp<T> operator-() const { return TVector2DTemp<T>(-X, -Y); }

	TVector2DTemp<T>& operator=(const TVector2DTemp<T>& other) { X = other.X; Y = other.Y; return *this; }

	TVector2DTemp<T> operator+(const TVector2DTemp<T>& other) const { return TVector2DTemp<T>(X + other.X, Y + other.Y); }
	TVector2DTemp<T>& operator+=(const TVector2DTemp<T>& other) { X += other.X; Y += other.Y; return *this; }

	TVector2DTemp<T> operator-(const TVector2DTemp<T>& other) const { return TVector2DTemp<T>(X - other.X, Y - other.Y); }
	TVector2DTemp<T>& operator-=(const TVector2DTemp<T>& other) { X -= other.X; Y -= other.Y; return *this; }

	TVector2DTemp<T> operator*(const TVector2DTemp<T>& other) const { return TVector2DTemp<T>(X * other.X, Y * other.Y); }
	TVector2DTemp<T>& operator*=(const TVector2DTemp<T>& other) { X *= other.X; Y *= other.Y; return *this; }
	TVector2DTemp<T> operator*(const T v) const { return TVector2DTemp<T>(X * v, Y * v); }
	TVector2DTemp<T>& operator*=(const T v) { X *= v; Y *= v; return *this; }

	TVector2DTemp<T> operator/(const TVector2DTemp<T>& other) const { return TVector2DTemp<T>(X / other.X, Y / other.Y); }
	TVector2DTemp<T>& operator/=(const TVector2DTemp<T>& other) { X /= other.X; Y /= other.Y; return *this; }
	TVector2DTemp<T> operator/(const T v) const { return TVector2DTemp<T>(X / v, Y / v); }
	TVector2DTemp<T>& operator/=(const T v) { X /= v; Y /= v; return *this; }

	bool operator==(const TVector2DTemp<T>& other) const { return other.X == X && other.Y == Y; }
	bool operator!=(const TVector2DTemp<T>& other) const { return other.X != X || other.Y != Y; }

	// functions

	//! returns if this vector equals the other one, taking floating point rounding errors into account
	bool Equals(const TVector2DTemp<T>& other, float tolerence = K_ROUNDING_ERROR)const
	{
		return Equals_Float(X, other.X, tolerence) && Equals_Float(Y, other.Y, tolerence);
	}

	//! returns if this vector equals (0,0), taking floating point rounding errors into account
	bool IsZero(float tolerence = K_ROUNDING_ERROR)const
	{
		return Equals_Float(X, 0, tolerence) && Equals_Float(Y, 0, tolerence);
	}


	void Set(const T& nx, const T& ny) { X = nx; Y = ny; }
	void Set(const TVector2DTemp<T>& p) { X = p.X; Y = p.Y; }

	//! Returns the length of the vector
	//! \return Returns the length of the vector.
	float Size() const { return sqrt((X*X + Y*Y)); }
	float SizeSquared() const { return ( (X*X + Y*Y)); }

	//! Returns the dot product of this vector with an other.
	T Dot(const TVector2DTemp<T>& other) const
	{
		return X*other.X + Y*other.Y;
	}

	//! Returns distance from an other point. Here, the vector is interpreted as
	//! point in 2 dimensional space.
	double GetDistanceFrom(const TVector2DTemp<T>& other) const
	{
		double vx = X - other.X;
		double vy = Y - other.Y;
		return sqrt(vx*vx + vy*vy);
	}

	//! rotates the point around a center by an amount of degrees.
	void RotateBy(float degrees, const TVector2DTemp<T>& center)
	{
		degrees *= K_G_RAD;
		T cs = (T)cos(degrees);
		T sn = (T)sin(degrees);

		X -= center.X;
		Y -= center.Y;

		Set(X*cs - Y*sn, X*sn + Y*cs);

		X += center.X;
		Y += center.Y;
	}

	//! normalizes the vector.
	TVector2DTemp<T>& Normalize()
	{
		T l = (T)Size();
		if (l == 0)
			return *this;

		l = (T)1.0 / l;
		X *= l;
		Y *= l;
		return *this;
	}

	//! Calculates the angle of this vector in grad in the trigonometric sense.
	//! This method has been suggested by Pr3t3nd3r.
	//! \return Returns a value between 0 and 360.
	inline double GetAngleTrig() const
	{
		if (X == 0.0)
			return Y < 0.0 ? 270.0 : 90.0;
		else
			if (Y == 0)
				return X < 0.0 ? 180.0 : 0.0;

		if (Y > 0.0)
			if (X > 0.0)
				return atan(Y / X) * K_RAD_G;
			else
				return 180.0 - atan(Y / -X) * K_RAD_G;
		else
			if (X > 0.0)
				return 360.0 - atan(-Y / X) * K_RAD_G;
			else
				return 180.0 + atan(-Y / -X) * K_RAD_G;
	}

	//! Calculates the angle of this vector in grad in the counter trigonometric sense.
	//! \return Returns a value between 0 and 360.
	float GetAngle()
	{
		if (Y == 0.0)  // corrected thanks to a suggestion by Jox
		{
			return X < 0.0 ? 180.0f : 0.0f;
		}
		else if (X == 0.0)
		{
			return Y < 0.0 ? 90.0f : 270.0f;
		}

		double tmp = Y / sqrt(X*X + Y*Y);
		tmp = atan(sqrt(1 - tmp*tmp) / tmp) * K_RAD_G;

		if (X > 0.0 && Y > 0.0)
			return float(tmp) + 270.0f;
		else
			if (X > 0.0 && Y < 0.0)
				return float(tmp) + 90.0f;
			else
				if (X < 0.0 && Y < 0.0)
					return 90.0f - float(tmp);
				else
					if (X<0.0 && Y>0.0)
						return 270.0f - float(tmp);

		return float(tmp);
	}

	//! Calculates the angle between this vector and another one in grad.
	//! \return Returns a value between 0 and 90.
	inline float GetAngleWith(const TVector2DTemp<T>& b) const
	{
		float tmp = X*b.X + Y*b.Y;

		if (tmp == 0.0)
			return 90.0;

		tmp = tmp / sqrt((X*X + Y*Y) * (b.X*b.X + b.Y*b.Y));
		if (tmp < 0.0) tmp = -tmp;

		return atan(sqrt(1 - tmp*tmp) / tmp) * K_RAD_G;
	}


	//! returns interpolated vector
	//! \param other: other vector to interpolate between
	//! \param d: value between 0.0f and 1.0f.
	TVector2DTemp<T> GetInterpolated(const TVector2DTemp<T>& other, float d) const
	{
		float inv = 1.0f - d;
		return TVector2DTemp<T>(other.X*inv + X*d,
			other.Y*inv + Y*d);
	}

	void Rotate(const TVector2DTemp<T>& point, float angle)
	{
		double sinAngle = sin(angle);
		double cosAngle = cos(angle);

		if (point.IsZero())
		{
			float tempX = x * cosAngle - y * sinAngle;
			y = y * cosAngle + x * sinAngle;
			x = tempX;
		}
		else
		{
			float tempX = x - point.X;
			float tempY = y - point.Y;

			x = tempX * cosAngle - tempY * sinAngle + point.X;
			y = tempY * cosAngle + tempX * sinAngle + point.Y;
		}
	}

	void ForAngle(const float a)
	{
		x = cosf(a);
		y = sinf(a);
	}

	TVector2DTemp<T> Rotate(const TVector2DTemp<T>& other) const {
		return TVector2DTemp<T>(x*other.x - y*other.y, x*other.y + y*other.x);
	};

	TVector2DTemp<T> RotateByAngle(const TVector2DTemp<T> & pivot, float angle) const
	{
		TVector2DTemp<T> r;
		r.ForAngle(angle);
		return pivot + (*this - pivot).Rotate(r);
	}

	// cross product of 2 vector. A->B X C->D
	static float CrossProduct2Vector(const TVector2DTemp<T>& A, const TVector2DTemp<T>& B, const TVector2DTemp<T>& C, const TVector2DTemp<T>& D)
	{
		return (D.Y - C.Y) * (B.X - A.X) - (D.X - C.X) * (B.Y - A.Y);
	}

	static float CrossProduct(const TVector2DTemp<T>& A, const TVector2DTemp<T>& B)
	{
		return A.X*B.Y - A.Y*B.X;
	}

	static bool IsLineIntersect(const TVector2DTemp<T>& A, const TVector2DTemp<T>& B, const TVector2DTemp<T>& C, const TVector2DTemp<T>& D, float *s, float *t)
	{
		// FAIL: Line undefined
		if ((A.X == B.X && A.Y == B.Y) || (C.X == D.X && C.Y == D.Y))
		{
			return false;
		}

		const float denom = CrossProduct2Vector(A, B, C, D);

		if (denom == 0)
		{
			// Lines parallel or overlap
			return false;
		}

		if (s != nullptr) *s = CrossProduct2Vector(C, D, C, A) / denom;
		if (t != nullptr) *t = CrossProduct2Vector(A, B, C, A) / denom;

		return true;
	}


	TVector2DTemp<T> Lerp(const TVector2DTemp<T>& other, float alpha) const
	{
		return *this * (1.f - alpha) + other * alpha;
	};

	TVector2DTemp<T> GetClampPoint(const TVector2DTemp<T>& min_inclusive, const TVector2DTemp<T>& max_inclusive) const
	{
		return TVector2DTemp<T>(std::clamp<T>(X, min_inclusive.X, max_inclusive.X), std::clamp<T>(Y, min_inclusive.Y, max_inclusive.Y));
	}

	TVector2DTemp<T> Prep() const
	{
		return TVector2DTemp<T>(-Y, X);
	}

	void ScaleSigned(T v)
	{
		X = (T)floor((double)X / (double)v);
		Y = (T)floor((double)Y / (double)v);
	}

	T Area() const
	{
		return Width * Height;
	}

	// member variables
	union
	{
		struct
		{
			T X, Y;
		};
		struct
		{
			T x, y;
		};
		struct
		{
			T U, V;
		};
		struct
		{
			T Width, Height;
		};
		struct
		{
			T width, height;
		};
		struct
		{
			T Value[2];
		};
	};
};

typedef TVector2DTemp<float>		kPoint;
typedef TVector2DTemp<float>		kSize2D;
typedef TVector2DTemp<float>		kTexUV;


