//
// Created by icpmoles on 03/02/26.
//

#ifndef TRIANGULATION_2_EXAMPLES_CLUSTER_H
#define TRIANGULATION_2_EXAMPLES_CLUSTER_H
#include "convexification/utils.hpp"

class ClusterContainer
{
public:
  ClusterContainer() {}
  explicit ClusterContainer(Face_handle f);

  size_t LocateVertex(Vertex_handle v);
  bool ShiftStart(Vertex_handle v_start);
  bool AddEdges(MultiEdge_t edges, Vertex_handle v_preceding, Vertex_handle v_following);
  bool AddVertexes(MultiVertex_t vertexes, Vertex_handle v_preceding, Vertex_handle v_following);
  bool AddFaces(MultiFace_t faces);
  bool Inflate();


  ~ClusterContainer()
  {
    vertexes_.~vector();
    edges_.~vector();
    faces_.~vector();
  }

private:
  MultiVertex_t vertexes_;
  MultiEdge_t edges_;
  MultiFace_t faces_;
  int cluster_id = 0;
};

inline bool ClusterContainer::ShiftStart(Vertex_handle v_start)
{
  size_t v_idx_start = 0 ;

  for(int i = 0; i < vertexes_.size(); i++)
  {
    if (vertexes_[i] == v_start)
      v_idx_start = i;
  }
  if (v_idx_start == vertexes_.size())
  {
    // vertex not found
    return false;
  }
  // The first vertex of the i-th edge should be the same as the one found by linear search.
  // Face_handle f = edges_[v_idx_start].first;
  // int edge_idx = edges_[v_idx_start].second;
  // assert(f->vertex(ccw(edge_idx)) == v_start);
  assert(edges_[v_idx_start].first->vertex(ccw(edges_[v_idx_start].second)) == v_start);


  // reformat vertex list
  MultiVertex_t new_vertexes;
  auto pos_vertex = std::next(vertexes_.begin(), v_idx_start);
  new_vertexes.insert( new_vertexes.begin(), pos_vertex, vertexes_.end());
  new_vertexes.insert( new_vertexes.end(), vertexes_.begin(), std::prev(pos_vertex));
  vertexes_ = new_vertexes;

  // reformat edges list
  MultiEdge_t new_edges;
  auto pos_edge = std::next(edges_.begin(), v_idx_start);
  new_edges.insert( new_edges.begin(), pos_edge, edges_.end());
  new_edges.insert( new_edges.end(), edges_.begin(), std::prev(pos_edge));
  edges_ = new_edges;

  assert(vertexes_[0] == v_start);
  assert(edges_[0].first->vertex(ccw(edges_[0].second)) == v_start);
  return true;

}
inline ClusterContainer::ClusterContainer(Face_handle f)
{
  faces_.push_back(f);
}
inline size_t ClusterContainer::LocateVertex(Vertex_handle v)
{
  for(int i = 0; i < vertexes_.size(); i++)
    if (vertexes_[i] == v_start)
      return i;
  }
  if (v_idx_start == vertexes_.size())
  {
    // vertex not found
    return false;
  }
}
inline bool ClusterContainer::Inflate() {}
inline bool ClusterContainer::AddEdges(MultiEdge_t edges, Vertex_handle v_preceding, Vertex_handle v_following)
{
  size_t v_idx_start = 0 , v_idx_end = 0;

  for(int i = 0; i < vertexes_.size(); i++)
  {
    if (vertexes_[i] == v_preceding)
      v_idx_start = i;
    if (vertexes_[i] == v_following)
      v_idx_end = i;
  }



}
inline bool ClusterContainer::AddVertexes(MultiVertex_t vertexes, Vertex_handle v_preceding, Vertex_handle v_following)
{

}
inline bool ClusterContainer::AddFaces(MultiFace_t faces) {}
#endif // TRIANGULATION_2_EXAMPLES_CLUSTER_H
