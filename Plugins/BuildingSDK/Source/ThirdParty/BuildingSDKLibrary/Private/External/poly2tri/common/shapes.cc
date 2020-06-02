/*
 * Poly2Tri Copyright (c) 2009-2010, Poly2Tri Contributors
 * http://code.google.com/p/poly2tri/
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must AddRef the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * * Neither the name of Poly2Tri nor the names of its contributors may be
 *   used to endorse or promote products derived from this software without specific
 *   prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "poly2tri/common/shapes.h"
#include <iostream>

namespace p2t {

Triangle::Triangle(IndexPoint& a, IndexPoint& b, IndexPoint& c)
{
  points_[0] = &a; points_[1] = &b; points_[2] = &c;
  neighbors_[0] = NULL; neighbors_[1] = NULL; neighbors_[2] = NULL;
  constrained_edge[0] = constrained_edge[1] = constrained_edge[2] = false;
  delaunay_edge[0] = delaunay_edge[1] = delaunay_edge[2] = false;
  interior_ = false;
}

// Update neighbor pointers
void Triangle::MarkNeighbor(IndexPoint* p1, IndexPoint* p2, Triangle* t)
{
  if ((p1 == points_[2] && p2 == points_[1]) || (p1 == points_[1] && p2 == points_[2]))
    neighbors_[0] = t;
  else if ((p1 == points_[0] && p2 == points_[2]) || (p1 == points_[2] && p2 == points_[0]))
    neighbors_[1] = t;
  else if ((p1 == points_[0] && p2 == points_[1]) || (p1 == points_[1] && p2 == points_[0]))
    neighbors_[2] = t;
  else
    assert(0);
}

// Exhaustive search to update neighbor pointers
void Triangle::MarkNeighbor(Triangle& t)
{
  if (t.Contains(points_[1], points_[2])) {
    neighbors_[0] = &t;
    t.MarkNeighbor(points_[1], points_[2], this);
  } else if (t.Contains(points_[0], points_[2])) {
    neighbors_[1] = &t;
    t.MarkNeighbor(points_[0], points_[2], this);
  } else if (t.Contains(points_[0], points_[1])) {
    neighbors_[2] = &t;
    t.MarkNeighbor(points_[0], points_[1], this);
  }
}

/**
 * Clears all references to all other triangles and points
 */
void Triangle::Clear()
{
    Triangle *t;
    for( int i=0; i<3; i++ )
    {
        t = neighbors_[i];
        if( t != NULL )
        {
            t->ClearNeighbor( this );
        }
    }
    ClearNeighbors();
    points_[0]=points_[1]=points_[2] = NULL;
}

void Triangle::ClearNeighbor(const Triangle *triangle )
{
    if( neighbors_[0] == triangle )
    {
        neighbors_[0] = NULL;
    }
    else if( neighbors_[1] == triangle )
    {
        neighbors_[1] = NULL;
    }
    else
    {
        neighbors_[2] = NULL;
    }
}

void Triangle::ClearNeighbors()
{
  neighbors_[0] = NULL;
  neighbors_[1] = NULL;
  neighbors_[2] = NULL;
}

void Triangle::ClearDelunayEdges()
{
  delaunay_edge[0] = delaunay_edge[1] = delaunay_edge[2] = false;
}

IndexPoint* Triangle::OppositePoint(Triangle& t, const IndexPoint& p)
{
  IndexPoint *cw = t.PointCW(p);
  return cw? PointCW(*cw) : nullptr;
}

// Legalized triangle by rotating clockwise around point(0)
void Triangle::Legalize(IndexPoint& point)
{
  points_[1] = points_[0];
  points_[0] = points_[2];
  points_[2] = &point;
}

// Legalize triagnle by rotating clockwise around oPoint
void Triangle::Legalize(IndexPoint& opoint, IndexPoint& npoint)
{
  if (&opoint == points_[0]) {
    points_[1] = points_[0];
    points_[0] = points_[2];
    points_[2] = &npoint;
  } else if (&opoint == points_[1]) {
    points_[2] = points_[1];
    points_[1] = points_[0];
    points_[0] = &npoint;
  } else if (&opoint == points_[2]) {
    points_[0] = points_[2];
    points_[2] = points_[1];
    points_[1] = &npoint;
  } else {
    assert(0);
  }
}

int Triangle::Index(const IndexPoint* p)
{
  if (p == points_[0]) {
    return 0;
  } else if (p == points_[1]) {
    return 1;
  } else if (p == points_[2]) {
    return 2;
  }
  assert(0);
  return -1;
}

int Triangle::EdgeIndex(const IndexPoint* p1, const IndexPoint* p2)
{
  if (points_[0] == p1) {
    if (points_[1] == p2) {
      return 2;
    } else if (points_[2] == p2) {
      return 1;
    }
  } else if (points_[1] == p1) {
    if (points_[2] == p2) {
      return 0;
    } else if (points_[0] == p2) {
      return 2;
    }
  } else if (points_[2] == p1) {
    if (points_[0] == p2) {
      return 1;
    } else if (points_[1] == p2) {
      return 0;
    }
  }
  return -1;
}

void Triangle::MarkConstrainedEdge(int index)
{
  constrained_edge[index] = true;
}

void Triangle::MarkConstrainedEdge(Edge& edge)
{
  MarkConstrainedEdge(edge.p, edge.q);
}

// Mark edge as constrained
void Triangle::MarkConstrainedEdge(IndexPoint* p, IndexPoint* q)
{
  if ((q == points_[0] && p == points_[1]) || (q == points_[1] && p == points_[0])) {
    constrained_edge[2] = true;
  } else if ((q == points_[0] && p == points_[2]) || (q == points_[2] && p == points_[0])) {
    constrained_edge[1] = true;
  } else if ((q == points_[1] && p == points_[2]) || (q == points_[2] && p == points_[1])) {
    constrained_edge[0] = true;
  }
}

// The point counter-clockwise to given point
IndexPoint* Triangle::PointCW(const IndexPoint& point)
{
  if (&point == points_[0]) {
    return points_[2];
  } else if (&point == points_[1]) {
    return points_[0];
  } else if (&point == points_[2]) {
    return points_[1];
  }
  assert(0);
  return NULL;
}

// The point counter-clockwise to given point
IndexPoint* Triangle::PointCCW(const IndexPoint& point)
{
  if (&point == points_[0]) {
    return points_[1];
  } else if (&point == points_[1]) {
    return points_[2];
  } else if (&point == points_[2]) {
    return points_[0];
  }
  assert(0);
  return NULL;
}

// The neighbor clockwise to given point
Triangle* Triangle::NeighborCW(const IndexPoint& point)
{
  if (&point == points_[0]) {
    return neighbors_[1];
  } else if (&point == points_[1]) {
    return neighbors_[2];
  }
  return neighbors_[0];
}

// The neighbor counter-clockwise to given point
Triangle* Triangle::NeighborCCW(const IndexPoint& point)
{
  if (&point == points_[0]) {
    return neighbors_[2];
  } else if (&point == points_[1]) {
    return neighbors_[0];
  }
  return neighbors_[1];
}

bool Triangle::GetConstrainedEdgeCCW(const IndexPoint& p)
{
  if (&p == points_[0]) {
    return constrained_edge[2];
  } else if (&p == points_[1]) {
    return constrained_edge[0];
  }
  return constrained_edge[1];
}

bool Triangle::GetConstrainedEdgeCW(const IndexPoint& p)
{
  if (&p == points_[0]) {
    return constrained_edge[1];
  } else if (&p == points_[1]) {
    return constrained_edge[2];
  }
  return constrained_edge[0];
}

void Triangle::SetConstrainedEdgeCCW(const IndexPoint& p, bool ce)
{
  if (&p == points_[0]) {
    constrained_edge[2] = ce;
  } else if (&p == points_[1]) {
    constrained_edge[0] = ce;
  } else {
    constrained_edge[1] = ce;
  }
}

void Triangle::SetConstrainedEdgeCW(const IndexPoint& p, bool ce)
{
  if (&p == points_[0]) {
    constrained_edge[1] = ce;
  } else if (&p == points_[1]) {
    constrained_edge[2] = ce;
  } else {
    constrained_edge[0] = ce;
  }
}

bool Triangle::GetDelunayEdgeCCW(const IndexPoint& p)
{
  if (&p == points_[0]) {
    return delaunay_edge[2];
  } else if (&p == points_[1]) {
    return delaunay_edge[0];
  }
  return delaunay_edge[1];
}

bool Triangle::GetDelunayEdgeCW(const IndexPoint& p)
{
  if (&p == points_[0]) {
    return delaunay_edge[1];
  } else if (&p == points_[1]) {
    return delaunay_edge[2];
  }
  return delaunay_edge[0];
}

void Triangle::SetDelunayEdgeCCW(const IndexPoint& p, bool e)
{
  if (&p == points_[0]) {
    delaunay_edge[2] = e;
  } else if (&p == points_[1]) {
    delaunay_edge[0] = e;
  } else {
    delaunay_edge[1] = e;
  }
}

void Triangle::SetDelunayEdgeCW(const IndexPoint& p, bool e)
{
  if (&p == points_[0]) {
    delaunay_edge[1] = e;
  } else if (&p == points_[1]) {
    delaunay_edge[2] = e;
  } else {
    delaunay_edge[0] = e;
  }
}

// The neighbor across to given point
Triangle* Triangle::NeighborAcross(const IndexPoint& opoint)
{
  if (&opoint == points_[0]) {
    return neighbors_[0];
  } else if (&opoint == points_[1]) {
    return neighbors_[1];
  }
  return neighbors_[2];
}

void Triangle::DebugPrint()
{
  using namespace std;
  cout << points_[0]->x << "," << points_[0]->y << " ";
  cout << points_[1]->x << "," << points_[1]->y << " ";
  cout << points_[2]->x << "," << points_[2]->y << endl;
}

//////////////////////////////////////////////////////////////////////////
IndexPoint* Triangle::GetPoint(int index)
{
	return points_[index];
}

Triangle* Triangle::GetNeighbor(int index)
{
	return neighbors_[index];
}

bool Triangle::Contains(const IndexPoint* p)
{
	return p == points_[0] || p == points_[1] || p == points_[2];
}

bool Triangle::Contains(const Edge& e)
{
	return Contains(e.p) && Contains(e.q);
}

bool Triangle::Contains(const IndexPoint* p, const IndexPoint* q)
{
	return Contains(p) && Contains(q);
}

bool Triangle::IsInterior()
{
	return interior_;
}

void Triangle::IsInterior(bool b)
{
	interior_ = b;
}

//////////////////////////////////////////////////////////////////////////
bool cmp(const IndexPoint* a, const IndexPoint* b)
{
	if (a->y < b->y) {
		return true;
	}
	else if (a->y == b->y) {
		// Make sure q is point with greater x value
		if (a->x < b->x) {
			return true;
		}
	}
	return false;
}

/// Add two points_ component-wise.
IndexPoint operator +(const IndexPoint& a, const IndexPoint& b)
{
	return IndexPoint(a.x + b.x, a.y + b.y);
}

/// Subtract two points_ component-wise.
IndexPoint operator -(const IndexPoint& a, const IndexPoint& b)
{
	return IndexPoint(a.x - b.x, a.y - b.y);
}

/// Multiply point by scalar
IndexPoint operator *(double s, const IndexPoint& a)
{
	return IndexPoint(s * a.x, s * a.y);
}

bool operator ==(const IndexPoint& a, const IndexPoint& b)
{
	return a.x == b.x && a.y == b.y;
}

bool operator !=(const IndexPoint& a, const IndexPoint& b)
{
	return !(a.x == b.x) && !(a.y == b.y);
}

/// Peform the dot product on two vectors.
double Dot(const IndexPoint& a, const IndexPoint& b)
{
	return a.x * b.x + a.y * b.y;
}

/// Perform the cross product on two vectors. In 2D this produces a scalar.
double Cross(const IndexPoint& a, const IndexPoint& b)
{
	return a.x * b.y - a.y * b.x;
}

/// Perform the cross product on a point and a scalar. In 2D this produces
/// a point.
IndexPoint Cross(const IndexPoint& a, double s)
{
	return IndexPoint(s * a.y, -s * a.x);
}

/// Perform the cross product on a scalar and a point. In 2D this produces
/// a point.
IndexPoint Cross(double s, const IndexPoint& a)
{
	return IndexPoint(-s * a.y, s * a.x);
}

}