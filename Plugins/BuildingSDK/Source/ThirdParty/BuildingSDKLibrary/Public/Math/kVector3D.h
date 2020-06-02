
#pragma once

#include "kMathBase.h"
#include "kVector2D.h"

template <class T>
class TRotationTemp
{
public:
	TRotationTemp()
	{
		Pitch = Yaw = Roll = 0;
	}

	TRotationTemp(float InPitch, float InYaw, float InRoll)
	{
		Yaw = InYaw;
		Roll = InRoll;
		Pitch = InPitch;
	}

	TRotationTemp<T> ToRadian()
	{
		TRotationTemp<T> v;
		v.Yaw = Yaw * K_G_RAD;
		v.Roll = Roll * K_G_RAD;
		v.Pitch = Pitch* K_G_RAD;
		return v;
	}

	TRotationTemp<T> ToDegress()
	{
		TRotationTemp<T> v;
		v.Yaw = Yaw * K_RAD_G;
		v.Roll = Roll * K_RAD_G;
		v.Pitch = Pitch * K_RAD_G;
		return v;
	}

	TRotationTemp<T> operator *(float Ratio) const { return TRotationTemp<T>(Pitch*Ratio, Yaw*Ratio, Roll*Ratio); }
	TRotationTemp<T> operator+(const TRotationTemp<T>& other) const { return TRotationTemp<T>(Pitch + other.Pitch, Yaw + other.Yaw, Roll + other.Roll); }
	TRotationTemp<T>& operator+=(const TRotationTemp<T>& other) { Pitch += other.Pitch; Yaw += other.Yaw; Roll += other.Roll; return *this; }
	TRotationTemp<T> operator-(const TRotationTemp<T>& other) const { return TRotationTemp<T>(Pitch - other.Pitch, Yaw - other.Yaw, Roll - other.Roll); }
	TRotationTemp<T>& operator-=(const TRotationTemp<T>& other) { Pitch -= other.Pitch; Yaw -= other.Yaw; Roll -= other.Roll; return *this; }

	bool IsZero() { return Pitch == 0 && Yaw == 0 && Roll == 0; }

	T Pitch, Yaw, Roll;
};

template <class T>
class TVector3DTemp
{
public:
	TVector3DTemp() : X(0), Y(0), Z(0) {};
	TVector3DTemp(T v) : X(v), Y(v), Z(v) {};
	TVector3DTemp(T nx, T ny, T nz) : X(nx), Y(ny), Z(nz) {};
	TVector3DTemp(const TVector3DTemp<T>& other) :X(other.X), Y(other.Y), Z(other.Z) {};
	TVector3DTemp(const TVector2DTemp<T> &other) :X(other.X), Y(other.Y), Z(0) {}
	TVector3DTemp(const TVector2DTemp<T> &other, T InZ) :X(other.X), Y(other.Y), Z(InZ) {}

	// operators

	TVector3DTemp<T> operator-() const { return TVector3DTemp<T>(-X, -Y, -Z); }

	TVector3DTemp<T>& operator=(const TVector3DTemp<T>& other) { X = other.X; Y = other.Y; Z = other.Z; return *this; }

	TVector3DTemp<T> operator+(const TVector3DTemp<T>& other) const { return TVector3DTemp<T>(X + other.X, Y + other.Y, Z + other.Z); }
	TVector3DTemp<T>& operator+=(const TVector3DTemp<T>& other) { X += other.X; Y += other.Y; Z += other.Z; return *this; }

	TVector3DTemp<T> operator-(const TVector3DTemp<T>& other) const { return TVector3DTemp<T>(X - other.X, Y - other.Y, Z - other.Z); }
	TVector3DTemp<T>& operator-=(const TVector3DTemp<T>& other) { X -= other.X; Y -= other.Y; Z -= other.Z; return *this; }

	TVector3DTemp<T> operator*(const TVector3DTemp<T>& other) const { return TVector3DTemp<T>(X * other.X, Y * other.Y, Z * other.Z); }
	TVector3DTemp<T>& operator*=(const TVector3DTemp<T>& other) { X *= other.X; Y *= other.Y; Z *= other.Z; return *this; }
	TVector3DTemp<T> operator*(const T v) const { return TVector3DTemp<T>(X * v, Y * v, Z * v); }
	TVector3DTemp<T>& operator*=(const T v) { X *= v; Y *= v; Z *= v; return *this; }

	TVector3DTemp<T> operator/(const TVector3DTemp<T>& other) const { return TVector3DTemp<T>(X / other.X, Y / other.Y, Z / other.Z); }
	TVector3DTemp<T>& operator/=(const TVector3DTemp<T>& other) { X /= other.X; Y /= other.Y; Z /= other.Z; return *this; }
	TVector3DTemp<T> operator/(const T v) const { return TVector3DTemp<T>(X / v, Y / v, Z / v); }
	TVector3DTemp<T>& operator/=(const T v) { T i = (T)1.0 / v; X *= i; Y *= i; Z *= i; return *this; }

	bool operator<=(const TVector3DTemp<T>&other) const { return X <= other.X && Y <= other.Y && Z <= other.Z; };
	bool operator>=(const TVector3DTemp<T>&other) const { return X >= other.X && Y >= other.Y && Z >= other.Z; };

	bool operator==(const TVector3DTemp<T>& other) const { return other.X == X && other.Y == Y && other.Z == Z; }
	bool operator!=(const TVector3DTemp<T>& other) const { return other.X != X || other.Y != Y || other.Z != Z; }

	// functions

	//! returns if this vector equals the other one, taking floating point rounding errors into account
	bool Equals(const TVector3DTemp<T>& other)const
	{
		return Equals_Float(X, other.X, 0.001f) && Equals_Float(Y, other.Y, 0.001f) && Equals_Float(Z, other.Z, 0.001f);
	}

	bool Equals(const TVector3DTemp<T>& other, T epsilon)const
	{
		return Equals_Float(X, other.X, epsilon) && Equals_Float(Y, other.Y, epsilon) && Equals_Float(Z, other.Z, epsilon);
	}

	//! returns if this vector equals (0,0,0), taking floating point rounding errors into account
	bool IsZero()const
	{
		return Equals_Float(X, 0, K_ROUNDING_ERROR) && Equals_Float(Y, 0, K_ROUNDING_ERROR) && Equals_Float(Z, 0, K_ROUNDING_ERROR);
	}

	bool EqualsOne()const
	{
		return Equals_Float(X, 1, K_ROUNDING_ERROR) && Equals_Float(Y, 1, K_ROUNDING_ERROR) && Equals_Float(Z, 1, K_ROUNDING_ERROR);
	}

	bool IsUniform()const//����ֵ����ͬ��Ϊuniform
	{
		return Equals_Float(X, Y, K_ROUNDING_ERROR) && Equals_Float(Y, Z, K_ROUNDING_ERROR) && Equals_Float(Z, X, K_ROUNDING_ERROR);
	}

	bool DivUniform(TVector3DTemp<T> &target, T&result, T epsilon = 0.0001f)
	{
		if (target.X != (T)0)
			result = X / target.X;
		else
		{
			if (target.Y != (T)0)
				result = Y / target.Y;
			else
			{
				if (target.Z != (T)0)
					result = Z / target.Z;
				else
					return false;
			}
		}

		TVector3DTemp<T> t;
		t = target*result;

		if (Equals(t, epsilon))
			return true;

		return false;
	}



	void Set(const T nx, const T ny, const T nz) { X = nx; Y = ny; Z = nz; }
	void Set(const TVector3DTemp<T>& p) { X = p.X; Y = p.Y; Z = p.Z; }
	void SetZero() { X = 0; Y = 0; Z = 0; }

	//! Returns length of the vector.
	float Size() const { return sqrt(X*X + Y*Y + Z*Z); }
	float SizeXZ() const { return sqrt(X*X + Z*Z); }
	float SizeXY() const { return sqrt(X*X + Y*Y); }
	float SizeYZ() const { return sqrt(Z*Z + Y*Y); }

	float GetMax() const
	{
		return std::max(std::max(X, Y), Z);
	}

	float GetAbsMax() const
	{
		return std::max(std::max(std::abs(X), std::abs(Y)), std::abs(Z));
	}

	float GetMin() const
	{
		return std::min(std::min(X, Y), Z);
	}

	float GetAbsMin() const
	{
		return std::min(std::min(std::abs(X), std::abs(Y)), std::abs(Z));
	}

	//! Returns squared length of the vector.
	/** This is useful because it is much faster then
	getLength(). */
	float SizeSquared() const { return X*X + Y*Y + Z*Z; }
	float SizeSquared_XZ() const { return (X*X + Z*Z); }
	float SizeSquared_XY() const { return (X*X + Y*Y); }
	float SizeSquared_YZ() const { return (Z*Z + Y*Y); }

	//! Returns the dot product with another vector.
	T Dot(const TVector3DTemp<T>& other) const
	{
		return X*other.X + Y*other.Y + Z*other.Z;
	}

	//! Returns distance from an other point.
	/** Here, the vector is interpreted as point in 3 dimensional space. */
	float GetDistanceFrom(const TVector3DTemp<T>& other) const
	{
		float vx = X - other.X; float vy = Y - other.Y; float vz = Z - other.Z;
		return sqrt(vx*vx + vy*vy + vz*vz);
	}

	//! Returns squared distance from an other point. 
	/** Here, the vector is interpreted as point in 3 dimensional space. */
	float GetDistanceFromSQ(const TVector3DTemp<T>& other) const
	{
		float vx = X - other.X; float vy = Y - other.Y; float vz = Z - other.Z;
		return (vx*vx + vy*vy + vz*vz);
	}

	//! Calculates the cross product with another vector
	TVector3DTemp<T> CrossProduct(const TVector3DTemp<T>& p) const
	{
		return TVector3DTemp<T>(Y * p.Z - Z * p.Y, Z * p.X - X * p.Z, X * p.Y - Y * p.X);
	}

	//! Returns if this vector interpreted as a point is on a line between two other points.
	/** It is assumed that the point is on the line. */
	bool IsBetweenPoints(const TVector3DTemp<T>& begin, const TVector3DTemp<T>& end) const
	{
		float f = (float)(end - begin).SizeSquared();
		return GetDistanceFromSQ(begin) <= f &&  GetDistanceFromSQ(end) <= f;
	}

	//! Normalizes the vector.
	TVector3DTemp<T>& Normalize()
	{
		T l = (T)Size();
		if (l == 0)
			return *this;

		l = (T)1.0 / l;
		X *= l;
		Y *= l;
		Z *= l;
		return *this;
	}

	//! Sets the lenght of the vector to a new value
	void SetLength(T newlength)
	{
		Normalize();
		*this *= newlength;
	}

	//newdir should be normalized
	void SetDirection(TVector3DTemp<T> &newdir)
	{
		TVector3DTemp<T>t;
		t = newdir;
		t *= (T)Size();
		*this = t;
	}

	//! Inverts the vector.
	void Invert()
	{
		X *= -1.0f;
		Y *= -1.0f;
		Z *= -1.0f;
	}

	void RotateXZBy(float degrees, const TVector3DTemp<T>& center)
	{
		degrees *= K_G_RAD;
		T cs = (T)cos(degrees);
		T sn = (T)sin(degrees);
		X -= center.X;
		Z -= center.Z;
		Set(X*cs - Z*sn, Y, X*sn + Z*cs);
		X += center.X;
		Z += center.Z;
	}

	void RotateXYBy(float degrees, const TVector3DTemp<T>& center)
	{
		degrees *= K_G_RAD;
		T cs = (T)cos(degrees);
		T sn = (T)sin(degrees);
		X -= center.X;
		Y -= center.Y;
		Set(X*cs - Y*sn, X*sn + Y*cs, Z);
		X += center.X;
		Y += center.Y;
	}

	void RotateYZBy(float degrees, const TVector3DTemp<T>& center)
	{
		degrees *= K_G_RAD;
		T cs = (T)cos(degrees);
		T sn = (T)sin(degrees);
		Z -= center.Z;
		Y -= center.Y;
		Set(X, Y*cs - Z*sn, Y*sn + Z*cs);
		Z += center.Z;
		Y += center.Y;
	}

	//! Returns interpolated vector.
	/** \param other: other vector to interpolate between
	\param d: value between 0.0f and 1.0f. */
	//if d is 1, fully me,if d is 0,fully other
	TVector3DTemp<T> GetInterpolated(const TVector3DTemp<T>& other, float d) const
	{
		float inv = 1.0f - d;
		return TVector3DTemp<T>(other.X*inv + X*d,
			other.Y*inv + Y*d,
			other.Z*inv + Z*d);
	}

	static TVector3DTemp<T> Lerp(const TVector3DTemp<T>& other0, const TVector3DTemp<T>& other1, float d)
	{
		float inv = 1.0f - d;
		return TVector3DTemp<T>(other0.X*d + other1.X*inv, other0.Y*d + other1.Y*inv, other0.Z*d + other1.Z*inv);
	}

	//! Gets the Y and Z rotations of a vector.
	/** Thanks to Arras on the Irrlicht forums to add this method.
	\return A vector representing the rotation in degrees of
	this vector. The Z component of the vector will always be 0. */
	TVector3DTemp<T> GetHorizontalAngle()
	{
		TVector3DTemp<T> angle;

		angle.Y = (T)atan2(X, Z);
		angle.Y *= (float)K_RAD_G;

		if (angle.Y < 0.0f) angle.Y += 360.0f;
		if (angle.Y >= 360.0f) angle.Y -= 360.0f;

		float z1;
		z1 = (T)sqrt(X*X + Z*Z);

		angle.X = (T)atan2(z1, Y);
		angle.X *= (float)K_RAD_G;
		angle.X -= 90.0f;

		if (angle.X < 0.0f) angle.X += 360.0f;
		if (angle.X >= 360) angle.X -= 360.0f;

		return angle;
	}

	//convert Direction representation to Head/Pitch/Banking representation
	//should be a normalized direction vector
	TRotationTemp<T> ToAngle_YUp()
	{
		// now calculate the angles
		T h, p, b;

		// banking is always irrelevant
		b = 0;
		// calculate pitch
		p = (T)(asin(Y)*K_RAD_G);

		// if y is near +1 or -1
		if (Y>0.9999 || Y<-0.9999)
			h = 0;
		else
			h = (T)(atan2(-X, -Z)*K_RAD_G);
			
		return TRotationTemp<T>(p, h, b);
	}

	TRotationTemp<T> ToAngle_ZUp()
	{
		TRotationTemp<T> R;

		// Find yaw.
		R.Yaw = atan2(Y, X) * K_RAD_G;

		// Find pitch.
		R.Pitch = atan2(Z, sqrtf(X*X + Y * Y)) * K_RAD_G;
		// Find roll.

		R.Roll = 0;

		return R;
	}

	//convert Head/Pitch/Banking representation to Direction representation 
	static TVector3DTemp<T> GetDirection_ZUp(const TRotationTemp<T> &R)
	{
		float SP = sin(R.Pitch*K_G_RAD);  // heading
		float CP = cos(R.Pitch*K_G_RAD);

		float SY = sin(R.Yaw*K_G_RAD);  // pitch
		float CY = cos(R.Yaw*K_G_RAD);

		return TVector3DTemp<T>(CP*CY, CP*SY, SP);
	}

	//convert Head/Pitch/Banking representation to Direction representation 
	static TVector3DTemp<T> &ToDirection_YUp(const TRotationTemp<T> &R)
	{
		double fSinH = sin(R.Yaw*K_G_RAD);  // heading
		double fCosH = cos(R.Yaw*K_G_RAD);

		double fSinP = sin(R.Pitch*K_G_RAD);  // pitch
		double fCosP = cos(R.Pitch*K_G_RAD);

		X = (T)(-fCosP*fSinH);
		Y = (T)+fSinP;
		Z = (T)(-fCosP*fCosH);

		return TVector3DTemp<T>(X, Y, Z);
	}

// 		//ת��Ϊŷ����
// 		void ToEuler()
// 		{
// 			ToAngle();
// 			X = X * float(G_RAD) + RM_PI;
// 		}

	//! Fills an array of 4 values with the vector data (usually floats).
	/** Useful for setting in shader constants for example. The fourth value
	will always be 0. */
	void GetAs4Values(T* array)
	{
		array[0] = X;
		array[1] = Y;
		array[2] = Z;
		array[3] = 0;
	}

	void FromDwordColor(int c)
	{
		unsigned char *p = (unsigned char *)&c;
		B = ((T)p[0]) / ((T)255);
		G = ((T)p[1]) / ((T)255);
		R = ((T)p[2]) / ((T)255);
	}

	void ToDwordColor(int &c)
	{
		unsigned char *p = (unsigned char *)&c;
		p[0] = std::clamp((int)(B*255.0 + 0.5), 0, 255);
		p[1] = std::clamp((int)(G*255.0 + 0.5), 0, 255);
		p[2] = std::clamp((int)(R*255.0 + 0.5), 0, 255);
		p[3] = 255;
	}

	T ToLightValue()
	{
		return (T)(((float)X)*0.3 + ((float)Y)*0.59 + ((float)Z)*0.11);
	}

	//floor the x,y,z onto the bound of gran
	void Floor(T gran)
	{
		X = ::floor(X / gran)*gran;
		Y = ::floor(Y / gran)*gran;
		Z = ::floor(Z / gran)*gran;
	}

	friend TVector3DTemp<T> GetNormalFrom3Points(const TVector3DTemp<T>& point1, const TVector3DTemp<T>& point2, const TVector3DTemp<T>& point3)
	{
		TVector3DTemp<T>  Normal;
		// creates the plane from 3 memberpoints
		Normal = (point2 - point1).CrossProduct(point3 - point1);
		Normal.Normalize();
		return Normal;
	}

	friend TVector3DTemp<T> operator*(float Scale, const TVector3DTemp<T>& V)
	{
		return TVector3DTemp<T>(V.X * Scale, V.Y * Scale, V.Z * Scale);
	}

	//Ѱ���뱾vector��ֱ����������,������������Ҳ���ഹֱ
	void FindBestAxis(TVector3DTemp<T>& Axis1, TVector3DTemp<T>& Axis2) const
	{
		T NX = abs(X);
		T NY = abs(Z);
		T NZ = abs(Y);

		if (NZ>NX && NZ>NY)	Axis1 = TVector3DTemp<T>(1, 0, 0);
		else					Axis1 = TVector3DTemp(0, 0, 1);


		TVector3DTemp<T> tempV = *this;
		T temp = tempV.Z;
		tempV.Z = tempV.Y;
		tempV.Y = temp;

		Axis1 = (Axis1 - tempV * (Axis1.Dot(tempV)));
		Axis1.Normalize();
		Axis2 = Axis1.CrossProduct(tempV);

		temp = Axis1.Z;
		Axis1.Z = Axis1.Y;
		Axis1.Y = temp;

		temp = Axis2.Z;
		Axis2.Z = Axis2.Y;
		Axis2.Y = temp;
	}

	TVector2DTemp<T> XY() const
	{
		return TVector2DTemp<T>(X, Y);
	}

	union
	{
		struct
		{
			T X, Y, Z;
		};
		struct
		{
			T x, y, z;
		};
		struct
		{
			T R, G, B;
		};
		struct
		{
			T Value[3];
		};
	};
};

typedef TVector3DTemp<float> kVector3D;
typedef TRotationTemp<float> kRotation;


