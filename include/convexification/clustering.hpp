//
// Created by icpmoles on 21/01/26.
//

#ifndef TRIANGULATION_2_EXAMPLES_CLUSTERING_H
#define TRIANGULATION_2_EXAMPLES_CLUSTERING_H

#include "convexification/utils.hpp"
#include "preprocessing.hpp"


typedef struct Face_Description {
  // payload
  size_t Cluster_Id =-1;   // id of the cluster

  size_t Face_Id =-1; // id of the face in the vector
  double area=-1.0;                    // area of the face

  // Check if assigned
  bool area_Calculated=false;          // wheter the area is calculated
  bool Cluster_Assigned=false;              // wheter it's assigned to a cluster
  bool Face_Assigned=false;          // wheter the area is calculated

  // walk metadata

  unsigned Distance_walked = 0;  //how many greedy steps without convexity

} Face_Description;

namespace CLS
{


  class Mesh_Augmented
  {
    public:
    Mesh_Augmented(CDT cdt)
    {
      this->cdt_ = std::move(cdt);
      populate_properties_();
    };

    Face_Description get_face_description(size_t i) const;


    void iterate();

    bool set_face_description(Face_Description description, size_t i);

  private:

    void add_face_to_cluster_(const Face_handle f, size_t i);
    void add_vertex_to_cluster_(Vertex_handle v1, Vertex_handle v2, Vertex_handle new_vertex, size_t i);

    void populate_properties_();

    static size_t get_vector_idx_(const Face_handle f);

    static Face_Description* get_description(const Face_handle f);


    CDT cdt_;
      std::vector<Face_Description> Faces_Properties_;
      MultiCluster_t Clusters_;

  };

  inline Face_Description Mesh_Augmented::get_face_description(size_t i) const
  {
    return this->Faces_Properties_[i];
  }

  void Mesh_Augmented::iterate()
  {
    double total_area = 0.0;
    for (const Face_handle f : this->cdt_.finite_face_handles())
    {
      if (f->is_in_domain())
      {
        Face_Description* description = get_description(f);
        std::cout << description->Face_Id << "th: " << description->area << std::endl;
        total_area += description->area;

      }
    }
    std::cout << "total: " << total_area << std::endl;


    Face_handle first = this->cdt_.finite_faces_begin();

    int cluster_id = 0;
    add_face_to_cluster_(first, cluster_id);

    int steps = 0;
    int steps_max = 5;
    while (steps < steps_max)
    {
      for (const Vertex_handle v : this->Clusters_[cluster_id])
      {
        auto s = v->incident_vertices(first);
        // TODO: Figure out how circulators work
        // Circulator c;
        //
        // CGAL::Triangulation_vertex_base_2<K,CGAL::Triangulation_ds_vertex_base_2<Tds>>::Vertex_circulator container(s);
        // container.
        // for (auto i=container.begin(); i<container.end(); i++ )
        // {
        //
        // }
      }
    }


  }

  inline bool Mesh_Augmented::set_face_description(Face_Description description, size_t i)
  {
    if (i > Faces_Properties_.size())
      return false;
    Faces_Properties_[i] = description;
    return true;
  }

  inline void Mesh_Augmented::add_face_to_cluster_(const Face_handle f, size_t i)
  {
    Cluster_t cluster = Clusters_[i];
    if (cluster.empty()){ // if i-th wasn't initialized already:
      cluster.push_back(f->vertex(0));
      cluster.push_back(f->vertex(1));
      cluster.push_back(f->vertex(2));

    }
  }

  inline void Mesh_Augmented::add_vertex_to_cluster_(const Vertex_handle v1, const Vertex_handle v2,
                                                                                  const Vertex_handle new_vertex, size_t i)
  {
    Cluster_t cluster = Clusters_[i];
    if (cluster.empty()){ // if i-th wasn't initialized already:
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
    for (const Face_handle f : cdt_.finite_face_handles())
    {
      const double area = UTILS::get_area(f);
      Faces_Properties_.push_back({.Face_Id = i, .area = area, .area_Calculated = true, .Face_Assigned = true});

      f->set_time_stamp(reinterpret_cast<std::size_t>(&Faces_Properties_[i]));

      i++;
    }
  }

  inline size_t Mesh_Augmented::get_vector_idx_(const Face_handle f)
  {
    // std::cout << "address : " << std::to_string(f->time_stamp()) <<std::endl;
    Face_Description* description = get_description(f);
    return description->Face_Id;
  }

  inline Face_Description* Mesh_Augmented::get_description(const Face_handle f)
  {
    auto* description = reinterpret_cast<Face_Description*>(f->time_stamp());
    return description;
  }
}

#endif //TRIANGULATION_2_EXAMPLES_CLUSTERING_H