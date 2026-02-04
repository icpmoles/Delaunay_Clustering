//
// Created by icpmoles on 04/02/26.
//

#ifndef TRIANGULATION_2_EXAMPLES_CLUSTER_CONTAINER_HPP
#define TRIANGULATION_2_EXAMPLES_CLUSTER_CONTAINER_HPP
#include "convexification/utils.hpp"


typedef struct Cluster_t
{
  MultiVertex_t vertexes;
  MultiEdge_t edges;
  MultiFace_t faces;
  bool success;
} Cluster_t;


namespace CC
{
  class Cluster_Container
  {
  public:
    Cluster_Container(Face_handle f);
    int LocateVertex(Vertex_handle v);
    bool ShiftStart(Vertex_handle v_start);
    bool Commit(Cluster_t new_cluster);
    Cluster_t AddEdges(MultiEdge_t edges, Vertex_handle v_preceding, Vertex_handle v_following);
    Cluster_t AddVertexes(MultiVertex_t vertexes, Vertex_handle v_preceding, Vertex_handle v_following);
    Cluster_t AddFaces(MultiFace_t faces);
    Cluster_t Inflate();

  private:
    MultiVertex_t vertexes_;
    MultiEdge_t edges_;
    MultiFace_t faces_;
    int cluster_id = 0;
    Polygon perimeter_;
  };

  inline bool Cluster_Container::ShiftStart(Vertex_handle v_start)
  {
    int v_idx_start = LocateVertex(v_start);
    if(v_idx_start == -1)
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
    new_vertexes.insert(new_vertexes.begin(), pos_vertex, vertexes_.end());
    new_vertexes.insert(new_vertexes.end(), vertexes_.begin(), std::prev(pos_vertex));
    vertexes_ = new_vertexes;

    // reformat edges list
    MultiEdge_t new_edges;
    auto pos_edge = std::next(edges_.begin(), v_idx_start);
    new_edges.insert(new_edges.begin(), pos_edge, edges_.end());
    new_edges.insert(new_edges.end(), edges_.begin(), std::prev(pos_edge));
    edges_ = new_edges;

    assert(vertexes_[0] == v_start);
    assert(edges_[0].first->vertex(ccw(edges_[0].second)) == v_start);
    return true;
  }

  inline bool Cluster_Container::Commit(Cluster_t new_cluster) {}
  inline Cluster_t Cluster_Container::AddVertexes(MultiVertex_t vertexes, Vertex_handle v_preceding,
                                                  Vertex_handle v_following)
  {
  }
  inline Cluster_t Cluster_Container::AddFaces(MultiFace_t faces) {}
  inline Cluster_Container::Cluster_Container(Face_handle f)
  {
    perimeter_.push_back(f->vertex(0)->point());
    perimeter_.push_back(f->vertex(1)->point());
    perimeter_.push_back(f->vertex(2)->point());
    assert(perimeter_.is_counterclockwise_oriented);
  }
  // {
  //   // commit face
  //   faces_.push_back(f);
  //
  //
  //   // commit vertexes
  //   vertexes_.push_back(f->vertex(0));
  //   vertexes_.push_back(f->vertex(1));
  //   vertexes_.push_back(f->vertex(2));
  //   // commit edges:
  //   edges_.push_back(Edge(f, 2));
  //   edges_.push_back(Edge(f, 0));
  //   edges_.push_back(Edge(f, 1));
  //
  //   //
  //   //        V0
  //   //       /  \
  //   //      /    \
  //   //  E0 /      \ E2
  //   //    /        \
  //   //   /          \
  //   // V1────────────V2
  //   //        E1
  //   // i-th edge connects the i.th vertex to the (i+1).th vertex
  //   //
  //
  //   // 0-th vertex is opposite to the 1st edge
  //   assert(vertexes_[0] == f->vertex(edges_[1].second));
  //   // 1st vertex is opposite to the 2nd edge
  //   assert(vertexes_[1] == f->vertex(edges_[2].second));
  //   // 2nd vertex is opposite to the 0th edge
  //   assert(vertexes_[2] == f->vertex(edges_[0].second));
  // }
  inline int Cluster_Container::LocateVertex(Vertex_handle v)
  {
    for(int i = 0; i < vertexes_.size(); i++)
    {
      if(vertexes_[i] == v)
        return i;
    }
    return -1;
  }
  inline Cluster_t Cluster_Container::Inflate() {}
  inline Cluster_t Cluster_Container::AddEdges(MultiEdge_t edges, Vertex_handle v_preceding, Vertex_handle v_following)
  {
    int idx_start = LocateVertex(v_preceding);
    int idx_end = LocateVertex(v_following);

    if(idx_end == -1 || idx_start == -1)
    {
      return {{}, {}, {}, false};
    }

    if(idx_start > idx_end)
    {
      ShiftStart(v_preceding);
    }
  }
} // namespace CC
#endif // TRIANGULATION_2_EXAMPLES_CLUSTER_CONTAINER_HPP
