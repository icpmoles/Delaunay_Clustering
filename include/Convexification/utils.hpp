//
// Created by icpmoles on 21/01/26.
//

#ifndef TRIANGULATION_2_EXAMPLES_UTILS_H
#define TRIANGULATION_2_EXAMPLES_UTILS_H

#include <parmetis.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/WKT.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/draw_polygon_2.h>
// #include <CGAL/draw_polygon_set_2.h>
// #include <CGAL/Polygon_set_2.h>
#include <CGAL/Polyline_simplification_2/simplify.h>
#include <CGAL/Triangulation_2.h>
#include <CGAL/circulator.h>
#include <CGAL/draw_polygon_with_holes_2.h>
#include <CGAL/draw_triangulation_2.h>
#include <CGAL/mark_domain_in_triangulation.h>
#include <CGAL/min_quadrilateral_2.h>

#include <CGAL/circulator.h>
#include <algorithm>
#include <cassert>
#include <vector>

#include <boost/property_map/property_map.hpp>
#include <iostream>
#include <unordered_map>


#include <fstream>
#include <iostream>

#define OBSTACLE_VALUE UINT_MAX
#define UNEXPLORED_VALUE (UINT_MAX / 2)

namespace PS = CGAL::Polyline_simplification_2;

typedef CGAL::Exact_predicates_inexact_constructions_kernel K; // EPIC
typedef CGAL::Triangulation_vertex_base_2<K> Vb;

typedef CGAL::Delaunay_mesh_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;

typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds> CDT;
typedef CGAL::Triangulation_2<K, Tds> Triangulation;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDT> Criteria;
typedef CDT::Face_handle Face_handle;
typedef CDT::Vertex_handle Vertex_handle;
typedef CDT::Edge Edge;


typedef CDT::Point Point;
typedef CGAL::Polygon_2<K> Polygon;
// typedef CGAL::Polygon_set_2<K> Polygon_set;
typedef CGAL::Bbox_2 bbox_2;

typedef CGAL::Polygon_with_holes_2<K> Polygon_wh;
typedef std::vector<Point> MultiPoint;
typedef std::list<Polygon_wh> Poly_list;

typedef PS::Stop_below_count_ratio_threshold Stop;
typedef PS::Squared_distance_cost Cost;


typedef PS::Stop_below_count_ratio_threshold Stop;
typedef PS::Squared_distance_cost Cost;


/**
 * List of vertexes (in CCW order)
 */
typedef std::vector<Vertex_handle> MultiVertex_t;

/**
 * List of Edges (in CCW order)
 */
typedef std::vector<Edge> MultiEdge_t;


/**
 * List of Edges (in CCW order)
 */
typedef std::vector<Face_handle> MultiFace_t;
typedef CGAL::Triangulation_ds_face_circulator_2<Tds> Face_Circulator;
typedef CGAL::Container_from_circulator<Face_Circulator> Face_Container;

typedef CGAL::Triangulation_ds_edge_circulator_2<Tds> Edge_Circulator;
typedef CGAL::Container_from_circulator<Edge_Circulator> Edge_Container;

typedef CGAL::Triangulation_ds_vertex_circulator_2<Tds> TDS_Vertex_Circulator;
typedef CGAL::Container_from_circulator<TDS_Vertex_Circulator> Vertex_Container;
typedef Vertex_Container::iterator Vertex_Iterator;

typedef std::unordered_map<Face_handle, bool> BooleanFaceMap;
typedef std::unordered_map<Face_handle, double> AreaFaceMap;

typedef uint BoundaryId_t;
typedef std::pair<MultiVertex_t, BoundaryId_t> Boundary_t;

typedef struct Face_Description
{
  // payload
  size_t Cluster_Id = -1; // id of the cluster

  size_t Face_Id = -1; // id of the face in the vector
  double Area = -1.0; // area of the face

  // Check if assigned
  bool is_Area_Calculated = false; // whether the area is calculated
  bool is_Cluster_Assigned = false; // whether it's assigned to a cluster
  bool is_Face_Assigned = false; // whether the Face_Id is assigned

  bool is_InDomain = false; // whether the Face is not an obstacle or outside the perimeter


  // walk metadata

  unsigned Distance = UNEXPLORED_VALUE; // OBSTACLE_VALUE / UNEXPLORED_VALUE depending on the value

} Face_Description;

namespace UTILS
{

  void get_stats(const CDT& triangulation, const boost::associative_property_map<BooleanFaceMap> map)
  {
    int face_count = 0;
    int indomain_face_count = 0;
    for(Face_handle f : triangulation.finite_face_handles())
    {
      if(get(map, f))
        ++indomain_face_count;
      ++face_count;
    }
    std::cout << std::endl << std::endl << "polygon indomain faces: " << indomain_face_count << std::endl;
    std::cout << "polygon total faces: " << face_count << std::endl << std::endl;
  }

  void get_stats(const CDT& triangulation)
  {
    int face_count = 0;
    int indomain_face_count = 0;
    for(Face_handle f : triangulation.finite_face_handles())
    {
      if(f->is_in_domain())
        ++indomain_face_count;
      ++face_count;
    }
    std::cout << std::endl << std::endl << "polygon indomain faces (w/o map): " << indomain_face_count << std::endl;
    std::cout << "polygon total faces (w/o map): " << face_count << std::endl << std::endl;
  }

  inline double get_area(Face_handle f)
  {
    Polygon polygon;
    polygon.push_back(f->vertex(0)->point());
    polygon.push_back(f->vertex(1)->point());
    polygon.push_back(f->vertex(2)->point());

    return polygon.area();
  }

  inline void print_vertex(const Vertex_handle v) { std::cout << v->point(); }

  inline void print_triangle_vertices(const Face_handle f)
  {
    std::cout << "1st Vertex " << f->vertex(0)->point() << std::endl;
    std::cout << "2nd Vertex " << f->vertex(1)->point() << std::endl;
    std::cout << "3rd Vertex " << f->vertex(2)->point() << std::endl;
  }

  inline void print_face_description(const Face_Description& fd)
  {
    std::cout << "ClusterID " << fd.Cluster_Id << " [" << (fd.is_Cluster_Assigned ? "v" : "x") << "] " << std::endl;
    std::cout << "FaceId " << fd.Face_Id << " [" << (fd.is_Face_Assigned ? "v" : "x") << "] " << std::endl;
    std::cout << "Area " << fd.Area << " [" << (fd.is_Area_Calculated ? "v" : "x") << "] " << std::endl;
    std::cout << "Distance " << fd.Distance << std::endl;
  }


  /**
   *
   * @param f Face of interest
   * @param v1 Vertex
   * @return True if v1 is a vertex of f
   */
  inline bool belong_to_face(const Face_handle f, const Vertex_handle v1)
  {
    if(f->vertex(0) == v1 || f->vertex(1) == v1 || f->vertex(2) == v1)
      return true;
    else
      return false;
  }

  /**
   *
   * @param f Face of interesr
   * @param v1 Vertex
   * @param v2 Vertex
   * @return True if v2 is in CCW order compared to v1 and are the vertexes of f
   */
  inline bool belong_to_face(Face_handle f, Vertex_handle v1, Vertex_handle v2)
  {
    if(f->vertex(0) == v1 && f->vertex(1) == v2)
      return true;
    if(f->vertex(1) == v1 && f->vertex(2) == v2)
      return true;
    if(f->vertex(2) == v1 && f->vertex(0) == v2)
      return true;
    return false;
  }

  /**
   *
   * @param f Face of interest
   * @param v1 Vertex
   * @param v2 Vertex
   * @param v3 Vertex
   * @return True if v1 v2 and v3 are in CCW order and are the vertexes of f
   */
  inline bool belong_to_face(Face_handle f, Vertex_handle v1, Vertex_handle v2, Vertex_handle v3)
  {
    if(f->vertex(0) == v1 && f->vertex(1) == v2 && f->vertex(2) == v3)
      return true;
    if(f->vertex(1) == v1 && f->vertex(2) == v2 && f->vertex(0) == v3)
      return true;
    if(f->vertex(2) == v1 && f->vertex(0) == v2 && f->vertex(1) == v3)
      return true;
    return false;
  }

  /**
   *
   * @param v1 1st vertex
   * @param v2 2nd vertex
   * @param dest saving location
   * @return True if there is a common face that has v1 and v2 in CCW order, saves result in dest. False otherwise
   */
  bool share_common_face(Vertex_handle v1, Vertex_handle v2, Face_handle& dest)
  {
    Face_Circulator first_face = v1->incident_faces();
    Face_Circulator circ = first_face;
    do
    {
      if(belong_to_face(circ, v1, v2))
      {
        dest = circ;
        return true;
      }
    }
    while(++circ != first_face);
    return false;
  }

  /**
   *
   * @param v1 1st vertex
   * @param v2 2nd vertex
   * @param v3 3rd vertex
   * @param dest saving location
   * @return True if there is a common face that has v1,v2 and v3 in CCW order, saves result in dest. False otherwise
   */
  bool share_common_face(Vertex_handle v1, Vertex_handle v2, Vertex_handle v3, Face_handle& dest)
  {
    Face_Circulator first_face = v1->incident_faces();
    Face_Circulator circ = first_face;
    do
    {
      if(belong_to_face(circ, v1, v2, v3))
      {
        dest = circ;
        return true;
      }
    }
    while(++circ != first_face);
    return false;
  }

  /**
   *
   * @param n size of the vector of interest
   * @return n of possible combinations of contiguous sub-vector of size at least 2 up to n
   * @note Application of https://en.wikipedia.org/wiki/Triangular_number
   */
  inline uint get_max_combinatorial_options(const uint n) { return n * (n - 1) / 2; }


  /**
   *
   * @param vector_size size of vector of interest
   * @param min_l minimum length of the combination bounds. Must be at least two
   * @return n order list (from the longest to the shortest) of pair of offsets to bound the iteration of the vector.
   */
  std::vector<std::pair<uint, uint>> get_combinatorial_indexes(const uint vector_size, const uint min_l = 2)
  {
    if(min_l < 2)
      throw std::runtime_error("min_l too low");
    else
    {
      std::vector<std::pair<uint, uint>> result;
      uint necessary_depth = vector_size + 1 - min_l;
      // loops through all depths
      for(int poss_opt = 1; poss_opt <= necessary_depth; poss_opt++)
      {
        // for each depth we have #depth options
        for(int op = 0; op < poss_opt; op++)
        {
          result.push_back(std::pair<uint, uint>(op, poss_opt - 1 - op));
        }
      }
      return result;
    }
  }


} // namespace UTILS


#endif // TRIANGULATION_2_EXAMPLES_UTILS_H
