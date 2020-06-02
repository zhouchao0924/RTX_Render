
#pragma once

#include "kVector3D.h"

//! 3d vector template class with lots of operators and methods.
template <class T>
class TVector4DTemp
{
public:

	TVector4DTemp() : x(0), y(0), z(0), w(0) {};
	TVector4DTemp(T v) : x(v), y(v), z(v), w(v) {};
	TVector4DTemp(T nx, T ny, T nz, T nw = 1.0f) : x(nx), y(ny), z(nz), w(nw) {};
	TVector4DTemp(const TVector4DTemp<T>& other) :x(other.x), y(other.y), z(other.z), w(other.w) {};

	// operators
	TVector4DTemp<T> operator-() const { return TVector4DTemp<T>(-x, -y, -z, -w); }
	TVector4DTemp<T>& operator=(const TVector4DTemp<T>& other) { x = other.X; y = other.y; z = other.z; w = other.w; return *this; }
	TVector4DTemp<T>& operator=(const TVector3DTemp<T>& other) { x = other.X; y = other.y; z = other.z; w = 1.0f; return *this; }

	TVector4DTemp<T> operator+(const TVector4DTemp<T>& other) const { return TVector4DTemp<T>(x + other.X, y + other.y, z + other.z, w + other.w); }
	TVector4DTemp<T>& operator+=(const TVector4DTemp<T>& other) { x += other.X; y += other.y; z += other.z; w += other.w; return *this; }

	TVector4DTemp<T> operator-(const TVector4DTemp<T>& other) const { return TVector4DTemp<T>(x - other.X, y - other.y, z - other.z, w - other.w); }
	TVector4DTemp<T>& operator-=(const TVector4DTemp<T>& other) { x -= other.X; y -= other.y; z -= other.z; w -= other.w; return *this; }

	TVector4DTemp<T> operator*(const TVector4DTemp<T>& other) const { return TVector4DTemp<T>(x * other.X, y * other.y, z * other.z, w*other.w); }
	TVector4DTemp<T>& operator*=(const TVector4DTemp<T>& other) { x *= other.X; y *= other.y; z *= other.z; w *= other.w; return *this; }
	TVector4DTemp<T> operator*(const T v) const { return TVector4DTemp<T>(x * v, y * v, z * v, w*v); }
	TVector4DTemp<T>& operator*=(const T v) { x *= v; y *= v; z *= v; w *= v; return *this; }

	TVector4DTemp<T> operator/(const TVector4DTemp<T>& other) const { return TVector4DTemp<T>(x / other.X, y / other.y, z / other.z, w / other.w); }
	TVector4DTemp<T>& operator/=(const TVector4DTemp<T>& other) { x /= other.X; y /= other.y; z /= other.z; w /= other.w; return *this; }
	TVector4DTemp<T> operator/(const T v) const { return TVector4DTemp<T>(x / v, y / v, z / v, w / v); }
	TVector4DTemp<T>& operator/=(const T v) { x /= v; y /= v; z /= v; w /= v; return *this; }

	bool operator<=(const TVector4DTemp<T>&other) const { return x <= other.X && y <= other.y && z <= other.z&& w <= other.w; };
	bool operator>=(const TVector4DTemp<T>&other) const { return x >= other.X && y >= other.y && z >= other.z&& w >= other.w; };

	bool operator==(const TVector4DTemp<T>& other) const { return other.X == x && other.y == y && other.z == z&&other.w == w; }
	bool operator!=(const TVector4DTemp<T>& other) const { return other.X != x || other.y != y || other.z != z || other.w != w; }

	// functions

	//! returns if this vector equals the other one, taking floating point rounding errors into account
	bool Equals(const TVector4DTemp<T>& other)const
	{
		return  Equals_Float(x, other.X) &&
			Equals_Float(y, other.y) &&
			Equals_Float(z, other.z) &&
			Equals_Float(w, other.w);
	}

	//! returns if this vector equals (0,0,0), taking floating point rounding errors into account
	bool IsZero()const
	{
		return  Equals_Float(x, 0) &&
			Equals_Float(y, 0) &&
			Equals_Float(z, 0) &&
			Equals_Float(w, 0);
	}

	bool EqualsOne()const
	{
		return  Equals_Float(x, 1) &&
			Equals_Float(y, 1) &&
			Equals_Float(z, 1) &&
			Equals_Float(w, 1);
	}

	void Set(const T nx, const T ny, const T nz, const T nw) { x = nx; y = ny; z = nz; w = nw; }
	void Set(const TVector4DTemp<T>& p) { x = p.X; y = p.y; z = p.z; w = p.w; }
	void SetZero() { x = 0; y = 0; z = 0; w = 0; }

	//! Returns the dot product with another vector.
	T Dot(const TVector4DTemp<T>& other) const
	{
		return x*other.X + y*other.y + z*other.z + w*other.w;
	}

	T SizeSquared() const
	{
		return x*x + y*y + z*z + w*w;
	}

	void FromDwordColor(unsigned int c)
	{
		unsigned char *p = (unsigned char *)&c;
		b = ((T)p[0]) / ((T)255);
		g = ((T)p[1]) / ((T)255);
		r = ((T)p[2]) / ((T)255);
		a = ((T)p[3]) / ((T)255);
	}

	void ToDwordColor(unsigned int &c)
	{
		unsigned char *p = (unsigned char *)&c;
		p[0] = std::clamp((int)(b*255.0 + 0.5), 0, 255);
		p[1] = std::clamp((int)(g*255.0 + 0.5), 0, 255);
		p[2] = std::clamp((int)(r*255.0 + 0.5), 0, 255);
		p[3] = std::clamp((int)(a*255.0 + 0.5), 0, 255);
	}

	void FromColor3D(TVector3DTemp<T> &c)
	{
		r = c.r;
		g = c.g;
		b = c.b;
		a = 1;
	}

	float Dot3(const TVector4DTemp &oth) const
	{
		return X*oth.X + Y*oth.Y + Z*oth.Z;
	}

	TVector4DTemp operator^(const TVector4DTemp& V) const
	{
		return TVector4DTemp(
			Y * V.Z - Z * V.Y,
			Z * V.X - X * V.Z,
			X * V.Y - Y * V.X,
			0.0f
		);
	}

	TVector4DTemp Cross(const TVector4DTemp& V) const
	{
		return TVector4DTemp(
			Y * V.Z - Z * V.Y,
			Z * V.X - X * V.Z,
			X * V.Y - Y * V.X,
			0.0f
		);
	}

	float& operator[](int ComponentIndex)
	{
		return weight[ComponentIndex];
	}


	float operator[](int ComponentIndex) const
	{
		return weight[ComponentIndex];
	}

	float Size3() const
	{
		return XYZ().Size();
	}

	TVector3DTemp<T> XYZ() const
	{
		return TVector3DTemp<T>(x, y, z);
	}

	float SizeSquared3() const
	{
		return X*X + Y*Y + Z*Z;
	}

	bool IsUnit3(float LengthSquaredTolerance = K_SMALL_NUMBER) const
	{
		return rsAbs(1.0f - SizeSquared3()) < LengthSquaredTolerance;
	}

	TVector4DTemp GetUnsafeNormal3() const
	{
		const float Scale = rosy::InvSqrt(X*X + Y*Y + Z*Z);
		return TVector4DTemp(X*Scale, Y*Scale, Z*Scale, 0.0f);
	}

	TVector4DTemp GetSafeNormal(float Tolerance = K_SMALL_NUMBER) const
	{
		const float SquareSum = X*X + Y*Y + Z*Z;
		if (SquareSum > Tolerance)
		{
			const float Scale = rosy::InvSqrt(SquareSum);
			return TVector4DTemp(X*Scale, Y*Scale, Z*Scale, 0.0f);
		}
		return TVector4DTemp(0.f);
	}

	// member variables
	union
	{
		struct
		{
			T x, y, z, w;
		};
		struct
		{
			T X, Y, Z, W;
		};
		struct
		{
			T r, g, b, a;
		};
		T weight[4];
	};
};

typedef TVector4DTemp<float> kVector4D;








