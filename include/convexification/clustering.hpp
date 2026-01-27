//
// Created by icpmoles on 21/01/26.
//

#ifndef TRIANGULATION_2_EXAMPLES_CLUSTERING_H
#define TRIANGULATION_2_EXAMPLES_CLUSTERING_H

#include "convexification/utils.hpp"


typedef struct Face_Description
{
  // payload
  size_t Cluster_Id = -1; // id of the cluster

  size_t Face_Id = -1; // id of the face in the vector
  double area = -1.0; // area of the face

  // Check if assigned
  bool area_Calculated = false; // whether the area is calculated
  bool Cluster_Assigned = false; // whether it's assigned to a cluster
  bool Face_Assigned = false; // whether the area is calculated

  // walk metadata

  unsigned Distance_walked = 0; // how many greedy steps without convexity

} Face_Description;

namespace CLS
{


  class Mesh_Augmented
  {
  public:
    /**
     *
     * @param cdt Starting Constrained Delauney Triangulation
     * The New Object of the class will keep a copy inside.
     * It will automatically calculate the area of the internal triangles.
     */
    Mesh_Augmented(CDT cdt)
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

    /**
     * Where the magic happens.
     */
    void iterate();

    /**
     *
     * @param description Struct containing the new description of the i-th face
     * @param i face_id that it's replacing
     * @return true if the face exists, false if there's an error assigning
     */
    bool set_face_description(Face_Description description, size_t i);

  private:
    void add_face_to_cluster_(Face_handle f, size_t i);

    Cluster_t add_vertex_to_cluster_(Cluster_t& cluster, Vertex_handle& v_in, size_t idx_preceding,
                                     size_t idx_following);
    Cluster_t add_vertex_to_cluster_(Cluster_t& cluster, Vertex_handle& v_in, const Vertex_handle& v_preceding,
                                     const Vertex_handle& v_following);

    void add_vertex_to_cluster_(Vertex_handle v1, Vertex_handle v2, Vertex_handle new_vertex, size_t i) const;

    void populate_properties_();

    bool get_inlier_face_(Face_handle& destination);

    static size_t get_vector_idx_(Face_handle f);

    static Face_Description* get_description_(Face_handle f);


    bool is_still_convex(Cluster_t cluster, Vertex_handle v_in, Vertex_handle v_preceding, Vertex_handle v_following);


    CDT cdt_;
    std::vector<Face_Description> Faces_Properties_;
    MultiCluster_t Clusters_;
  };

  inline Face_Description Mesh_Augmented::get_face_description(size_t i) const { return this->Faces_Properties_[i]; }

  inline void Mesh_Augmented::iterate()
  {
    double total_area = 0.0;
    for(const Face_handle f : this->cdt_.finite_face_handles())
    {
      if(f->is_in_domain())
      {
        Face_Description* description = get_description_(f);
        // std::cout << description->Face_Id << "th: " << description->area << std::endl;
        total_area += description->area;
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

    int cluster_id = 0;
    add_face_to_cluster_(first_face, cluster_id);

    int steps = 0;
    int steps_max = 5;
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

  inline bool Mesh_Augmented::set_face_description(Face_Description description, size_t i)
  {
    if(i > Faces_Properties_.size())
      return false;
    Faces_Properties_[i] = description;
    return true;
  }

  inline Cluster_t Mesh_Augmented::add_vertex_to_cluster_(Cluster_t& cluster, Vertex_handle& v_in, size_t idx_preceding,
                                                          size_t idx_following)
  {
  }

  inline Cluster_t Mesh_Augmented::add_vertex_to_cluster_(Cluster_t& cluster, Vertex_handle& v_in,
                                                          const Vertex_handle& v_preceding,
                                                          const Vertex_handle& v_following)
  {
    size_t preceding_idx = -1;
    size_t following_idx = -1;
    size_t counter = 0;
    for(Vertex_handle it : cluster)
    {
      if(v_preceding == it)
      {
        preceding_idx = counter;
      }
      if(v_following == it)
      {
        following_idx = counter;
      }

      counter++;
    }
    return add_vertex_to_cluster_(cluster, v_in, preceding_idx, following_idx);
  };

  inline void Mesh_Augmented::add_face_to_cluster_(const Face_handle f, size_t i)
  {

    Cluster_t cluster;
    if(Clusters_.size() <= i)
    {
      Clusters_.resize(i + 1);
    }
    if(cluster.empty())
    { // if i-th wasn't initialized already:
      cluster.push_back(f->vertex(0));
      cluster.push_back(f->vertex(1));
      cluster.push_back(f->vertex(2));
    }

    Clusters_[i] = std::move(cluster);
  }

  inline void Mesh_Augmented::add_vertex_to_cluster_(const Vertex_handle v1, const Vertex_handle v2,
                                                     const Vertex_handle new_vertex, const size_t i) const
  {
    Cluster_t cluster = Clusters_[i];
    if(cluster.empty())
    { // if i-th wasn't initialized already:
      cluster.push_back(v1);
      cluster.push_back(v2);
      cluster.push_back(new_vertex);
    }
  }

  inline void Mesh_Augmented::populate_properties_()
  {
    const size_t n_faces = cdt_.number_of_faces();
    Faces_Properties_.clear();
    Faces_Properties_.reserve(n_faces);
    size_t i = 0;
    for(const Face_handle f : cdt_.finite_face_handles())
    {
      const double area = UTILS::get_area(f);
      Faces_Properties_.push_back({.Face_Id = i, .area = area, .area_Calculated = true, .Face_Assigned = true});

      f->set_time_stamp(reinterpret_cast<std::size_t>(&Faces_Properties_[i]));

      i++;
    }
  }

  inline bool Mesh_Augmented::get_inlier_face_(Face_handle& destination)
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

  inline size_t Mesh_Augmented::get_vector_idx_(const Face_handle f)
  {
    // std::cout << "address : " << std::to_string(f->time_stamp()) <<std::endl;
    Face_Description* description = get_description_(f);
    return description->Face_Id;
  }

  inline Face_Description* Mesh_Augmented::get_description_(const Face_handle f)
  {
    auto* description = reinterpret_cast<Face_Description*>(f->time_stamp());
    return description;
  }


  bool Mesh_Augmented::is_still_convex(const Cluster_t cluster, Vertex_handle v_in, Vertex_handle v_preceding,
                                       Vertex_handle v_following) {


  };

} // namespace CLS

#endif // TRIANGULATION_2_EXAMPLES_CLUSTERING_H
