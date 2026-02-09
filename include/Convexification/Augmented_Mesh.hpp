//
// Created by icpmoles on 21/01/26.
//

#ifndef TRIANGULATION_2_EXAMPLES_CLUSTERING_H
#define TRIANGULATION_2_EXAMPLES_CLUSTERING_H


#include "Convexification/utils.hpp"


namespace AUM // Augmented Mesh
{


  class Augmented_Mesh
  {
  public:
    /**
     *
     * @param cdt Starting Constrained Delauney Triangulation
     * The New Object of the class will keep a copy inside.
     * It will automatically calculate the area of the internal triangles.
     */
    Augmented_Mesh(CDT cdt)
    {
      this->cdt_ = std::move(cdt);
      populate_properties_();
    };

    /**
     *
     * @param i  Index of the face (obtained by get_description(FaceHandle)->Face_Id )
     * @return Face_Description struct
     */
    [[nodiscard]] Face_Description get_face_description(size_t i) const;

    CDT get_cdt() { return this->cdt_; }

    /**
     *
     * @param description Struct containing the new description of the i-th face
     * @param i face_id that it's replacing
     * @return true if the face exists, false if there's an error assigning
     */
    void set_face_description(const Face_Description& description, size_t i);

    /**
     *
     * @param description Struct containing the new description of the i-th face
     * @param f  facehandle of the subject it's replacing
     * @return true if the face exists, false if there's an error assigning
     */
    void set_face_description(const Face_Description& description, Face_handle f);


    Face_Description* get_face_description(Face_handle f);

  private:
    // /**
    //  *
    //  * @param cluster Subject to inflate
    //  * @param cluster_id Cluster to assign it to
    //  * @return A cluster of the inflated region.
    //  */
    // MultiVertex_t inflate_cluster_(MultiVertex_t& cluster, size_t cluster_id);
    // MultiVertex_t insert_cluster_(MultiVertex_t original_cluster, MultiVertex_t insert, Vertex_handle v_preceding,
    //                               Vertex_handle v_following);
    // static MultiVertex_t add_vertex_to_cluster_(MultiVertex_t& cluster, MultiVertex_t v_in, size_t idx_preceding,
    //                                             size_t idx_following);
    // MultiVertex_t add_vertex_to_cluster_(MultiVertex_t& cluster, const Vertex_handle& v_in,
    //                                      const Vertex_handle& v_preceding, const Vertex_handle& v_following);

    // void add_vertex_to_cluster_(Vertex_handle v1, Vertex_handle v2, Vertex_handle new_vertex, size_t i) const;

    void populate_properties_();

    /**
     *
     * @param destination Where to save the Face_Handle if the procedure is successful
     * @return if the call was successful
     */
    bool get_inlier_face_(Face_handle& destination) const;

    size_t get_vector_idx_(Face_handle f);

    void print_face_info(Face_handle f, bool print_vertexes);


    /**
     * Resets the Distance value of the unassigned faces.
     * @return number of faces still not assigned to a cluster
     */
    size_t reset_unassigned_faces();

    CDT cdt_;
    std::vector<Face_Description> Faces_Properties_;
  };

  inline Face_Description Augmented_Mesh::get_face_description(size_t i) const { return this->Faces_Properties_[i]; }

  /*

  inline void Augmented_Mesh::iterate()
  {
    double total_area = 0.0;
    for(const Face_handle f : this->cdt_.finite_face_handles())
    {
      if(f->is_in_domain())
      {
        Face_Description* description = get_face_description_(f);
        // std::cout << description->Face_Id << "th: " << description->area << std::endl;
        // TODO: Iterate edge
        // https://doc.cgal.org/5.6.3/TDS_2/classTriangulationDataStructure__2.html#a35f5c887003a6d08b6cef13228c89bd6
        // somehow
        // TODO: reorient cluster list to start from arbitrary point
        total_area += description->Area;
      }
    }
    std::cout << "total: " << total_area << std::endl;


    // Face_handle first_face = this->cdt_.finite_faces_begin();
    Face_handle first_face;
    if(!get_inlier_face_(first_face))
    {
      std::cout << "Ill formed triangulation. Aborting..." << std::endl;
      return;
    }
    std::cout << "Triangle of interest" << std::endl;
    UTILS::print_triangle_vertices(first_face);


    show_map_(0);
    print_face_info(first_face);


    int cluster_id = 0;
    // add_face_to_cluster_(first_face, cluster_id);
    //
    //
    // show_map_(0);
    // print_face_info(first_face);

    int steps = 0;
    int steps_max = 1;

    do
    {
      std::cout << std::endl << "step: " << steps << std::endl;
      // for(const Vertex_handle v : this->Clusters_[cluster_id])
      for(const Vertex_handle v : cdt_.finite_vertex_handles())
      {
        std::cout << "vertex of interest: " << v->point() << std::endl;
        Face_Circulator first_f = v->incident_faces();
        Face_Circulator circ = first_f;
        int face_counter = 0;
        do
        {
          // std::cout << face_counter << "th incident vertex:" << circ->vertex(1)->point() << std::endl;
          if(is_cluster_inflatable_(facehandle_to_cluster_(circ), cluster_id))
          {
            add_face_to_cluster_(circ, cluster_id);
            cluster_id++;
          }


          // print_face_info(circ, true);
          std::cout << std::endl << std::endl;
          face_counter++;
        }
        while(++circ != first_f);
        std::cout << std::endl << std::endl;
      }
    }
    while(++steps < steps_max);

    for(int i = 0; i < cluster_id; i++)
    {
      std::cout << "Showing seed: " << i << "/" << cluster_id << std::endl;
      show_map_(i);
    }

    do
    {
      for(const Vertex_handle v : this->Clusters_[cluster_id])
      {
        std::cout << "vertex of interest: " << v->point() << std::endl;
        Vertex_Circulator first_vertex = v->incident_vertices();
        Vertex_Circulator circ = first_vertex;
        int vertex_counter = 0;
        do
        {
          std::cout << vertex_counter << "th incident vertex:" << circ->point() << std::endl;
          vertex_counter++;
        }
        while(++circ != first_vertex);
        std::cout << std::endl << std::endl;
      }
    }
    while(++steps < steps_max);

  }
  */
  inline void Augmented_Mesh::set_face_description(const Face_Description& description, size_t i)
  {
    if(i > Faces_Properties_.size())
      // return false;
      Faces_Properties_.at(i) = description;
    // return true;
  }
  inline void Augmented_Mesh::set_face_description(const Face_Description& description, Face_handle f)
  {
    set_face_description(description, get_vector_idx_(f));
  }


  // Cluster_t Augmented_Mesh::facehandle_to_cluster_(Face_handle f, bool CCW)
  // {
  //   int second_element = 1, third_element = 2;
  //   if(!CCW)
  //   {
  //     second_element = 2;
  //     third_element = 1;
  //   }
  //
  //   /* FaceHandle returns the vertexes in CCW order if accessed in sequential form ( 0 -> 1 -> 2 -> 3 -> 4 -> ... )
  //    * in (in modulo 3) ( 0 -> 1 -> 2 )
  //    * But it returns in CW way if accessed in a reverse way ( 0 -> 2 -> 1 )
  //    */
  //   MultiVertex_t new_vertexes{f->vertex(0), f->vertex(second_element), f->vertex(third_element)};
  //   MultiEdge_t new_edges{Edge(f, 0)};
  //
  //   return (struct Cluster_t){new_vertexes, new_edges};
  // }

  //
  // bool Augmented_Mesh::is_cluster_inflatable_(const Cluster_t& cluster, const size_t cluster_id)
  // {
  //   for(const Vertex_handle v : cluster)
  //   {
  //     std::cout << "vertex of interest: " << v->point() << std::endl;
  //     Face_Circulator first_f = v->incident_faces();
  //     Face_Circulator circ = first_f;
  //     int face_counter = 0;
  //     do
  //     {
  //       // std::cout << face_counter << "th incident vertex:" << circ->vertex(1)->point() << std::endl;
  //       print_face_info(circ, true);
  //
  //       /* Checks if all the faces in the immediate neighborhood are:
  //        * 1) inside the domain
  //        * 2) not assigned already
  //        * 3) if assigned already, they must be to the same candidate cluster
  //        */
  //       Face_Description* fd = get_face_description_(circ);
  //       if(((fd->is_Cluster_Assigned == false) || (fd->is_Cluster_Assigned == true && fd->Cluster_Id == cluster_id))
  //       &&
  //          circ->is_in_domain())
  //       {
  //         print_face_info(circ);
  //         std::cout << "Face is a good candidate" << std::endl;
  //       }
  //       else
  //       {
  //         print_face_info(circ);
  //         std::cout << "Face is NOT a good candidate" << std::endl;
  //         return false;
  //       }
  //       // std::cout << std::endl << std::endl;
  //       // face_counter++;
  //     }
  //     while(++circ != first_f);
  //     std::cout << std::endl << std::endl;
  //   }
  //   return true;
  // }
  // inline MultiVertex_t Augmented_Mesh::add_vertex_to_cluster_(MultiVertex_t& cluster, MultiVertex_t v_in,
  //                                                             size_t idx_preceding, size_t idx_following)
  // {
  //   MultiVertex_t new_cluster(cluster);
  //   if(idx_following < idx_preceding) //
  //   {
  //   }
  //   else
  //   {
  //   }
  //   return new_cluster;
  // }

  // inline MultiVertex_t Augmented_Mesh::add_vertex_to_cluster_(MultiVertex_t& cluster, const Vertex_handle& v_in,
  //                                                             const Vertex_handle& v_preceding,
  //                                                             const Vertex_handle& v_following)
  // {
  //   size_t preceding_idx = -1;
  //   size_t following_idx = -1;
  //   size_t counter = 0;
  //   for(Vertex_handle it : cluster)
  //   {
  //     if(v_preceding == it)
  //     {
  //       preceding_idx = counter;
  //     }
  //     if(v_following == it)
  //     {
  //       following_idx = counter;
  //     }
  //
  //     counter++;
  //   }
  //   return add_vertex_to_cluster_(cluster, MultiVertex_t{v_in}, preceding_idx, following_idx);
  // };

  // inline void Mesh_Augmented::add_vertex_to_cluster_(const Vertex_handle v1, const Vertex_handle v2,
  //                                                    const Vertex_handle new_vertex, const size_t i) const
  // {
  //   MultiVertex_t cluster = Clusters_.at(i);
  //   if(cluster.empty())
  //   { // if i-th wasn't initialized already:
  //     cluster.push_back(v1);
  //     cluster.push_back(v2);
  //     cluster.push_back(new_vertex);
  //   }
  // }
  // inline void Mesh_Augmented::add_face_to_cluster_(const Face_handle f, size_t i)
  // {
  //
  //   Cluster cluster;
  //   if(Clusters_.size() <= i)
  //   {
  //     Clusters_.resize(i + 1);
  //   }
  //   if(cluster.empty())
  //   { // if i-th wasn't initialized already:
  //     cluster = facehandle_to_cluster_(f);
  //   }
  //
  //   Clusters_.at(i) = std::move(cluster);
  //   get_face_description_(f)->Cluster_Id = i;
  //   get_face_description_(f)->is_Cluster_Assigned = true;
  //   get_face_description_(f)->Distance = 0;
  // }


  inline void Augmented_Mesh::populate_properties_()
  {
    const size_t n_faces = cdt_.number_of_faces();
    Faces_Properties_.clear();
    Faces_Properties_.reserve(n_faces);
    size_t i = 0;
    for(const Face_handle f : this->cdt_.finite_face_handles())
    {
      Faces_Properties_.push_back({.Face_Id = i,
                                   .Area = UTILS::get_area(f),
                                   .is_Area_Calculated = true,
                                   .is_Face_Assigned = true,
                                   .Distance = f->is_in_domain() ? UNEXPLORED_VALUE : OBSTACLE_VALUE});

      f->set_time_stamp(i);

      i++;
    }
  }

  inline bool Augmented_Mesh::get_inlier_face_(Face_handle& destination) const
  {
    Face_handle tentative = this->cdt_.finite_faces_begin();
    const Face_handle last = this->cdt_.finite_faces_end();

    do
    {
      if(tentative->is_in_domain())
      {
        destination = tentative;
        return true;
      }
    }
    while(++tentative != last);
    return false;
  }

  inline size_t Augmented_Mesh::get_vector_idx_(const Face_handle f) { return get_face_description(f)->Face_Id; }

  inline Face_Description* Augmented_Mesh::get_face_description(const Face_handle f)
  {
    const size_t i = f->time_stamp();
    return &Faces_Properties_[i];
  }


  // bool Augmented_Mesh::is_still_convex_(Cluster_t cluster, MultiEdge_t edges_in, Vertex_handle v_preceding,
  //                                       Vertex_handle v_following) {
  //
  //
  // };


  // inline void Augmented_Mesh::show_map_(size_t cluster_id)
  // {
  //   typedef std::unordered_map<Face_handle, bool> FaceOwnershipMap;
  //   FaceOwnershipMap in_free_space_map;
  //
  //   for(const Face_handle f : cdt_.all_face_handles())
  //   {
  //     if(f->is_in_domain() && get_face_description_(f)->Cluster_Id == cluster_id)
  //     {
  //       in_free_space_map.insert(std::pair<Face_handle, bool>(f, true));
  //     }
  //     else
  //     {
  //       in_free_space_map.insert(std::pair<Face_handle, bool>(f, false));
  //     }
  //   }
  //   const boost::associative_property_map<FaceOwnershipMap> in_free_space(in_free_space_map);
  //
  //   CGAL::draw(cdt_, in_free_space);
  // }

  inline void Augmented_Mesh::print_face_info(const Face_handle f, bool print_vertexes)
  {
    Face_Description* fd = get_face_description(f);
    std::cout << "Face_Id: " << fd->Face_Id << " (" << fd->is_Face_Assigned << ")" << std::endl;
    std::cout << "Cluster_Id: " << fd->Cluster_Id << " (" << fd->is_Cluster_Assigned << ")" << std::endl;
    std::cout << "Area: " << fd->Area << " (" << fd->is_Area_Calculated << ")" << std::endl;
    std::cout << "Distance: " << fd->Distance << " (" << f->is_in_domain() << ")" << std::endl;
    if(print_vertexes)
      UTILS::print_triangle_vertices(f);
  }

  inline size_t Augmented_Mesh::reset_unassigned_faces()
  {
    size_t i = 0;
    for(const Face_handle f : this->cdt_.finite_face_handles())
    {
      if(get_face_description(f)->is_Cluster_Assigned == false)
      {
        i++;
        get_face_description(f)->Distance = f->is_in_domain() ? UNEXPLORED_VALUE : OBSTACLE_VALUE;
      }
    }

    return i;
  }

} // namespace AUM

#endif // TRIANGULATION_2_EXAMPLES_CLUSTERING_H
