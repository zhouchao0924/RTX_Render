
#pragma once


#include "kVector3D.h"
#include "kMatrix43.h"
#include "assert.h"

template <class T>
class TXformTemp
{
public:
	TXformTemp()
		:Scale(1.0f)
	{
	}
	TXformTemp(const TVector3DTemp<T> &InLocation, const TRotationTemp<T> &InRotation, const TVector3DTemp<T> &InScale)
		: Location(InLocation)
		, Rotation(InRotation)
		, Scale(InScale)
	{
	}
	
	TMatrix43Temp<T> GetMatrix()
	{
		return TMatrix43Temp<T>(Location, Rotation.ToRadian(), Scale);
	}

	TMatrix43Temp<T> GetMatrix_ZUp()
	{
		TMatrix43Temp<T> rotMat;
		TMatrix43Temp<T> scaleMat;
		
		scaleMat.SetScale(Scale);
		rotMat.SetRotator_ZUp(Rotation.ToRadian());

		TMatrix43Temp<T> mat = scaleMat*rotMat;
		mat.AddTranslation(Location);

		return mat;
	}

	TVector3DTemp<T> Location;
	TRotationTemp<T> Rotation;
	TVector3DTemp<T> Scale;
};

typedef TXformTemp<float> kXform;





