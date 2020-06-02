
#pragma once

#include "kMathBase.h"
#include "kVector3D.h"
#include "kVector4D.h"

//! Enumeration for intersection relations of 3d objects
enum kEIntersectionRelation3D
{
	kISREL3D_FRONT = 0,
	kISREL3D_BACK,
	kISREL3D_PLANAR,
	kISREL3D_SPANNING,
	kISREL3D_CLIPPED
};

//! Template TPlane class with some intersection testing methods.
template <class T>
class TPlaneTemp
{
public:

	// Constructors

	TPlaneTemp() : Normal(0, 1, 0) { RecalculateD(TVector3DTemp<T>(0, 0, 0)); };
	TPlaneTemp(T px, T py, T pz, T nx, T ny, T nz) : Normal(nx, ny, nz) { RecalculateD(TVector3DTemp<T>(px, py, pz)); };
	TPlaneTemp(const TPlaneTemp<T>& other) :D(other.D), Normal(other.Normal) {};
	TPlaneTemp(const TVector3DTemp<T>& point1, const TVector3DTemp<T>& point2, const TVector3DTemp<T>& point3) { Set(point1, point2, point3); };

	TPlaneTemp(const TVector3DTemp<T>& MPoint, const TVector3DTemp<T>& Normal) : Normal(Normal) { RecalculateD(MPoint); };
	TPlaneTemp(T nx, T ny, T nz, T d) : Normal(nx, ny, nz), D(d) { };
	TPlaneTemp(const TVector3DTemp<T>& normal, T d) : Normal(normal), D(d) { };

	// operators
	inline bool operator==(const TPlaneTemp<T>& other) const { return (D == other.D && Normal == other.Normal); };
	inline bool operator!=(const TPlaneTemp<T>& other) const { return !(D == other.D && Normal == other.Normal); };

	inline TPlaneTemp<T>& operator+=(const TVector3DTemp<T>& point)
	{
		RecalculateD(GetMemberPoint() + point);
		return *this;
	}

	// functions

	void Set(const TVector3DTemp<T>& point, const TVector3DTemp<T>& nvector)
	{
		Normal = nvector;
		Normal.Normalize();
		RecalculateD(point);
	}

	void Set(const TVector3DTemp<T>& nvect, T d)
	{
		Normal = nvect;
		D = d;
	}

	void Set(const TVector3DTemp<T>& point1, const TVector3DTemp<T>& point2, const TVector3DTemp<T>& point3)
	{
		// creates the TPlane from 3 memberpoints
		Normal = (point2 - point1).CrossProduct(point3 - point1);
		Normal.Normalize();
		RecalculateD(point1);
	}

	//! Returns an intersection with a 3d line.
	//! \param lineVect: Vector of the line to intersect with, normalized
	//! \param linePoint: Point of the line to intersect with.
	//! \param outIntersection: Place to store the intersection point, if there is one.
	//! \return Returns true if there was an intersection, false if there was not.
	bool GetIntersectionWithLine(const TVector3DTemp<T>& linePoint, const TVector3DTemp<T>& lineVect,
		TVector3DTemp<T>& outIntersection) const
	{
		T t2 = Normal.Dot(lineVect);
		if ((t2 >= (T)-0.001f) && (t2 <= (T)0.001f))
			return false;//Almost parallel
		T t = -(Normal.Dot(linePoint) - D) / t2;
		//T t =- (Normal.dotProduct(linePoint) + D) / t2;			
		outIntersection = linePoint + (lineVect * t);
		return true;
	}

	//! Clip the part on the line that is at the backside of this Plane
	//! \param lineVect: Vector of the line to intersect with.
	//! \param linePoint: Point of the line to intersect with.
	//! \param tIntersection: the rate of intersection point on the given line, if there is one.
	//! \param bClipForward:the direction (on the line )that is clipped
	//! \return Returns true if there was an intersection, false if there was not.
	bool ClipLine(const TVector3DTemp<T>& linePoint, const TVector3DTemp<T>& lineVect,
		T& tIntersection, bool &bClipForward) const
	{
		T t2 = Normal.Dot(lineVect);
		if ((t2 >= (T)-0.001f) && (t2 <= (T)0.001f))
			return false;
		tIntersection = -(Normal.Dot(linePoint) - D) / t2;
		bClipForward = (t2 > 0);
		return true;
	}

	//! Returns where on a line between two points an intersection with this TPlane happened.
	//! Only useful if known that there is an intersection.
	//! \param linePoint1: Point1 of the line to intersect with.
	//! \param linePoint2: Point2 of the line to intersect with.
	//! \return Returns where on a line between two points an intersection with this TPlane happened.
	//! For example, 0.5 is returned if the intersection happened exectly in the middle of the two points.
	T GetKnownIntersectionWithLine(const TVector3DTemp<T>& linePoint1,
		const TVector3DTemp<T>& linePoint2) const
	{
		TVector3DTemp<T> vect = linePoint2 - linePoint1;
		T t2 = Normal.Dot(vect);
		return -((Normal.Dot(linePoint1) - D) / t2);
		//return -((Normal.dotProduct(linePoint1) + D) / t2);
	}

	//! Returns an intersection with a 3d line, limited between two 3d points.
	//! \param linePoint1: Point 1 of the line.
	//! \param linePoint2: Point 2 of the line.
	//! \param outIntersection: Place to store the intersection point, if there is one.
	//! \return Returns true if there was an intersection, false if there was not.
	bool GetIntersectionWithLimitedLine(const TVector3DTemp<T>& linePoint1,
		const TVector3DTemp<T>& linePoint2, TVector3DTemp<T>& outIntersection) const
	{
		return (GetIntersectionWithLine(linePoint1, linePoint2 - linePoint1, outIntersection) &&
			outIntersection.IsBetweenPoints(linePoint1, linePoint2));
	}

	//! Classifies the relation of a point to this TPlane.
	//! \param point: Point to classify its relation.
	//! \return Returns ISREL3D_FRONT if the point is in front of the TPlane,
	//! ISREL3D_BACK if the point is behind of the TPlane, and
	//! ISREL3D_PLANAR if the point is within the TPlane.
	kEIntersectionRelation3D ClassifyPointRelation(const TVector3DTemp<T>& point) const
	{
		T d = Normal.Dot(point) - D;
		//f32 d = Normal.dotProduct(point) + D;

		if (d < -K_ROUNDING_ERROR)
			return kISREL3D_BACK;

		if (d > K_ROUNDING_ERROR)
			return kISREL3D_FRONT;

		return kISREL3D_PLANAR;
	}

	//! Recalculates the distance from origin by applying
	//! a new member point to the TPlane.
	void RecalculateD(const TVector3DTemp<T>& MPoint)
	{
		D = MPoint.Dot(Normal);
		//D =  -MPoint.dotProduct(Normal);
	}

	//!\return Returns a member point of the TPlane.
	TVector3DTemp<T> GetMemberPoint() const
	{
		return Normal * D;
	}

	//get the equation coefficient(ABCD) of this TPlane
	//note that the inequation of A*x+B*y+C*z+D>=0 represent the front of the TPlane
	void GetEquation(TVector4DTemp<T>&abcd) const
	{
		TVector3DTemp<T>mp = GetMemberPoint();
		abcd.X = Normal.X;
		abcd.y = Normal.y;
		abcd.z = Normal.z;
		abcd.w = -(Normal.X*mp.X + Normal.y*mp.y + Normal.z*mp.z);
	}

	//! Tests if there is a intersection between this TPlane and another
	//! \return Returns true if there is a intersection.
	bool ExistsInterSection(const TPlaneTemp<T>& other) const
	{
		TVector3DTemp<T> cross = other.Normal.CrossProduct(Normal);
		return cross.Size() > 1e-08f;
	}
	//! Intersects this TPlane with another.
	//! \return Returns true if there is a intersection, false if not.
	bool GetIntersectionWithPlane(const TPlaneTemp<T>& other, TVector3DTemp<T>& outLinePoint,
		TVector3DTemp<T>& outLineVect) const
	{
		T fn00 = Normal.Size();
		T fn01 = Normal.Dot(other.Normal);
		T fn11 = other.Normal.Size();
		T det = fn00*fn11 - fn01*fn01;

		if (fabs(det) < 1e-05f)
			return false;

		det = 1.0 / det;
		T fc0 = (fn11*D - fn01*other.D) * det;
		T fc1 = (fn00*+other.D - fn01*D) * det;

		outLineVect = Normal.CrossProduct(other.Normal);
		outLinePoint = Normal*(T)fc0 + other.Normal*(T)fc1;
		return true;
	}
	//! Returns the intersection point with two other TPlanes if there is one.
	bool GetIntersectionWithTPlanes(const TPlaneTemp<T>& o1, const TPlaneTemp<T>& o2, TVector3DTemp<T>& outPoint) const
	{
		TVector3DTemp<T> linePoint, lineVect;
		if (GetIntersectionWithPlane(o1, linePoint, lineVect))
			return o2.GetIntersectionWithLine(linePoint, lineVect, outPoint);
		return false;
	}
	//! Returns if the TPlane is front of backfacing. Note that this only
	//! works if the normal is Normalized.
	//! \param lookDirection: Look direction.
	//! \return Returns true if the TPlane is front facing, which mean it would
	//! be visible, and false if it is backfacing.
	bool IsFrontFacing(const TVector3DTemp<T>& lookDirection) const
	{
		return Normal.Dot(lookDirection) <= 0.0f;
	}
	//! Returns the distance to a point.  Note that this only
	//! works if the normal is Normalized.
	T GetDistanceTo(const TVector3DTemp<T>& point) const
	{
		return point.Dot(Normal) - D;
	}

	//! Returns the distance to a point.  Note that this only
	//! works if the normal is Normalized.
	void GetProjectionOf(const TVector3DTemp<T>& point, TVector3DTemp<T>&pointProj) const
	{
		T dist;
		dist = point.Dot(Normal) - D;
		pointProj = point + (-Normal* dist);
	}
	//-------------------------------------------------------------------------------------------------------------
	void Flip()
	{
		Normal.X = -Normal.X;
		Normal.y = -Normal.y;
		Normal.z = -Normal.z;
		D = -D;
	}

	inline T Dot(const TVector3DTemp<T>& MPoint) const
	{
		return Normal.Dot(MPoint) - D;
	}

	TPlaneTemp operator+(const TPlaneTemp& V) const
	{
		return TPlaneTemp(Normal + V.Normal, D + V.D);
	}

	TPlaneTemp operator-(const TPlaneTemp& V) const
	{
		return TPlaneTemp(Normal - V.Normal, D - V.D);
	}

	TPlaneTemp operator/(const T  Scale) const
	{
		T  RScale = 1.f / Scale;
		return TPlaneTemp(Normal* RScale, D * RScale);
	}

	TPlaneTemp operator*(const T  Scale) const
	{
		return TPlaneTemp(Normal* Scale, D * Scale);
	}
	TPlaneTemp operator*(const TPlaneTemp& V)
	{
		return TPlaneTemp(Normal*V.Normal, D * V.D);
	}

	TPlaneTemp operator+=(const TPlaneTemp& V)
	{
		Normal += V.Normal; D += V.D;
		return *this;
	}
	TPlaneTemp operator-=(const TPlaneTemp& V)
	{
		Normal -= V.Normal; D -= V.D;
		return *this;
	}
	TPlaneTemp operator*=(const T Scale)
	{
		Normal *= Scale; D *= Scale;
		return *this;
	}
	TPlaneTemp operator*=(const TPlaneTemp& V)
	{
		Normal *= V.Normal; D *= V.D;
		return *this;
	}
	TPlaneTemp operator/=(const T Scale)
	{
		float RV = 1.f / Scale;
		Normal *= RV; D *= RV;
		return *this;
	}
	TVector3DTemp<T> Normal;		// normal vector
	T D;						// distance from origin
};

typedef TPlaneTemp<float>  kPlane3D;



