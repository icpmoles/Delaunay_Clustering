//
// Created by icpmoles on 06/03/26.
//

#ifndef DELAUNAY_CLUSTERING_SOLUTION_HPP
#define DELAUNAY_CLUSTERING_SOLUTION_HPP

#include <CGAL/Polygon_2.h>
#include "Convexification/utils.hpp"

namespace CC
{
  class Cluster_Solution
  {
  public:
    explicit Cluster_Solution(const uint cluster_id, MultiVertex_t original_full_boundary, const uint solution_id = 0) :
        solution_id_(solution_id), father_cluster_id_(cluster_id), old_boundary_(original_full_boundary)
    {
    }
    Cluster_Solution(Cluster_Solution const& rhs, const int solution_id)
    {
      solution_id_ = solution_id;

      father_cluster_id_ = rhs.father_cluster_id_;
      new_boundary_ = rhs.new_boundary_;
      old_boundary_ = rhs.old_boundary_;
      new_polygon_ = rhs.new_polygon_;
      new_faces_ = rhs.new_faces_;
      new_polygon_ = rhs.new_polygon_;
      area_increment_ = rhs.area_increment_;
      convexity_ = rhs.convexity_;
      convexity_checked_ = rhs.convexity_checked_;
    }
    MultiVertex_t get_new_boundary() { return this->new_boundary_; };
    MultiVertex_t get_old_boundary() { return this->old_boundary_; };
    MultiFace_t get_new_faces() { return this->new_faces_; };
    Polygon get_new_polygon() { return this->new_polygon_; };
    float get_area_increment() const { return this->area_increment_; };
    int get_solution_id() const { return this->solution_id_; };
    int get_cluster_id() const { return this->father_cluster_id_; };
    bool get_convexity()
    {
      if(convexity_checked_)
        return this->convexity_;

      convexity_ = this->new_polygon_.is_convex();
      convexity_checked_ = true;
      return this->convexity_;
    };


  private:
    MultiVertex_t new_boundary_;
    MultiVertex_t old_boundary_;
    MultiFace_t new_faces_;
    Polygon new_polygon_; // what the cluster will look like after application
    float area_increment_ = 0.0; // how much new area this solution brings
    uint solution_id_;
    uint father_cluster_id_;
    bool convexity_ = false;
    bool convexity_checked_ = false;
  };
} // namespace CC
#endif // DELAUNAY_CLUSTERING_SOLUTION_HPP
