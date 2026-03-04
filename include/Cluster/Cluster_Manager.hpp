//
// Created by icpmoles on 03/02/26.
//

#ifndef TRIANGULATION_2_EXAMPLES_CLUSTER_H
#define TRIANGULATION_2_EXAMPLES_CLUSTER_H

#include "Cluster/Cluster.hpp"
#include "Convexification/Augmented_Mesh.hpp"
#include "Convexification/utils.hpp"

//
// typedef struct Cluster_t
// {
//   MultiVertex_t vertexes;
//   MultiEdge_t edges;
//   MultiFace_t faces;
//   int cluster_id = 0;
// } Cluster_t;

namespace CM // Cluster Manager
{
  class Cluster_Manager
  {
  public:
    /**
     *
     * @param cdt Constrained Delauney Triangulation to use as a base for the new Structure
     */
    Cluster_Manager(CDT cdt) : mesh_(cdt)
    {
      std::cout << "PRINTING AT CREATION" << std::endl;
      CGAL::draw(*mesh_.get_cdt_ptr());
    }
    void show_map(size_t cluster_id);
    void show_map();
    void iterate();

    ~Cluster_Manager() = default;

  private:
    AUM::Augmented_Mesh mesh_;
    std::vector<CC::Cluster> clusters_;
  };


  inline void Cluster_Manager::iterate()
  {
    show_map();
    Face_handle destination;
    if (mesh_.get_inlier_face_(destination))
    {
      std::cout << "Found inlier" << std::endl;
      UTILS::print_triangle_vertices(destination);
      CC::Cluster cluster(destination, &mesh_, 0);
      Face_Description* fd = mesh_.get_face_description(destination);
      UTILS::print_face_description(*fd);
    };
  }

  inline void Cluster_Manager::show_map()
  {
    std::cout << "SHOWING MAP PTR" << std::endl;
    CGAL::draw(*mesh_.get_cdt_ptr());

    // NOTE: draw with copy of cdt is bugged, not sure if from my end or from the library
    // std::cout << "SHOWING MAP COPY" << std::endl;
    // CGAL::draw(mesh_.get_cdt());
  }

  inline void Cluster_Manager::show_map(size_t cluster_id)
  {
    typedef std::unordered_map<Face_handle, bool> FaceOwnershipMap;
    FaceOwnershipMap in_free_space_map;
    int counter = 0;
    int total_counter = 0;
    for(const Face_handle f : mesh_.get_cdt_ptr()->all_face_handles())
    { //  &&
      if(f->is_in_domain() && mesh_.get_face_description(f)->Cluster_Id == cluster_id)
      {
        std::cout << "show_map: Found inlier n°" << counter<< std::endl;

        UTILS::print_triangle_vertices(f);
        UTILS::print_face_description(*mesh_.get_face_description(f));
        in_free_space_map.insert(std::pair<Face_handle, bool>(f, true));
        counter++;

      }
      else
      {
        in_free_space_map.insert(std::pair<Face_handle, bool>(f, false));
      }
      total_counter++;
    }
    const boost::associative_property_map<FaceOwnershipMap> in_free_space(in_free_space_map);
    std::cout << "PRINTING WITH CLST ID: "<< cluster_id << ", MATCHING:" << counter << "/" << total_counter << std::endl;

    CGAL::draw(*mesh_.get_cdt_ptr(), in_free_space);
  }

} // namespace CM
#endif // TRIANGULATION_2_EXAMPLES_CLUSTER_H
