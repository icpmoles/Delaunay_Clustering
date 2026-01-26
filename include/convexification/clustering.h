//
// Created by icpmoles on 21/01/26.
//

#ifndef TRIANGULATION_2_EXAMPLES_CLUSTERING_H
#define TRIANGULATION_2_EXAMPLES_CLUSTERING_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Polygon_2.h>

#include "utils.h"


typedef CGAL::Exact_predicates_inexact_constructions_kernel       K; // EPIC
typedef CGAL::Triangulation_vertex_base_2<K>                      Vb;

typedef CGAL::Delaunay_mesh_face_base_2<K>                        Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb>              Tds;

typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds>        CDT;
typedef CGAL::Triangulation_2<K>                                  Triangulation;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDT>                  Criteria;
typedef CDT::Face_handle                                          Face_handle;
typedef CDT::Vertex_handle                                        Vertex_handle;

typedef CDT::Point                                                Point;
typedef CGAL::Polygon_2<K>                                        Polygon;
typedef CGAL::Bbox_2                                              bbox_2;

typedef CGAL::Polygon_with_holes_2<K>                             Polygon_wh;
typedef std::vector<Point>                                        MultiPoint;
typedef std::list<Polygon_wh>                                     Poly_list;

typedef std::vector<Face_handle>                                  Cluster_t;
typedef std::vector<Cluster_t>                                    Clusters_t;

typedef struct {
  // payload
  size_t Cluster_Id =-1;   // id of the cluster
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

    Face_Description get_face_description(size_t i)
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
    }

    bool set_face_description(Face_Description description, size_t i)
    {
      if (i > Faces_Properties_.size())
        return false;
      Faces_Properties_[i] = description;
      return true;
    }

    private:

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
      Clusters_t Clusters_;

  };


}

#endif //TRIANGULATION_2_EXAMPLES_CLUSTERING_H