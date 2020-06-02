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

#ifndef SWEEP_CONTEXT_H
#define SWEEP_CONTEXT_H

#include <list>
#include <vector>
#include <cstddef>

namespace p2t {

// Inital triangle factor, seed triangle will extend 30% of
// PointSet width to both left and right.
const double kAlpha = 0.3;

struct IndexPoint;
class Triangle;
struct LiveNode;
struct Edge;
class AdvancingFront;

class SweepContext {
public:

/// Constructor
SweepContext(const std::vector<IndexPoint*>& polyline);
/// Destructor
~SweepContext();

void set_head(IndexPoint* p1);

IndexPoint* head() const;

void set_tail(IndexPoint* p1);

IndexPoint* tail() const;

size_t point_count() const;

LiveNode* LocateNode(const IndexPoint& point);

void RemoveNode(LiveNode* node);

void CreateAdvancingFront(const std::vector<LiveNode*>& nodes);

/// Try to map a node to all sides of this triangle that don't have a neighbor
void MapTriangleToNodes(Triangle& t);

void AddToMap(Triangle* triangle);

IndexPoint* GetPoint(size_t index);

IndexPoint* GetPoints();

void RemoveFromMap(Triangle* triangle);

void AddHole(const std::vector<IndexPoint*>& polyline);

void AddPoint(IndexPoint* point);

AdvancingFront* front() const;

void MeshClean(Triangle& triangle);

std::vector<Triangle*> &GetTriangles();
std::list<Triangle*> &GetMap();

std::vector<Edge*> edge_list;

struct Basin {
  LiveNode* left_node;
  LiveNode* bottom_node;
  LiveNode* right_node;
  double width;
  bool left_highest;

  Basin() : left_node(NULL), bottom_node(NULL), right_node(NULL), width(0.0), left_highest(false)
  {
  }

  void Clear()
  {
    left_node = NULL;
    bottom_node = NULL;
    right_node = NULL;
    width = 0.0;
    left_highest = false;
  }
};

struct EdgeEvent {
  Edge* constrained_edge;
  bool right;

  EdgeEvent() : constrained_edge(NULL), right(false)
  {
  }
};

Basin basin;
EdgeEvent edge_event;

private:

friend class Sweep;

std::vector<Triangle*> triangles_;
std::list<Triangle*> map_;
std::vector<IndexPoint*> points_;

// Advancing front
AdvancingFront* front_;
// head point used with advancing front
IndexPoint* head_;
// tail point used with advancing front
IndexPoint* tail_;

LiveNode *af_head_, *af_middle_, *af_tail_;

void InitTriangulation();
void InitEdges(const std::vector<IndexPoint*>& polyline);

};

inline AdvancingFront* SweepContext::front() const
{
  return front_;
}

inline size_t SweepContext::point_count() const
{
  return points_.size();
}

inline void SweepContext::set_head(IndexPoint* p1)
{
  head_ = p1;
}

inline IndexPoint* SweepContext::head() const
{
  return head_;
}

inline void SweepContext::set_tail(IndexPoint* p1)
{
  tail_ = p1;
}

inline IndexPoint* SweepContext::tail() const
{
  return tail_;
}

}

#endif
