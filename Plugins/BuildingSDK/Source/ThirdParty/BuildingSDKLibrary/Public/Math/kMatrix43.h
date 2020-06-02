

#pragma once

#include "kQuat.h"
#include "kSphere.h"
#include "kCapsule.h"
#include "kPlane.h"
#include "kBox.h"
#include "kVector3D.h"
#include "assert.h"

template <class T>
class TMatrix43Temp
{
public:

	TMatrix43Temp()
	{
		MakeIdentity();
	}

	TMatrix43Temp(
		T i00, T i10, T i20,
		T i01, T i11, T i21,
		T i02, T i12, T i22,
		T i03, T i13, T i23) :
		m00(i00), m10(i01), m20(i02), m30(i03),
		m01(i10), m11(i11), m21(i12), m31(i13),
		m02(i20), m12(i21), m22(i22), m32(i23)
	{
	}


	//! Simple operator for directly accessing every element of the matrix.
	T& operator()(int row, int col) { return M[col * 4 + row]; }

	//! Simple operator for directly accessing every element of the matrix.
	const T& operator()(int row, int col) const { return M[col * 4 + row]; }

	//! Sets this matrix equal to the other matrix.
	TMatrix43Temp<T>& operator=(const TMatrix43Temp<T> &other)
	{
		for (int i = 0; i < 12; ++i)
			M[i] = other.M[i];
		return *this;
	}

	TMatrix43Temp<T> & Set(
		T v00, T v01, T v02,
		T v10, T v11, T v12,
		T v20, T v21, T v22,
		T v30, T v31, T v32)
	{
		m00 = v00; m01 = v01; m02 = v02;
		m10 = v10; m11 = v11; m12 = v12;
		m20 = v20; m21 = v21; m22 = v22;
		m30 = v30; m31 = v31; m32 = v32;
		return *this;
	}

	TMatrix43Temp<T>(const TVector3DTemp<T> &Location, const TRotationTemp<T> &Rotation, const TVector3DTemp<T> &Scale = TVector3DTemp<T>(1.0f))
	{
		SetTransform(Scale, TVector3DTemp<T>(Rotation.Pitch*K_G_RAD, Rotation.Yaw*K_G_RAD, Rotation.Roll*K_G_RAD), Location);
	}
	
	TMatrix43Temp<T> & Set(TVector3DTemp<T> &xaxis, TVector3DTemp<T> &yaxis, TVector3DTemp<T> &zaxis, TVector3DTemp<T> &o)
	{
		memcpy(M, &xaxis, sizeof(xaxis));
		memcpy(M + 3, &yaxis, sizeof(yaxis));
		memcpy(M + 6, &zaxis, sizeof(zaxis));
		memcpy(M + 9, &o, sizeof(o));
		return *this;
	}

	void  Zero()
	{
		memset(M, 0, sizeof(M));
	}

	TMatrix43Temp<T> & MakeIdentity()
	{
		memset(M, 0, sizeof(M));
		m00 = m11 = m22 = 1.0f;
		return *this;
	}

	TMatrix43Temp<T> & SetScale(T scaleX, T scaleY, T scaleZ)
	{
		Set(scaleX, 0.f, 0.f,
			0.f, scaleY, 0.f,
			0.f, 0.f, scaleZ,
			0.f, 0.f, 0.f);
		return *this;
	}

	TMatrix43Temp<T> & SetScale(const TVector3DTemp<T>& scale)
	{
		SetScale(scale.X, scale.Y, scale.Z);
		return *this;
	}

	TMatrix43Temp<T> & AddScale(T scaleX, T scaleY, T scaleZ)
	{
		TMatrix43Temp<T> matrix;
		matrix.SetScale(scaleX, scaleY, scaleZ);
		(*this) *= matrix;
		return *this;
	}

	TMatrix43Temp<T> & AddScale(const TVector3DTemp<T>& scale)
	{
		TMatrix43Temp<T> matrix;
		matrix.SetScale(scale.X, scale.Y, scale.Z);
		(*this) *= matrix;
		return *this;
	}

	TVector3DTemp<T> GetScale() const
	{
		TVector3DTemp<T>t;
		t.X = (T)(((TVector3DTemp<T>*)&m00)->Size());
		t.Y = (T)(((TVector3DTemp<T>*)&m10)->Size());
		t.Z = (T)(((TVector3DTemp<T>*)&m20)->Size());
		return t;
	}

	T GetScaleX() const
	{
		return (T)(((TVector3DTemp<T>*)&m00)->Size());
	}

	TMatrix43Temp<T> & RemoveScale()
	{
		TMatrix43Temp<T> t;
		t.SetScale(((T)1) / (T)(((TVector3DTemp<T>*)&m00)->Size()),
			((T)1) / (T)(((TVector3DTemp<T>*)&m10)->Size()),
			((T)1) / (T)(((TVector3DTemp<T>*)&m20)->Size()));
		(*this) = t*(*this);
		return *this;
	}

	TMatrix43Temp<T> & SetRotationY(T radian)
	{
		T sinv = sin(radian);
		T cosv = cos(radian);
		Set(cosv, 0.f, -sinv,
			0.f, 1.f, 0.f,
			sinv, 0.f, cosv,
			0.f, 0.f, 0.f);
		return *this;
	}

	TMatrix43Temp<T> & AddRotationY(T radian)
	{
		TMatrix43Temp<T> matrix;
		matrix.SetRotationY(radian);
		(*this) *= matrix;
		return *this;
	}

	TMatrix43Temp<T> & SetRotationZ(T radian)
	{
		T sinv = sin(radian);
		T cosv = cos(radian);
		Set(cosv, sinv, 0.f,
			-sinv, cosv, 0.f,
			0.f, 0.f, 1.f,
			0.f, 0.f, 0.f);
		return *this;
	}

	TMatrix43Temp<T> & AddRotationZ(T radian)
	{
		TMatrix43Temp<T> matrix;
		matrix.SetRotationZ(radian);
		(*this) *= matrix;
		return *this;
	}

	//axis is required normalized
	TMatrix43Temp<T> & SetRotationAxis(const TVector3DTemp<T>& axis, T radian)
	{
		T sinv = sin(radian);
		T cosv = cos(radian);
		T invCos = 1.f - cosv;
		T xyInv = axis.X * axis.Y * invCos;
		T xzInv = axis.X * axis.Z * invCos;
		T yzInv = axis.Y * axis.Z * invCos;
		T xSin = axis.X * sinv;
		T ySin = axis.Y * sinv;
		T zSin = axis.Z * sinv;
		Set((axis.X * axis.X * invCos + cosv), (xyInv - zSin), (xzInv + ySin),
			(xyInv + zSin), (axis.Y * axis.Y * invCos + cosv), (yzInv - xSin),
			(xzInv - ySin), (yzInv + xSin), (axis.Z * axis.Z * invCos + cosv),
			0.0f, 0.0f, 0.0f
		);
		return *this;
	}

	TMatrix43Temp<T> & AddRotationAxis(const TVector3DTemp<T>& axis, T radian)
	{
		TMatrix43Temp<T> matrix;
		matrix.SetRotationAxis(axis, radian);
		(*this) *= matrix;
		return *this;
	}

	void GetRotationAxis(TVector3DTemp<T>* axis, T* radian) const
	{
		T radianResult = acos(0.5f * ((m00 + m11 + m22) - 1.f));
		*radian = radianResult;
		if (radianResult > 0.f)
		{
			if (radianResult < 3.141592653589793f)
			{
				axis->Set(m12 - m21, m20 - m02, m01 - m10);
				axis->Normalize();
			}
			else
			{
				if (m00 >= m11)
				{
					if (m00 >= m22)
					{
						axis->X = 0.5f * sqrt(m00 - m11 - m22 + 1.f);
						T halfInverse = 0.5f / axis->X;
						axis->Y = halfInverse * m10;
						axis->Z = halfInverse * m20;
					}
					else
					{
						axis->Z = 0.5f * sqrt(m22 - m00 - m11 + 1.f);
						T halfInverse = 0.5f / axis->Z;
						axis->X = halfInverse * m20;
						axis->Y = halfInverse * m21;
					}
				}
				else
				{
					if (m11 >= m22)
					{
						axis->Y = 0.5f * sqrt(m11 - m00 - m22 + 1.f);
						T halfInverse = 0.5f / axis->Y;
						axis->X = halfInverse * m10;
						axis->Z = halfInverse * m21;
					}
					else
					{
						axis->Z = 0.5f * sqrt(m22 - m00 - m11 + 1.f);
						T halfInverse = 0.5f / axis->Z;
						axis->X = halfInverse * m20;
						axis->Y = halfInverse * m21;
					}
				}
			}
		}
		else
		{
			axis->set(1.f, 0.f, 0.f);
		}
	}

	//q should be normalized
	TMatrix43Temp<T> & FromQuaternion(const TQuatTemplate<T>& q)
	{
		T x2 = q.X + q.X;
		T y2 = q.Y + q.Y;
		T z2 = q.Z + q.Z;
		T xx2 = q.X * x2;
		T xy2 = q.X * y2;
		T xz2 = q.X * z2;
		T yy2 = q.Y * y2;
		T yz2 = q.Y * z2;
		T zz2 = q.Z * z2;
		T wx2 = q.W * x2;
		T wy2 = q.W * y2;
		T wz2 = q.W * z2;
		m00 = 1.f - (yy2 + zz2);
		m01 = xy2 + wz2;
		m02 = xz2 - wy2;

		m10 = xy2 - wz2;
		m11 = 1.f - (xx2 + zz2);
		m12 = yz2 + wx2;

		m20 = xz2 + wy2;
		m21 = yz2 - wx2;
		m22 = 1.f - (xx2 + yy2);

		m30 = m31 = m32 = 0.f;
		return *this;
	}

	TMatrix43Temp<T> & AddQuaternion(const TQuatTemplate<T>& q)
	{
		TMatrix43Temp<T> matrix;
		matrix.SetQuaternion(q);
		(*this) *= matrix;
		return *this;
	}

	TMatrix43Temp<T> & SetQuaternion(const TQuatTemplate<T>& q)
	{
		T x2 = q.X + q.X;
		T y2 = q.Y + q.Y;
		T z2 = q.Z + q.Z;
		T xx2 = q.X * x2;
		T xy2 = q.X * y2;
		T xz2 = q.X * z2;
		T yy2 = q.Y * y2;
		T yz2 = q.Y * z2;
		T zz2 = q.Z * z2;
		T wx2 = q.W * x2;
		T wy2 = q.W * y2;
		T wz2 = q.W * z2;
		m00 = 1.f - (yy2 + zz2);
		m01 = xy2 + wz2;
		m02 = xz2 - wy2;

		m10 = xy2 - wz2;
		m11 = 1.f - (xx2 + zz2);
		m12 = yz2 + wx2;

		m20 = xz2 + wy2;
		m21 = yz2 - wx2;
		m22 = 1.f - (xx2 + yy2);

		m30 = m31 = m32 = 0.f;

		return *this;
	}

	TQuatTemplate<T> GetQuaternion() const
	{
		TQuatTemplate<T> q;

		float trace = m00 + m11 + m22 + 1.0f;
		if (trace > 0.0f)
		{
			float s = 0.5f / sqrt(trace);
			q.W = 0.25f / s;
			q.X = (m12 - m21) * s;
			q.Y = (m20 - m02) * s;
			q.Z = (m01 - m10) * s;
		}
		else
		{
			if (m00 > m11 && m00 > m22) {
				float s = 2.0f * sqrt(1.0f + m00 - m11 - m22);
				q.W = (m21 - m12) / s;
				q.X = 0.25f * s;
				q.Y = (m10 + m01) / s;
				q.Z = (m20 + m02) / s;
			}
			else if (m11 > m22) {
				float s = 2.0f * sqrt(1.0f + m11 - m00 - m22);
				q.W = (m20 - m02) / s;
				q.X = (m10 + m01) / s;
				q.Y = 0.25f * s;
				q.Z = (m21 + m12) / s;
			}
			else {
				float s = 2.0f * sqrt(1.0f + m22 - m00 - m11);
				q.W = (m10 - m01) / s;
				q.X = (m20 + m02) / s;
				q.Y = (m21 + m12) / s;
				q.Z = 0.25f * s;
			}
		}

		return q;
	}

	TMatrix43Temp<T> & SetRotationXYZ(const TVector3DTemp<T>& radian)
	{
		T sinX = sin(radian.X);
		T cosX = cos(radian.X);
		T sinY = sin(radian.Y);
		T cosY = cos(radian.Y);
		T sinZ = sin(radian.Z);
		T cosZ = cos(radian.Z);
		m00 = cosY * cosZ;
		m01 = cosY * sinZ;
		m02 = -sinY;
		m10 = sinX * sinY * cosZ - cosX * sinZ;
		m11 = sinX * sinY * sinZ + cosX * cosZ;
		m12 = sinX * cosY;
		m20 = cosX * sinY * cosZ + sinX * sinZ;
		m21 = cosX * sinY * sinZ - sinX * cosZ;
		m22 = cosX * cosY;
		m30 = 0.f;
		m31 = 0.f;
		m32 = 0.f;
		return *this;
	}

	TMatrix43Temp<T> & AddRotationXYZ(const TVector3DTemp<T>& radian)
	{
		TMatrix43Temp<T> matrix;
		matrix.SetRotationXYZ(radian);
		(*this) *= matrix;
		return *this;
	}

	TVector3DTemp<T>  GetRotationXYZ() const
	{
		TVector3DTemp<T> radian;
		T yRadian = asin(-m02);
		radian.Y = yRadian;
		if (yRadian < 1.57079632679489661923f)
		{
			if (yRadian > -1.57079632679489661923f)
			{
				radian.X = atan2(m12, m22);
				radian.Z = atan2(m01, m00);
				return radian;
			}
			else
			{
				radian.X = -atan2(m10, m11);
				radian.Z = 0.f;
				return kVector3D(0);
			}
		}
		else
		{
			radian.X = atan2(m10, m11);
			radian.Z = 0.f;
			return kVector3D(0);
		}
	}

	TVector3DTemp<T>& GetRow(int iRow) const
	{
		return *((TVector3DTemp<T>*)(&m00 + iRow * 3));
	}

	TVector3DTemp<T> GetOrigin() const
	{
		return TVector3DTemp<T>(m30, m31, m32);
	}

	TMatrix43Temp<T> & SetOrigin(T translationX, T translationY, T translationZ)
	{
		Set(1.f, 0.f, 0.f,
			0.f, 1.f, 0.f,
			0.f, 0.f, 1.f,
			translationX, translationY, translationZ);
		return *this;
	}

	TMatrix43Temp<T> & SetOrigin(const TVector3DTemp<T>& translation)
	{
		SetOrigin(translation.X, translation.Y, translation.Z);
		return *this;
	}

	TMatrix43Temp<T> & AddTranslation(T translationX, T translationY, T translationZ)
	{
		m30 += translationX;
		m31 += translationY;
		m32 += translationZ;
		return *this;
	}

	TMatrix43Temp<T> & AddTranslation(const TVector3DTemp<T>& translation)
	{
		m30 += translation.X;
		m31 += translation.Y;
		m32 += translation.Z;
		return *this;
	}

	TMatrix43Temp<T> & SetTransform(const TVector3DTemp<T>& scale, const TVector3DTemp<T>& radian, const TVector3DTemp<T>& translation)
	{
		T sinX = sin(radian.X);
		T cosX = cos(radian.X);
		T sinY = sin(radian.Y);
		T cosY = cos(radian.Y);
		T sinZ = sin(radian.Z);
		T cosZ = cos(radian.Z);
		m00 = scale.X * (cosY * cosZ);
		m01 = scale.X * (cosY * sinZ);
		m02 = scale.X * (-sinY);
		m10 = scale.Y * (sinX * sinY * cosZ - cosX * sinZ);
		m11 = scale.Y * (sinX * sinY * sinZ + cosX * cosZ);
		m12 = scale.Y * (sinX * cosY);
		m20 = scale.Z * (cosX * sinY * cosZ + sinX * sinZ);
		m21 = scale.Z * (cosX * sinY * sinZ - sinX * cosZ);
		m22 = scale.Z * (cosX * cosY);
		m30 = translation.X;
		m31 = translation.Y;
		m32 = translation.Z;
		return *this;
	}

	//q should be normilized
	TMatrix43Temp<T> & SetTransform(const TVector3DTemp<T>& scale, const TQuatTemplate<T>& q, const TVector3DTemp<T>& translation)
	{
		T x2 = q.X + q.X;
		T y2 = q.Y + q.Y;
		T z2 = q.Z + q.Z;
		T xx2 = q.X * x2;
		T xy2 = q.X * y2;
		T xz2 = q.X * z2;
		T yy2 = q.Y * y2;
		T yz2 = q.Y * z2;
		T zz2 = q.Z * z2;
		T wx2 = q.W * x2;
		T wy2 = q.W * y2;
		T wz2 = q.W * z2;
		m00 = scale.X * (1.f - (yy2 + zz2));
		m01 = scale.X * (xy2 + wz2);
		m02 = scale.X * (xz2 - wy2);
		m10 = scale.Y * (xy2 - wz2);
		m11 = scale.Y * (1.f - (xx2 + zz2));
		m12 = scale.Y * (yz2 + wx2);
		m20 = scale.Z * (xz2 + wy2);
		m21 = scale.Z * (yz2 - wx2);
		m22 = scale.Z * (1.f - (xx2 + yy2));
		m30 = translation.X;
		m31 = translation.Y;
		m32 = translation.Z;
		return *this;
	}

	TMatrix43Temp<T> & AddTransform(const TVector3DTemp<T>& scale, const TQuatTemplate<T>& q, const TVector3DTemp<T>& translation)
	{
		TMatrix43Temp<T> matrix;
		matrix.SetTransform(scale, q, translation);
		(*this) *= matrix;
		return *this;
	}


	TMatrix43Temp<T> & BuildCameraLookAtMatrixLH(const TVector3DTemp<T> &xaxis,
		const TVector3DTemp<T> &yaxis, const TVector3DTemp<T> &zaxis,
		const TVector3DTemp<T> &o)
	{
		m00 = xaxis.X;
		m01 = yaxis.X;
		m02 = zaxis.X;

		m10 = xaxis.Y;
		m11 = yaxis.Y;
		m12 = zaxis.Y;

		m20 = xaxis.Z;
		m21 = yaxis.Z;
		m22 = zaxis.Z;

		m30 = -xaxis.Dot(o);
		m31 = -yaxis.Dot(o);
		m32 = -zaxis.Dot(o);
		return *this;
	}


	TMatrix43Temp operator *(const TMatrix43Temp& mtx) const
	{
		return TMatrix43Temp(
			(m00 * mtx.m00) + (m01 * mtx.m10) + (m02 * mtx.m20),
			(m00 * mtx.m01) + (m01 * mtx.m11) + (m02 * mtx.m21),
			(m00 * mtx.m02) + (m01 * mtx.m12) + (m02 * mtx.m22),
			(m10 * mtx.m00) + (m11 * mtx.m10) + (m12 * mtx.m20),
			(m10 * mtx.m01) + (m11 * mtx.m11) + (m12 * mtx.m21),
			(m10 * mtx.m02) + (m11 * mtx.m12) + (m12 * mtx.m22),
			(m20 * mtx.m00) + (m21 * mtx.m10) + (m22 * mtx.m20),
			(m20 * mtx.m01) + (m21 * mtx.m11) + (m22 * mtx.m21),
			(m20 * mtx.m02) + (m21 * mtx.m12) + (m22 * mtx.m22),
			(m30 * mtx.m00) + (m31 * mtx.m10) + (m32 * mtx.m20) + mtx.m30,
			(m30 * mtx.m01) + (m31 * mtx.m11) + (m32 * mtx.m21) + mtx.m31,
			(m30 * mtx.m02) + (m31 * mtx.m12) + (m32 * mtx.m22) + mtx.m32);
	}

	TMatrix43Temp& operator *=(TMatrix43Temp mtx)
	{
		T old00 = m00;
		T old10 = m01;
		m00 = (old00 * mtx.m00) + (old10 * mtx.m10) + (m02 * mtx.m20);
		m01 = (old00 * mtx.m01) + (old10 * mtx.m11) + (m02 * mtx.m21);
		m02 = (old00 * mtx.m02) + (old10 * mtx.m12) + (m02 * mtx.m22);
		T old01 = m10;
		T old11 = m11;
		m10 = (old01 * mtx.m00) + (old11 * mtx.m10) + (m12 * mtx.m20);
		m11 = (old01 * mtx.m01) + (old11 * mtx.m11) + (m12 * mtx.m21);
		m12 = (old01 * mtx.m02) + (old11 * mtx.m12) + (m12 * mtx.m22);
		T old02 = m20;
		T old12 = m21;
		m20 = (old02 * mtx.m00) + (old12 * mtx.m10) + (m22 * mtx.m20);
		m21 = (old02 * mtx.m01) + (old12 * mtx.m11) + (m22 * mtx.m21);
		m22 = (old02 * mtx.m02) + (old12 * mtx.m12) + (m22 * mtx.m22);
		T old03 = m30;
		T old13 = m31;
		m30 = (old03 * mtx.m00) + (old13 * mtx.m10) + (m32 * mtx.m20) + mtx.m30;
		m31 = (old03 * mtx.m01) + (old13 * mtx.m11) + (m32 * mtx.m21) + mtx.m31;
		m32 = (old03 * mtx.m02) + (old13 * mtx.m12) + (m32 * mtx.m22) + mtx.m32;
		return *this;
	}

	void TransformVect(const TVector3DTemp<T>& in, TVector3DTemp<T> &out) const
	{
		out.Set(
			in.X * m00 + in.Y * m10 + in.Z * m20 + m30,
			in.X * m01 + in.Y * m11 + in.Z * m21 + m31,
			in.X * m02 + in.Y * m12 + in.Z * m22 + m32);
	}

	TVector3DTemp<T> TransformVect(const TVector3DTemp<T>& in) const
	{
		TVector3DTemp<T> out;
		TransformVect(in, out);
		return out;
	}

	float RotDeterminant() const
	{
		return
			m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
			m[1][0] * (m[0][1] * m[2][2] - m[0][2] * m[2][1]) +
			m[2][0] * (m[0][1] * m[1][2] - m[0][2] * m[1][1]);
	}

	TMatrix43Temp<T> & ScaleTranslation(const TVector3DTemp<T>& Scale3D)
	{
		m[3][0] *= Scale3D.X;
		m[3][1] *= Scale3D.Y;
		m[3][2] *= Scale3D.Z;
		return *this;
	}

	/** Faster version of InverseTransformFVector that assumes no scaling. WARNING: Will NOT work correctly if there is scaling in the matrix. */
	TVector3DTemp<T> InverseTransformFVectorNoScale(const TVector3DTemp<T> &V) const
	{
		// Check no scaling in matrix
		check(rsAbs(1.f - rsAbs(RotDeterminant())) < 0.01f);

		kVector3D t, Result;

		t.X = V.X - m[3][0];
		t.Y = V.Y - m[3][1];
		t.Z = V.Z - m[3][2];

		Result.X = t.X * m[0][0] + t.Y * m[0][1] + t.Z * m[0][2];
		Result.Y = t.X * m[1][0] + t.Y * m[1][1] + t.Z * m[1][2];
		Result.Z = t.X * m[2][0] + t.Y * m[2][1] + t.Z * m[2][2];

		return Result;
	}

	TVector3DTemp<T> InverseTransformNormalNoScale(const TVector3DTemp<T> &V) const
	{
		// Check no scaling in matrix
		check(rsAbs(1.f - rsAbs(RotDeterminant())) < 0.01f);
		return TVector3DTemp<T>(V.X * m[0][0] + V.Y * m[0][1] + V.Z * m[0][2],
			V.X * m[1][0] + V.Y * m[1][1] + V.Z * m[1][2],
			V.X * m[2][0] + V.Y * m[2][1] + V.Z * m[2][2]);
	}


	//using 3x3 part
	void TransformNormal(const TVector3DTemp<T>& in, TVector3DTemp<T>& out) const
	{
		return  out.Set(
			in.X * m00 + in.Y * m10 + in.Z * m20,
			in.X * m01 + in.Y * m11 + in.Z * m21,
			in.X * m02 + in.Y * m12 + in.Z * m22);
	}

	TVector3DTemp<T> TransformNormal(const TVector3DTemp<T>& in) const
	{
		TVector3DTemp<T> out;
		TransformNormal(in, out);
		return out;
	}

	void TransformSphere(const TSphereTemp<T>&in, TSphereTemp<T>&out) const
	{
		TransformVect(in.center, out.center);

		float scale = (T)(((TVector3DTemp<T>*)&m00)->Size());
		out.radius = in.radius*scale;
	}

	void TransformCapsule(const TCapsuleTemp<T>&in, TCapsuleTemp<T>&out) const
	{
		TransformVect(in.start, out.start);
		TransformVect(in.end, out.end);

		float scale = (T)(((TVector3DTemp<T>*)&m00)->Size());
		out.radius = in.radius*scale;
	}


	//! Transforms a plane by this matrix
	void TransformPlane(TPlaneTemp<T> &plane) const
	{
		TVector3DTemp<T> member;
		TransformVect(plane.GetMemberPoint(), member);

		TVector3DTemp<T> nNew;
		TransformNormal(plane.Normal, nNew);

		plane.Normal = nNew;
		plane.D = member.Dot(nNew);
	}

	//! Transforms a plane by this matrix
	void TransformPlane(const TPlaneTemp<T> &in, TPlaneTemp<T> &out) const
	{
		out = in;
		TransformPlane(out);
	}

	TBoxTemp<T> TransformBoxEx(const TBoxTemp<T>& box) const
	{
		TBoxTemp<T> out;

		TVector3DTemp<T> corners[8], cornersT[8];
		box.GetCorners(corners);

		int i;
		for (i = 0; i < 8; ++i)
			TransformVect(corners[i], cornersT[i]);

		out.Set(cornersT[0]);

		for (i = 1; i < 8; ++i)
			out.Add(cornersT[i]);

		return out;
	}

	void TransformBoxInPlace(TBoxTemp<T>& box) const
	{
		TVector3DTemp<T> corners[8], cornersT[8];
		box.GetCorners(corners);

		int i;
		for (i = 0; i < 8; ++i)
			TransformVect(corners[i], cornersT[i]);

		box.Set(cornersT[0]);

		for (i = 1; i < 8; ++i)
			box.Add(cornersT[i]);
	}

	void TransformLine(TLineTemp<T>&line)const
	{
		TransformVect(line.start);
		TransformVect(line.end);
	}

	void TransformLine(TLineTemp<T>&in, TLineTemp<T>&out)const
	{
		TransformVect(in.start, out.start);
		TransformVect(in.end, out.end);
	}

	TMatrix43Temp operator *(T value) const
	{
		return TMatrix43Temp(
			m00 * value, m01 * value, m02 * value,
			m10 * value, m11 * value, m12 * value,
			m20 * value, m21 * value, m22 * value,
			m30 * value, m31 * value, m32 * value);
	}

	TMatrix43Temp& operator *=(T value)
	{
		m00 *= value;
		m01 *= value;
		m02 *= value;
		m10 *= value;
		m11 *= value;
		m12 *= value;
		m20 *= value;
		m21 *= value;
		m22 *= value;
		m30 *= value;
		m31 *= value;
		m32 *= value;
		return *this;
	}

	TMatrix43Temp<T> & Transpose()
	{
		T swap;
		swap = m01; m01 = m10; m10 = swap;
		swap = m02; m02 = m20; m20 = swap;
		swap = m12; m12 = m21; m21 = swap;
		m30 = m31 = m32 = 0.f;
		return *this;
	}

	TMatrix43Temp<T> TransposeAdjoint() const
	{
		TMatrix43Temp<T> TA;

		TA.m[0][0] = m[1][1] * m[2][2] - m[1][2] * m[2][1];
		TA.m[0][1] = m[1][2] * m[2][0] - m[1][0] * m[2][2];
		TA.m[0][2] = m[1][0] * m[2][1] - m[1][1] * m[2][0];

		TA.m[1][0] = m[2][1] * m[0][2] - m[2][2] * m[0][1];
		TA.m[1][1] = m[2][2] * m[0][0] - m[2][0] * m[0][2];
		TA.m[1][2] = m[2][0] * m[0][1] - m[2][1] * m[0][0];

		TA.m[2][0] = m[0][1] * m[1][2] - m[0][2] * m[1][1];
		TA.m[2][1] = m[0][2] * m[1][0] - m[0][0] * m[1][2];
		TA.m[2][2] = m[0][0] * m[1][1] - m[0][1] * m[1][0];

		TA.m[3][0] = 0.f;
		TA.m[3][1] = 0.f;
		TA.m[3][2] = 0.f;

		return TA;
	}


	T Determinant() const
	{
		return
			m00 * (m11 * m22 - m12 * m21) -
			m01 * (m10 * m22 - m12 * m20) +
			m02 * (m10 * m21 - m11 * m20);
	}

	bool MakeInverse()
	{
		TMatrix43Temp<T> temp;

		if (GetInverse(&temp))
		{
			*this = temp;
			return true;
		}

		return false;
	}

	bool GetInverse(TMatrix43Temp* invertMatrix) const
	{
		invertMatrix->m00 = m11 * m22 - m12 * m21;
		invertMatrix->m10 = -(m10 * m22 - m12 * m20);
		invertMatrix->m20 = m10 * m21 - m11 * m20;
		T determ =
			m00 * invertMatrix->m00 +
			m01 * invertMatrix->m10 +
			m02 * invertMatrix->m20;

		if (determ==0)
			return false;

		invertMatrix->m01 = -(m01 * m22 - m02 * m21);
		invertMatrix->m02 = m01 * m12 - m02 * m11;
		invertMatrix->m11 = m00 * m22 - m02 * m20;
		invertMatrix->m12 = -(m00 * m12 - m02 * m10);
		invertMatrix->m21 = -(m00 * m21 - m01 * m20);
		invertMatrix->m22 = m00 * m11 - m01 * m10;
		invertMatrix->m30 = -(
			m10 * (m21 * m32 - m31 * m22) -
			m11 * (m20 * m32 - m30 * m22) +
			m12 * (m20 * m31 - m30 * m21));
		invertMatrix->m31 =
			m00 * (m21 * m32 - m31 * m22) -
			m01 * (m20 * m32 - m30 * m22) +
			m02 * (m20 * m31 - m30 * m21);
		invertMatrix->m32 = -(
			m00 * (m11 * m32 - m31 * m12) -
			m01 * (m10 * m32 - m30 * m12) +
			m02 * (m10 * m31 - m30 * m11));
		T invDeterm = 1.f / determ;
		(*invertMatrix) *= invDeterm;
		return false;
	}

	bool operator ==(const TMatrix43Temp& target) const
	{
		return (
			(m00 == target.m00) && (m10 == target.m10) && (m20 == target.m20) &&
			(m01 == target.m01) && (m11 == target.m11) && (m21 == target.m21) &&
			(m02 == target.m02) && (m12 == target.m12) && (m22 == target.m22) &&
			(m30 == target.m30) && (m31 == target.m31) && (m32 == target.m32));
	}

	bool Equals(const TMatrix43Temp& target) const
	{
		return
			(Equals_Float(m00, target.m00) &&
				Equals_Float(m01, target.m01) &&
				Equals_Float(m02, target.m02) &&
				Equals_Float(m30, target.m30) &&
				Equals_Float(m10, target.m10) &&
				Equals_Float(m11, target.m11) &&
				Equals_Float(m12, target.m12) &&
				Equals_Float(m31, target.m31) &&
				Equals_Float(m20, target.m20) &&
				Equals_Float(m21, target.m21) &&
				Equals_Float(m22, target.m22) &&
				Equals_Float(m32, target.m32));
	}

	bool IsIdentity() const
	{
		return
			(Equals_Float(m00, 1) &&
				Equals_Float(m01, 0) &&
				Equals_Float(m02, 0) &&
				Equals_Float(m10, 0) &&
				Equals_Float(m11, 1) &&
				Equals_Float(m12, 0) &&
				Equals_Float(m20, 0) &&
				Equals_Float(m21, 0) &&
				Equals_Float(m22, 1) &&
				Equals_Float(m30, 0) &&
				Equals_Float(m31, 0) &&
				Equals_Float(m32, 0));
	}


	bool operator !=(const TMatrix43Temp& target) const
	{
		return (
			(m00 != target.m00) || (m10 != target.m10) || (m20 != target.m20) ||
			(m01 != target.m01) || (m11 != target.m11) || (m21 != target.m21) ||
			(m02 != target.m02) || (m12 != target.m12) || (m22 != target.m22) ||
			(m30 != target.m30) || (m31 != target.m31) || (m32 != target.m32));
	}

	kVector3D GetAxis(int Index) const
	{
		assert(Index < 3);
		return kVector3D(M[Index], M[3 + Index], M[6 + Index]);
	}

	void SetRotator_ZUp(const TRotationTemp<T> &ration)
	{
		SetRotationXYZ(TVector3DTemp<T>(ration.Roll, ration.Pitch, ration.Yaw));
	}

	void SetRotator_YUp(const TRotationTemp<T> &ration)
	{
		SetRotationXYZ(TVector3DTemp<T>(ration.Pitch, ration.Yaw, ration.Roll));
	}

	union
	{
		struct
		{
			T m00; T m01; T m02;
			T m10; T m11; T m12;
			T m20; T m21; T m22;
			T m30; T m31; T m32;
		};
		T m[4][3];
		T M[12];
	};

	// 	FString &ToString() const
	// 	{
	// 		static FString strMat;
	// 		TCHAR buffer[256];
	// 		wsprintf(buffer,
	// 			"{\n  ( %.8f, %.8f, %.8f )\n  ( %.8f, %.8f, %.8f )\n"
	// 			"  ( %.8f, %.8f, %.8f )\n  ( %.8f, %.8f, %.8f )\n}",
	// 			m00, m10, m20, m01, m11, m21, m02, m12, m22, m03, m13, m23);
	// 		strMat = buffer;
	// 		return strMat;
	// 	}
	// 
	// 	static const TMatrix43<T> &Identity()
	// 	{
	// 		static TMatrix43<T> t;
	// 		return t;
	// 	}
};

typedef TMatrix43Temp<float> kMatrix43;



