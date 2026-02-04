//
// Created by icpmoles on 03/02/26.
//

#ifndef TRIANGULATION_2_EXAMPLES_CLUSTER_H
#define TRIANGULATION_2_EXAMPLES_CLUSTER_H

#include "convexification/Augmented_Mesh.hpp"
#include "convexification/Cluster_Container.hpp"
#include "convexification/utils.hpp"

//
// typedef struct Cluster_t
// {
//   MultiVertex_t vertexes;
//   MultiEdge_t edges;
//   MultiFace_t faces;
//   int cluster_id = 0;
// } Cluster_t;

namespace CM // Cluster
{
  class Cluster_Manager
  {
  public:
    Cluster_Manager(CDT cdt) : mesh_(cdt) {}
    void show_map_(size_t cluster_id);
    void iterate();

    ~Cluster_Manager();

  private:
    AUM::Augmented_Mesh mesh_;
    std::vector<CC::Cluster_Container> clusters_;
  };


  inline void Cluster_Manager::iterate() {}

  inline void Cluster_Manager::show_map_(size_t cluster_id)
  {
    typedef std::unordered_map<Face_handle, bool> FaceOwnershipMap;
    FaceOwnershipMap in_free_space_map;

    for(const Face_handle f : mesh_.get_cdt().all_face_handles())
    {
      if(f->is_in_domain() && mesh_.get_face_description_(f)->Cluster_Id == cluster_id)
      {
        in_free_space_map.insert(std::pair<Face_handle, bool>(f, true));
      }
      else
      {
        in_free_space_map.insert(std::pair<Face_handle, bool>(f, false));
      }
    }
    const boost::associative_property_map<FaceOwnershipMap> in_free_space(in_free_space_map);

    CGAL::draw(mesh_.get_cdt(), in_free_space);
  }

} // namespace CM
#endif // TRIANGULATION_2_EXAMPLES_CLUSTER_H
