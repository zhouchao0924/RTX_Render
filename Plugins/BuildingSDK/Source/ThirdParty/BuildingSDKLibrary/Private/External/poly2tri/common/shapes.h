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

#pragma once

// Include guard
#ifndef SHAPES_H
#define SHAPES_H

#include <vector>
#include <cstddef>
#include <assert.h>
#include <cmath>

namespace p2t {

struct Edge;

struct IndexPoint {

  int refIndex;
  float x, y;

  /// Default constructor does nothing (for performance).
  IndexPoint()
  {
    x = 0.0;
    y = 0.0;
  }

  /// The edges this point constitutes an upper ending point
  std::vector<Edge*> edge_list;

  /// Construct using coordinates.
  IndexPoint(double x, double y, int i = -1) : x(x), y(y), refIndex(i) {}

  /// Set this point to all zeros.
  void set_zero()
  {
    x = 0.0;
    y = 0.0;
  }

  /// Set this point to some specified coordinates.
  void set(double x_, double y_)
  {
    x = x_;
    y = y_;
  }

  /// Negate this point.
  IndexPoint operator -() const
  {
    IndexPoint v;
    v.set(-x, -y);
    return v;
  }

  /// Add a point to this point.
  void operator +=(const IndexPoint& v)
  {
    x += v.x;
    y += v.y;
  }

  /// Subtract a point from this point.
  void operator -=(const IndexPoint& v)
  {
    x -= v.x;
    y -= v.y;
  }

  /// Multiply this point by a scalar.
  void operator *=(double a)
  {
    x *= a;
    y *= a;
  }

  /// Get the length of this point (the norm).
  double Length() const
  {
    return sqrt(x * x + y * y);
  }

  /// Convert this point into a unit point. Returns the Length.
  double Normalize()
  {
    const double len = Length();
    x /= len;
    y /= len;
    return len;
  }

};

// Represents a simple polygon's edge
struct Edge {

  IndexPoint* p, *q;

  /// Constructor
  Edge(IndexPoint& p1, IndexPoint& p2) : p(&p1), q(&p2)
  {
    if (p1.y > p2.y) {
      q = &p1;
      p = &p2;
    } else if (p1.y == p2.y) {
      if (p1.x > p2.x) {
        q = &p1;
        p = &p2;
      } else if (p1.x == p2.x) {
        // Repeat points
        assert(false);
      }
    }

    q->edge_list.push_back(this);
  }
};

// Triangle-based data structures are know to have better performance than quad-edge structures
// See: J. Shewchuk, "Triangle: Engineering a 2D Quality Mesh Generator and Delaunay Triangulator"
//      "Triangulations in CGAL"
class Triangle {
public:

/// Constructor
Triangle(IndexPoint& a, IndexPoint& b, IndexPoint& c);

/// Flags to determine if an edge is a Constrained edge
bool constrained_edge[3];
/// Flags to determine if an edge is a Delauney edge
bool delaunay_edge[3];

IndexPoint* GetPoint(int index);
IndexPoint* PointCW(const IndexPoint& point);
IndexPoint* PointCCW(const IndexPoint& point);
IndexPoint* OppositePoint(Triangle& t, const IndexPoint& p);

Triangle* GetNeighbor(int index);
void MarkNeighbor(IndexPoint* p1, IndexPoint* p2, Triangle* t);
void MarkNeighbor(Triangle& t);

void MarkConstrainedEdge(int index);
void MarkConstrainedEdge(Edge& edge);
void MarkConstrainedEdge(IndexPoint* p, IndexPoint* q);

int Index(const IndexPoint* p);
int EdgeIndex(const IndexPoint* p1, const IndexPoint* p2);

Triangle* NeighborCW(const IndexPoint& point);
Triangle* NeighborCCW(const IndexPoint& point);
bool GetConstrainedEdgeCCW(const IndexPoint& p);
bool GetConstrainedEdgeCW(const IndexPoint& p);
void SetConstrainedEdgeCCW(const IndexPoint& p, bool ce);
void SetConstrainedEdgeCW(const IndexPoint& p, bool ce);
bool GetDelunayEdgeCCW(const IndexPoint& p);
bool GetDelunayEdgeCW(const IndexPoint& p);
void SetDelunayEdgeCCW(const IndexPoint& p, bool e);
void SetDelunayEdgeCW(const IndexPoint& p, bool e);

bool Contains(const IndexPoint* p);
bool Contains(const Edge& e);
bool Contains(const IndexPoint* p, const IndexPoint* q);
void Legalize(IndexPoint& point);
void Legalize(IndexPoint& opoint, IndexPoint& npoint);
/**
 * Clears all references to all other triangles and points
 */
void Clear();
void ClearNeighbor(const Triangle *triangle);
void ClearNeighbors();
void ClearDelunayEdges();

bool IsInterior();
void IsInterior(bool b);

Triangle* NeighborAcross(const IndexPoint& opoint);

void DebugPrint();

private:

/// Triangle points
IndexPoint* points_[3];
/// Neighbor list
Triangle* neighbors_[3];

/// Has this triangle been marked as an interior triangle?
bool interior_;
};

bool cmp(const IndexPoint* a, const IndexPoint* b);
IndexPoint operator +(const IndexPoint& a, const IndexPoint& b);
IndexPoint operator -(const IndexPoint& a, const IndexPoint& b);
IndexPoint operator *(double s, const IndexPoint& a);
bool operator ==(const IndexPoint& a, const IndexPoint& b);
bool operator !=(const IndexPoint& a, const IndexPoint& b);
double Dot(const IndexPoint& a, const IndexPoint& b);
double Cross(const IndexPoint& a, const IndexPoint& b);
IndexPoint Cross(const IndexPoint& a, double s);
IndexPoint Cross(double s, const IndexPoint& a);

}

#endif