
#pragma  once

#include "kLine.h"
#include "kVector3D.h"
#include "kPlane.h"
#include "kSphere.h"

template <class T>
class TBoxTemp
{
public:
	// Constructors
	TBoxTemp() { Invalid(); };
	TBoxTemp(const TVector3DTemp<T>& min, const TVector3DTemp<T>& max) : MinEdge(min), MaxEdge(max) {};
	TBoxTemp(const TVector3DTemp<T>& init) : MinEdge(init), MaxEdge(init) {};
	TBoxTemp(T minx, T miny, T minz, T maxx, T maxy, T maxz) : MinEdge(minx, miny, minz), MaxEdge(maxx, maxy, maxz) {};

	// operators
	inline bool operator==(const TBoxTemp<T>& other) const { return (MinEdge == other.MinEdge && other.MaxEdge == MaxEdge); };
	inline bool operator!=(const TBoxTemp<T>& other) const { return !(MinEdge == other.MinEdge && other.MaxEdge == MaxEdge); };
	inline TBoxTemp &operator&=(const TBoxTemp<T>& other)
	{
		if (MinEdge.X)
			return !(MinEdge == other.MinEdge && other.MaxEdge == MaxEdge);
	};

	// functions
	//! Adds a point to the bounding TBox, causing it to grow bigger, 
	//! if point is outside of the TBox
	//! \param p: Point to add into the TBox.
	void Add(const TVector3DTemp<T>& p)
	{
		Add(p.X, p.Y, p.Z);
	}

	//! Adds an other bounding TBox to the bounding TBox, causing it to grow bigger,
	//! if the TBox is outside of the TBox
	//! \param b: Other bounding TBox to add into this TBox.
	void Add(const TBoxTemp<T>& b)
	{
		if (IsInvalid())
		{
			*this = b;
			return;
		}
		if (b.MinEdge.X<MinEdge.X)
			MinEdge.X = b.MinEdge.X;
		if (b.MinEdge.Y<MinEdge.Y)
			MinEdge.Y = b.MinEdge.Y;
		if (b.MinEdge.Z<MinEdge.Z)
			MinEdge.Z = b.MinEdge.Z;

		if (b.MaxEdge.X>MaxEdge.X)
			MaxEdge.X = b.MaxEdge.X;
		if (b.MaxEdge.Y>MaxEdge.Y)
			MaxEdge.Y = b.MaxEdge.Y;
		if (b.MaxEdge.Z>MaxEdge.Z)
			MaxEdge.Z = b.MaxEdge.Z;
	}

	//! Resets the bounding TBox.
	void Set(T x, T y, T z)
	{
		MaxEdge.Set(x, y, z);
		MinEdge = MaxEdge;
	}

	//! Resets the bounding TBox.
	void Set(const TBoxTemp<T>& initValue)
	{
		*this = initValue;
	}

	//! Resets the bounding TBox.
	void Set(const TVector3DTemp<T>& initValue)
	{
		MaxEdge = initValue;
		MinEdge = initValue;
	}

	void Invalid()
	{
		MinEdge.Set(1, 1, 1);
		MaxEdge.Set(-1, -1, -1);
	}

	//! Adds a point to the bounding TBox, causing it to grow bigger, 
	//! if point is outside of the TBox.
	//! \param x: X Coordinate of the point to add to this TBox.
	//! \param y: Y Coordinate of the point to add to this TBox.
	//! \param z: Z Coordinate of the point to add to this TBox.
	void Add(T x, T y, T z)
	{
		if (IsInvalid())
		{
			Set(x, y, z);
			return;
		}
		if (x>MaxEdge.X) MaxEdge.X = x;
		if (y>MaxEdge.Y) MaxEdge.Y = y;
		if (z>MaxEdge.Z) MaxEdge.Z = z;

		if (x<MinEdge.X) MinEdge.X = x;
		if (y<MinEdge.Y) MinEdge.Y = y;
		if (z<MinEdge.Z) MinEdge.Z = z;
	}

	void Expand(T x, T y, T z)
	{
		MinEdge.X -= x;
		MinEdge.Y -= y;
		MinEdge.Z -= z;

		MaxEdge.X += x;
		MaxEdge.Y += y;
		MaxEdge.Z += z;
	}

	TBoxTemp<T> &Move(const TVector3DTemp<T>& p)
	{
		MinEdge += p;
		MaxEdge += p;
		return *this;
	}

	//! Determinates if a point is within this TBox.
	//! \param p: Point to check.
	//! \return Returns true if the point is withing the TBox, and false if it is not.
	bool IsPointInside(const TVector3DTemp<T>& p) const
	{
		return (p.X >= MinEdge.X && p.X <= MaxEdge.X &&
			p.Y >= MinEdge.Y && p.Y <= MaxEdge.Y &&
			p.Z >= MinEdge.Z && p.Z <= MaxEdge.Z);
	}

	//! Determinates if a point is within this TBox and its borders.
	//! \param p: Point to check.
	//! \return Returns true if the point is withing the TBox, and false if it is not.
	bool IsPointTotalInside(const TVector3DTemp<T>& p) const
	{
		if (p.x < MinEdge.x)
			return false;

		if (p.x > MaxEdge.x)
			return false;
	
		if (p.y < MinEdge.y)
			return false;

		if (p.y > MaxEdge.y)
			return false;

		if (p.z < MinEdge.z)
			return false;

		if (p.z > MaxEdge.z)
			return false;

		return true;
	}

	//! Determinates if the TBox intersects with another TBox.
	//! \param other: Other TBox to check a intersection with.
	//! \return Returns true if there is a intersection with the other TBox, 
	//! otherwise false.
	bool IntersectsWithBox(const TBoxTemp<T>& other) const
	{
		return (MinEdge <= other.MaxEdge && MaxEdge >= other.MinEdge);
	}

	//! Determinates if the TBox totally contains another TBox.
	bool ContainsBox(const TBoxTemp<T>& other) const
	{
		return (MinEdge <= other.MinEdge&&MaxEdge >= other.MaxEdge);
	}

	//! Tests if the TBox intersects with a line
	//! \param line: Line to test intersection with.
	//! \return Returns true if there is an intersection and false if not.
	bool IntersectsWithLine(const TLineTemp<T>& line) const
	{
		return IntersectsWithLine(line.Middle(), line.Vector().Normalize(), (T)(line.Size() * 0.5));
	}

	//! Tests if the TBox intersects with a line
	//! \return Returns true if there is an intersection and false if not.
	bool IntersectsWithLine(const TVector3DTemp<T>& linemiddle,
		const TVector3DTemp<T>& linevect,
		T halflength) const
	{
		const TVector3DTemp<T> e = (MaxEdge - MinEdge) * (T)0.5;
		const TVector3DTemp<T> t = (MinEdge + e) - linemiddle;
		float r;

		if ((fabs(t.X) > e.X + halflength * fabs(linevect.X)) ||
			(fabs(t.Y) > e.Y + halflength * fabs(linevect.Y)) ||
			(fabs(t.Z) > e.Z + halflength * fabs(linevect.Z)))
			return false;

		r = e.Y * (T)fabs(linevect.Z) + e.Z * (T)fabs(linevect.Y);
		if (fabs(t.Y*linevect.Z - t.Z*linevect.Y) > r)
			return false;

		r = e.X * (T)fabs(linevect.Z) + e.Z * (T)fabs(linevect.X);
		if (fabs(t.Z*linevect.X - t.X*linevect.Z) > r)
			return false;

		r = e.X * (T)fabs(linevect.Y) + e.Y * (T)fabs(linevect.X);
		if (fabs(t.X*linevect.Y - t.Y*linevect.X) > r)
			return false;

		return true;
	}


	//! Classifies a relation with a plane.
	//! \param plane: Plane to classify relation to.
	//! \return Returns ISREL3D_FRONT if the TBox is in front of the plane,
	//! ISREL3D_BACK if the TBox is back of the plane, and
	//! ISREL3D_CLIPPED if is on both sides of the plane.
	kEIntersectionRelation3D ClassifyPlaneRelation(const TPlaneTemp<T>& plane) const
	{
		TVector3DTemp<T> nearPoint(MaxEdge);
		TVector3DTemp<T> farPoint(MinEdge);

		if (plane.Normal.X > (T)0)
		{
			nearPoint.X = MinEdge.X;
			farPoint.X = MaxEdge.X;
		}

		if (plane.Normal.Y > (T)0)
		{
			nearPoint.Y = MinEdge.Y;
			farPoint.Y = MaxEdge.Y;
		}

		if (plane.Normal.Z > (T)0)
		{
			nearPoint.Z = MinEdge.Z;
			farPoint.Z = MaxEdge.Z;
		}

		if (plane.Normal.Dot(nearPoint) - plane.D > (T)0)
			return kISREL3D_FRONT;

		if (plane.Normal.Dot(farPoint) - plane.D > (T)0)
			return kISREL3D_CLIPPED;

		return kISREL3D_BACK;
	}

	bool IntersectsWithConvexShape(const TPlaneTemp<T> *p_planes, int p_plane_count) const
	{

		TVector3DTemp<T> half_extents = GetHalfExtent();
		TVector3DTemp<T> ofs = MinEdge + half_extents;

		for (int i = 0; i<p_plane_count; i++)
		{
			const TPlaneTemp<T> &p = p_planes[i];
			TVector3DTemp<T> point
			(
				(p.Normal.X>0) ? -half_extents.X : half_extents.X,
				(p.Normal.y>0) ? -half_extents.y : half_extents.y,
				(p.Normal.z>0) ? -half_extents.z : half_extents.z
			);

			point += ofs;

			if (p.Dot(point)>0)
			{
				return false;
			}
		}

		return true;
	}

	bool IntersectsWithSegment(const TVector3DTemp<T>& p_from, const TVector3DTemp<T>& p_to, TVector3DTemp<T>* r_clip = NULL, TVector3DTemp<T>* r_normal = NULL) const
	{
		T min = 0, max = 1;
		int axis = 0;
		float sign = 0;

		for (int i = 0; i<3; i++) {

			T seg_from = p_from.Value[i];
			T seg_to = p_to.Value[i];

			T box_begin = MinEdge.Value[i];
			T box_end = MaxEdge.Value[i];

			T cmin, cmax;
			float csign;

			if (seg_from < seg_to) {

				if (seg_from > box_end || seg_to < box_begin)
					return false;

				T length = seg_to - seg_from;
				cmin = (seg_from < box_begin) ? ((box_begin - seg_from) / length) : 0;
				cmax = (seg_to > box_end) ? ((box_end - seg_from) / length) : 1;
				csign = -1.0;

			}
			else {

				if (seg_to > box_end || seg_from < box_begin)
					return false;

				T length = seg_to - seg_from;
				cmin = (seg_from > box_end) ? (box_end - seg_from) / length : 0;
				cmax = (seg_to < box_begin) ? (box_begin - seg_from) / length : 1;
				csign = 1.0;
			}

			if (cmin > min) {
				min = cmin;
				axis = i;
				sign = csign;
			}
			if (cmax < max)
				max = cmax;
			if (max < min)
				return false;
		}


		TVector3DTemp<T> rel = p_to - p_from;

		if (r_normal) {
			TVector3DTemp<T> normal;
			normal.Value[axis] = sign;
			*r_normal = normal;
		}

		if (r_clip)
			*r_clip = p_from + rel*min;

		return true;
	}

	//! returns center of the bounding TBox
	TVector3DTemp<T> GetCenter() const
	{
		return (MinEdge + MaxEdge) / 2;
	}

	//! returns extend of the TBox
	TVector3DTemp<T> GetExtent() const
	{
		return MaxEdge - MinEdge;
	}

	TVector3DTemp<T> GetHalfExtent() const
	{
		return (MaxEdge - MinEdge) / 2;
	}

	//the 6 planes are in the order:left,right,front,back,up,down
	void GetPlanes(TPlaneTemp<T> *planes) const
	{
		TVector3DTemp<T> middle = (MinEdge + MaxEdge) / 2;
		TVector3DTemp<T> diag = middle - MaxEdge;

		TVector3DTemp<T> corners[8];

		corners[0].Set(middle.X + diag.X, middle.Y + diag.Y, middle.Z + diag.Z);
		corners[1].Set(middle.X + diag.X, middle.Y - diag.Y, middle.Z + diag.Z);
		corners[2].Set(middle.X + diag.X, middle.Y + diag.Y, middle.Z - diag.Z);
		corners[3].Set(middle.X + diag.X, middle.Y - diag.Y, middle.Z - diag.Z);
		corners[4].Set(middle.X - diag.X, middle.Y + diag.Y, middle.Z + diag.Z);
		corners[5].Set(middle.X - diag.X, middle.Y - diag.Y, middle.Z + diag.Z);
		corners[6].Set(middle.X - diag.X, middle.Y + diag.Y, middle.Z - diag.Z);
		corners[7].Set(middle.X - diag.X, middle.Y - diag.Y, middle.Z - diag.Z);

		//All these points are clockwise when looking from OUTSIDE the aabb
		planes[0].Set(corners[4], corners[6], corners[5]);
		planes[1].Set(corners[2], corners[0], corners[1]);
		planes[2].Set(corners[6], corners[2], corners[7]);
		planes[3].Set(corners[0], corners[4], corners[5]);
		planes[4].Set(corners[0], corners[6], corners[4]);
		planes[5].Set(corners[3], corners[1], corners[7]);
	}


	//! stores all 8 corners of the TBox into a array
	//! \param corners: Pointer to array of 8 corners
	void GetCorners(TVector3DTemp<T> *corners) const
	{
		TVector3DTemp<T> middle = (MinEdge + MaxEdge) / 2;
		TVector3DTemp<T> diag = middle - MaxEdge;
		/*
		Corners are stored in this way:
		Hey, am I an ascii artist, or what? :) niko.
		//                  /4--------/0
		//                 /  |      / |
		//                /   |     /  |
		//                6---------2  |
		//                |   5- - -| -1
		//                |  /      |  /
		//                |/        | /
		//                7---------3/

		//			MaMaHuHu, cxi
		*/

		corners[0].Set(middle.X + diag.X, middle.Y + diag.Y, middle.Z + diag.Z);
		corners[1].Set(middle.X + diag.X, middle.Y - diag.Y, middle.Z + diag.Z);
		corners[2].Set(middle.X + diag.X, middle.Y + diag.Y, middle.Z - diag.Z);
		corners[3].Set(middle.X + diag.X, middle.Y - diag.Y, middle.Z - diag.Z);
		corners[4].Set(middle.X - diag.X, middle.Y + diag.Y, middle.Z + diag.Z);
		corners[5].Set(middle.X - diag.X, middle.Y - diag.Y, middle.Z + diag.Z);
		corners[6].Set(middle.X - diag.X, middle.Y + diag.Y, middle.Z - diag.Z);
		corners[7].Set(middle.X - diag.X, middle.Y - diag.Y, middle.Z - diag.Z);
	}


	//! stores all 12 edges of the TBox into a array
	//! \param edges: Pointer to array of 12 edges
	void GetEdges(TLineTemp<T> *edges) const
	{
		TVector3DTemp<T> middle = (MinEdge + MaxEdge) / 2;
		TVector3DTemp<T> diag = middle - MaxEdge;
		TVector3DTemp<T> corners[8];
		corners[0].Set(middle.X + diag.X, middle.Y + diag.Y, middle.Z + diag.Z);
		corners[1].Set(middle.X + diag.X, middle.Y - diag.Y, middle.Z + diag.Z);
		corners[2].Set(middle.X + diag.X, middle.Y + diag.Y, middle.Z - diag.Z);
		corners[3].Set(middle.X + diag.X, middle.Y - diag.Y, middle.Z - diag.Z);
		corners[4].Set(middle.X - diag.X, middle.Y + diag.Y, middle.Z + diag.Z);
		corners[5].Set(middle.X - diag.X, middle.Y - diag.Y, middle.Z + diag.Z);
		corners[6].Set(middle.X - diag.X, middle.Y + diag.Y, middle.Z - diag.Z);
		corners[7].Set(middle.X - diag.X, middle.Y - diag.Y, middle.Z - diag.Z);

		edges[0].Set(corners[0], corners[1]);
		edges[1].Set(corners[2], corners[3]);
		edges[2].Set(corners[4], corners[5]);
		edges[3].Set(corners[6], corners[7]);
		edges[4].Set(corners[1], corners[3]);
		edges[5].Set(corners[5], corners[7]);
		edges[6].Set(corners[0], corners[2]);
		edges[7].Set(corners[4], corners[6]);
		edges[8].Set(corners[1], corners[5]);
		edges[9].Set(corners[3], corners[7]);
		edges[10].Set(corners[0], corners[4]);
		edges[11].Set(corners[2], corners[6]);
	}

	void GetDiagonals(TLineTemp<T> *diagonals) const
	{
		TVector3DTemp<T> middle = (MinEdge + MaxEdge) / 2;
		TVector3DTemp<T> diag = middle - MaxEdge;

		TVector3DTemp<T> corners[8];
		corners[0].Set(middle.X + diag.X, middle.Y + diag.Y, middle.Z + diag.Z);
		corners[1].Set(middle.X + diag.X, middle.Y - diag.Y, middle.Z + diag.Z);
		corners[2].Set(middle.X + diag.X, middle.Y + diag.Y, middle.Z - diag.Z);
		corners[3].Set(middle.X + diag.X, middle.Y - diag.Y, middle.Z - diag.Z);
		corners[4].Set(middle.X - diag.X, middle.Y + diag.Y, middle.Z + diag.Z);
		corners[5].Set(middle.X - diag.X, middle.Y - diag.Y, middle.Z + diag.Z);
		corners[6].Set(middle.X - diag.X, middle.Y + diag.Y, middle.Z - diag.Z);
		corners[7].Set(middle.X - diag.X, middle.Y - diag.Y, middle.Z - diag.Z);

		diagonals[0].SetLine(corners[6], corners[1]);
		diagonals[1].SetLine(corners[0], corners[7]);
		diagonals[2].SetLine(corners[4], corners[3]);
		diagonals[3].SetLine(corners[2], corners[5]);
	}

	//calculate the coord where the line enter & leave the aabb
	//return whether there is any intersection
	//if linePoint is in the aabb,outIntersectionEnter will be filled with it
	bool CalcIntersectionWithLine(const TVector3DTemp<T>& linePoint, const TVector3DTemp<T>& lineVect, TVector3DTemp<T>& outIntersectionEnter, TVector3DTemp<T>& outIntersectionLeave)
	{
		TPlaneTemp<T> planes[6];
		GetPlanes(planes);

		TVector3DTemp<T> v;
		TVector3DTemp<T> buffer[6];
		int n;
		n = 0;

		//The left/right plane
		planes[0].GetIntersectionWithLine(linePoint, lineVect, v);
		if ((v.Y >= MinEdge.Y) && (v.Y <= MaxEdge.Y) && (v.Z >= MinEdge.Z) && (v.Z <= MaxEdge.Z) && ((v - linePoint).DotProduct(lineVect) >= 0.0f))
			buffer[n++] = v;
		planes[1].GetIntersectionWithLine(linePoint, lineVect, v);
		if ((v.Y >= MinEdge.Y) && (v.Y <= MaxEdge.Y) && (v.Z >= MinEdge.Z) && (v.Z <= MaxEdge.Z) && ((v - linePoint).DotProduct(lineVect) >= 0.0f))
			buffer[n++] = v;

		//The Front/Back plane
		planes[2].GetIntersectionWithLine(linePoint, lineVect, v);
		if ((v.Y >= MinEdge.Y) && (v.Y <= MaxEdge.Y) && (v.X >= MinEdge.X) && (v.X <= MaxEdge.X) && ((v - linePoint).DotProduct(lineVect) >= 0.0f))
			buffer[n++] = v;
		planes[3].GetIntersectionWithLine(linePoint, lineVect, v);
		if ((v.Y >= MinEdge.Y) && (v.Y <= MaxEdge.Y) && (v.X >= MinEdge.X) && (v.X <= MaxEdge.X) && ((v - linePoint).DotProduct(lineVect) >= 0.0f))
			buffer[n++] = v;

		//The Up/Down plane
		planes[4].GetIntersectionWithLine(linePoint, lineVect, v);
		if ((v.Z >= MinEdge.Z) && (v.Z <= MaxEdge.Z) && (v.X >= MinEdge.X) && (v.X <= MaxEdge.X) && ((v - linePoint).DotProduct(lineVect) >= 0.0f))
			buffer[n++] = v;
		planes[5].GetIntersectionWithLine(linePoint, lineVect, v);
		if ((v.Z >= MinEdge.Z) && (v.Z <= MaxEdge.Z) && (v.X >= MinEdge.X) && (v.X <= MaxEdge.X) && ((v - linePoint).DotProduct(lineVect) >= 0.0f))
			buffer[n++] = v;

		if (n <= 0)
			return false;

		int i;
		double maxdistSQ, mindistSQ;
		maxdistSQ = -1.0f;
		mindistSQ = 1e8;
		for (i = 0; i<n; i++)
		{
			double distSQ;
			distSQ = (buffer[i] - linePoint).Size();
			if (distSQ>maxdistSQ)
			{
				maxdistSQ = distSQ;
				outIntersectionLeave = buffer[i];
			}
			if (distSQ<mindistSQ)
			{
				mindistSQ = distSQ;
				outIntersectionEnter = buffer[i];
			}
		}

		if (IsPointInside(linePoint))
			outIntersectionEnter = linePoint;

		return true;
	}

	bool IsInvalid() const
	{
		return  (MinEdge.X>MaxEdge.X + K_ROUNDING_ERROR) ||
			(MinEdge.Y>MaxEdge.Y + K_ROUNDING_ERROR) ||
			(MinEdge.Z>MaxEdge.Z + K_ROUNDING_ERROR);
	}

	//! returns if the TBox is empty, which means that there is
	//! no space within the min and the max edge.
	bool IsEmpty() const
	{
		return  (MinEdge.X >= MaxEdge.X) &&
			(MinEdge.Y >= MaxEdge.Y) &&
			(MinEdge.Z >= MaxEdge.Z);
	}

	bool IsPartialEmpty() const
	{
		return
			(MinEdge.X >= MaxEdge.X) ||
			(MinEdge.Y >= MaxEdge.Y) ||
			(MinEdge.Z >= MaxEdge.Z);
	}


	//! repairs the TBox, if for example MinEdge and MaxEdge are swapped.
	void Repair()
	{
		T t;
		if (MinEdge.X > MaxEdge.X)
		{
			t = MinEdge.X; MinEdge.X = MaxEdge.X; MaxEdge.X = t;
		}
		if (MinEdge.Y > MaxEdge.Y)
		{
			t = MinEdge.Y; MinEdge.Y = MaxEdge.Y; MaxEdge.Y = t;
		}
		if (MinEdge.Z > MaxEdge.Z)
		{
			t = MinEdge.Z; MinEdge.Z = MaxEdge.Z; MaxEdge.Z = t;
		}
	}

	//! Calculates a new interpolated bounding TBox.
	//! \param other: other TBox to interpolate between
	//! \param d: value between 0.0f and 1.0f.
	TBoxTemp<T> GetInterpolated(const TBoxTemp<T>& other, float d) const
	{
		T inv = 1.0f - d;
		return TBoxTemp<T>((other.MinEdge*inv) + (MinEdge*d), (other.MaxEdge*inv) + (MaxEdge*d));
	}

	//split the aabb along the longest axis
	void Split(TBoxTemp<T>&aabb1, TBoxTemp<T>&aabb2)
	{
		int longaxis = 0;
		TVector3DTemp<T> extend = GetExtent();
		TVector3DTemp<T> center = GetCenter();
		if (extend.y>extend.X)
		{
			if (extend.z>extend.y)
				longaxis = 2;
			else
				longaxis = 1;
		}
		else
		{
			if (extend.z>extend.X)
				longaxis = 2;
		}

		aabb1 = aabb2 = (*this);
		((float*)&aabb1.MaxEdge)[longaxis] = ((float*)&center)[longaxis];
		((float*)&aabb2.MinEdge)[longaxis] = ((float*)&center)[longaxis];
	}

	T GetRadius()
	{
		TVector3DTemp<T> Ext = GetHalfExtent();
		if (Ext.X >= Ext.Y)
		{
			return Ext.X>Ext.Z ? Ext.X : Ext.Z;
		}
		return Ext.Y>Ext.Z ? Ext.Y : Ext.Z;
	}

	T GetLongestAxis() const {

		TVector3DTemp<T> Ext = GetExtent();

		T max_size = Ext.X;

		if (Ext.Y>max_size) {
			max_size = Ext.Y;
		}

		if (Ext.Z > max_size) {
			max_size = Ext.Z;
		}

		return max_size;
	}

	void ScaleExt(float InScale)
	{
		TVector3DTemp<T> Ext = MaxEdge - MinEdge;
		Ext = InScale *Ext;
		MaxEdge = MinEdge + Ext;
	}

	TSphereTemp<T> GetBoundSphere()
	{
		TSphereTemp<T> sphere;
		sphere.Set(GetCenter(), ((T)GetExtent().Size()) / 2.0f);
		return sphere;
	}

	void AddSphere(const TSphereTemp<T> &sphere)
	{
		MinEdge.Set(sphere.center.X - sphere.radius, sphere.center.y - sphere.radius, sphere.center.z - sphere.radius);
		MaxEdge.Set(sphere.center.X + sphere.radius, sphere.center.y + sphere.radius, sphere.center.z + sphere.radius);
	}

	//update the TSphere's radius with the aabb(while keeping the center fixed)
	//FC for :Fixed Center
	void AddToBoundSphere(TSphereTemp<T> &sphere)
	{
		TVector3DTemp<T> buf[8];
		GetCorners(buf);
		for (int i = 0; i < 8; i++)
		{
			sphere.AddPoint(buf[i]);
		}
	}

	bool Equals(const TBoxTemp<T> &Other, float epsilon) const
	{
		return MinEdge.Equals(Other.MinEdge, epsilon) && MaxEdge.Equals(Other.MaxEdge, epsilon);
	}

	// member variables
	TVector3DTemp<T> MinEdge;
	TVector3DTemp<T> MaxEdge;
};

typedef  TBoxTemp<float> kBox3D;



