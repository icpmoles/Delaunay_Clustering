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
     * @param show_plots Wheter to show plots or not
     */
    Cluster_Manager(CDT cdt, bool show_plots = false) : mesh_(cdt) { show_plots_ = show_plots; }
    void show_map(size_t cluster_id);
    void show_map();
    void iterate();

    ~Cluster_Manager() = default;
    bool get_show_plots() { return show_plots_; }
    void set_show_plots(bool show_plots) { show_plots_ = show_plots; }

  private:
    AUM::Augmented_Mesh mesh_;
    std::vector<CC::Cluster> clusters_;
    bool show_plots_ = false;
  };


  inline void Cluster_Manager::iterate()
  {
    Face_handle seed_face;
    int n_clusters = 0; // clusters are numbered sequentially
    if(mesh_.get_inlier_face_(seed_face))
    {
      std::cout << "Found inlier" << std::endl;
      UTILS::print_triangle_vertices(seed_face);
      CC::Cluster cluster_0(seed_face, &mesh_, n_clusters++);
      // assert(mesh_.get_face_description(seed_face)->Cluster_Id == n_clusters);
      std::cout << std::endl;

      if(cluster_0.refresh_free_boundaries())
      {
        std::cout << "Cluster has free boundaries" << std::endl;
        std::vector<Boundary_t> list_boundaries = cluster_0.get_free_boundaries();
        int boundary_index = 0;
        for(auto boundary : list_boundaries)
        {
          std::cout << boundary_index << "): ";
          for(auto vertex : boundary.first)
          {
            std::cout << *vertex << " -> ";
          }
          boundary_index++;
          std::cout << std::endl;
        }
      }

#ifndef NDEBUG
      CC::Cluster_circulator cursor_c_v_print;
      const CC::Cluster_circulator start_c_v_print = cursor_c_v_print =
        cluster_0.vertices_circulator(); //  cluster's vertexes cursor/start handles
      std::cout << "======== Testing Circulator" << std::endl;
      do
      {

        UTILS::print_vertex(*cursor_c_v_print);
        std::cout << " 's next is: ";
        UTILS::print_vertex(cursor_c_v_print.get_next());
        std::cout << " , test if modified: ";

        UTILS::print_vertex(*cursor_c_v_print);

        std::cout << std::endl;
      }
      while(++cursor_c_v_print != start_c_v_print);
#endif

      CC::Cluster_circulator cursor_c_v;
      const CC::Cluster_circulator start_c_v = cursor_c_v =
        cluster_0.vertices_circulator(); //  cluster's vertexes cursor/start handles

      int cluster_el_counter = 0;
      bool all_neighbours_suitable = true;

      do //  loop through all vertexes of cluster
      {
        // assert(seed_face->vertex(cluster_el_counter)==cursor_c_v.get_vertex());
        Vertex_handle vertex_ = cursor_c_v.get_vertex();
        Vertex_handle next_vertex_ = cursor_c_v.get_next();
        assert(vertex_ != next_vertex_);
        TDS_Vertex_Circulator cursor_f_v, start_f_v; //  face's vertexes cursor/start handles
        cursor_f_v = start_f_v = vertex_->incident_vertices();
        int neighbour_el_counter = 0;
        int cluster_vertex_has_suitable_neighbour = false;
        do // loop through all vertex neighbours
        {
          bool is_suitable = cluster_0.is_insertable(next_vertex_, vertex_, cursor_f_v);
          UTILS::print_vertex(next_vertex_);
          std::cout << " && ";
          UTILS::print_vertex(vertex_);
          std::cout << " (cls:" << cluster_el_counter << "/neig:" << neighbour_el_counter << ") <--"
                    << (is_suitable ? "-" : "x") << "--> ";
          UTILS::print_vertex(cursor_f_v);
          std::cout << std::endl;
          if(is_suitable)
          {
            Polygon new_polygon = cluster_0.get_future_polygon(vertex_, cursor_f_v);
            std::cout << "New Polygon: " << new_polygon;
            if(show_plots_)
              CGAL::draw(new_polygon);
            cluster_vertex_has_suitable_neighbour = true;
          }
          std::cout << std::endl << std::endl;
          neighbour_el_counter++;
        }
        while(++cursor_f_v != start_f_v);
        if(!cluster_vertex_has_suitable_neighbour)
        {
          all_neighbours_suitable = false;
        }

        cluster_el_counter++;
      }
      while(++cursor_c_v != start_c_v);

      if(all_neighbours_suitable)
      {
        std::cout << "==== Inflation Possible" << std::endl;
      }
    };

    std::cout << "Generated a total of " << n_clusters << " clusters" << std::endl;
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
        std::cout << "show_map: Found inlier n°" << counter << std::endl;

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
    std::cout << "PRINTING WITH CLST_ID: " << cluster_id << ", MATCHING:" << counter << "/" << total_counter
              << std::endl;

    CGAL::draw(*mesh_.get_cdt_ptr(), in_free_space);
  }

} // namespace CM
#endif // TRIANGULATION_2_EXAMPLES_CLUSTER_H
