//
// Created by icpmoles on 12/03/26.
//

#ifndef DELAUNAY_CLUSTERING_DUALGRAPH_HPP
#define DELAUNAY_CLUSTERING_DUALGRAPH_HPP

#include "Convexification//utils.hpp"

struct NodeDescriptor_t
{
  // n.b. nodes id in the chaco format start from 1. Zero means uninitialized
  size_t Node_id = 0; // == Face_id
  bool use_node_cost = false;
  bool use_edge_cost = false;
  float Node_cost = 0.0;
  std::vector<float> Edge_costs = {0.0, 0.0, 0.0};
  std::vector<size_t> Neighbour_nodes = {0, 0, 0};
  Face_handle * fh_ptr;
}; // Faces of the triangulation become nodes of the dual graph

namespace DG
{
  class DualGraph
  {
  public:
    DualGraph(AUM::Augmented_Mesh mesh);

  private:
    AUM::Augmented_Mesh* mesh_ptr;
    std::vector<NodeDescriptor_t> nodes_;
  };
  inline DualGraph::DualGraph(AUM::Augmented_Mesh mesh)
  {
    std::cout << "Initializing Dual Graph" << std::endl;
    mesh_ptr = &mesh;

    // Fill First
    Face_handle iterator = this->mesh_ptr->get_cdt_faces_begin();
    const Face_handle last = this->mesh_ptr->get_cdt_faces_end();

    int idx = 0;
    bool success = true;
    do
    {
      Face_Description* desc = this->mesh_ptr->get_face_description(iterator);
      if(desc->is_InDomain)
      {
        std::vector<Face_Description*> nb = this->mesh_ptr->get_face_neighbours(iterator);
        NodeDescriptor_t nd{
          .Node_id = desc->Face_Id,
          .use_node_cost = false,
          .use_edge_cost = false,
          .Node_cost = 0.0,
          .Edge_costs = {0.0, 0.0, 0.0},
          .Neighbour_nodes = {0, 0, 0},
          .fh_ptr = &(iterator),
        };
        for(int i = 0; i < nb.size(); ++i)
        {
          int good_neigbour = 0;
          if(nb.at(i) != nullptr)
          {
            nd.Neighbour_nodes.at(good_neigbour) = nb.at(i)->Face_Id;
          }
        }
        nodes_.push_back(nd);

        if(desc->Face_Id != idx)
        {
          // std::cout << "Dual Graph: skipped counter";
          success = false;
        }

        idx++;
      }
      // else
      // {
      //   success = false;
      //   break;
      // }
    }
    while(++iterator != last);

    std::cout << "Dual Graph: Success = " << success << std::endl;
    std::cout << "Dual Graph: Saved " << idx << " nodes" << std::endl;

    for(auto i: nodes_)
    {

    }
  }
} // namespace DG

#endif // DELAUNAY_CLUSTERING_DUALGRAPH_HPP
