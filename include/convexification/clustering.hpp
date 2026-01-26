//
// Created by icpmoles on 21/01/26.
//

#ifndef TRIANGULATION_2_EXAMPLES_CLUSTERING_H
#define TRIANGULATION_2_EXAMPLES_CLUSTERING_H

#include "convexification/utils.hpp"


typedef struct Face_Description {
  // payload
  size_t Cluster_Id =-1;   // id of the cluster
  int Distance_walked = 0;
  size_t Face_Id =-1; // id of the face in the vector
  double area=-1.0;                    // area of the face

  // Check if assigned
  bool area_Calculated=false;          // wheter the area is calculated
  bool Cluster_Assigned=false;              // wheter it's assigned to a cluster
  bool Face_Assigned=false;          // wheter the area is calculated


} Face_Description;

namespace CLS
{


  class Constrained_Delaunay_Triangulation_with_Info
  {
    public:
    Constrained_Delaunay_Triangulation_with_Info(CDT cdt)
    {
      this->cdt_ = std::move(cdt);
      populate_properties_();
    };

    Face_Description get_face_description(size_t i) const
    {
      return this->Faces_Properties_[i];
    }



    void iterate()
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













    }

    bool set_face_description(Face_Description description, size_t i)
    {
      if (i > Faces_Properties_.size())
        return false;
      Faces_Properties_[i] = description;
      return true;
    }

    private:

      void add_face_to_cluster(Face_handle f, size_t i)
      {
        if (Clusters_[i].size() == 0){ // if i-th wasn't initialized already


        }
      }

      void populate_properties_()
      {
        const size_t n_faces = cdt_.number_of_faces();
        Faces_Properties_.clear();
        Faces_Properties_.reserve(n_faces);
        size_t i = 0;
        for (const Face_handle f : cdt_.finite_face_handles())
        {
          const double area = UTILS::get_area(f);
          Faces_Properties_.push_back({ .Face_Id = i, .area=area, .area_Calculated = true, .Face_Assigned=true });

          f->set_time_stamp(reinterpret_cast<std::size_t>(&Faces_Properties_[i]));

          i++;
        }

      };

      static size_t get_vector_idx_(const Face_handle f)
      {
        // std::cout << "address : " << std::to_string(f->time_stamp()) <<std::endl;
        Face_Description* description = get_description(f);
        return description->Face_Id;
      }

      static Face_Description* get_description( Face_handle f)
      {
        auto* description = reinterpret_cast<Face_Description*>(f->time_stamp());
        return description;
      }


      CDT cdt_;
      std::vector<Face_Description> Faces_Properties_;
      MultiCluster_t Clusters_;

  };


}

#endif //TRIANGULATION_2_EXAMPLES_CLUSTERING_H