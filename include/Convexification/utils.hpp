//
// Created by icpmoles on 21/01/26.
//

#ifndef TRIANGULATION_2_EXAMPLES_UTILS_H
#define TRIANGULATION_2_EXAMPLES_UTILS_H

#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/WKT.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
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

// /**
//  * Collection of Clusters
//  */
// typedef std::vector<Cluster_t> MultiCluster_t;

// For circulator of (mesh) vertex type
// Why not using a standard Polygon2 type? Well, the polygon2 only allows iterating across points(x,y)
// losing the face information
// typedef MultiVertex_t::iterator MV_I;
// typedef CGAL::Circulator_from_iterator<MV_I> Cluster_Circulator;
// typedef CGAL::Container_from_circulator<Cluster_Circulator> Cluster_Container;
// typedef Cluster_Container::iterator Cluster_Iterator;


typedef CGAL::Triangulation_ds_face_circulator_2<Tds> Face_Circulator;
typedef CGAL::Container_from_circulator<Face_Circulator> Face_Container;

typedef CGAL::Triangulation_ds_edge_circulator_2<Tds> Edge_Circulator;
typedef CGAL::Container_from_circulator<Edge_Circulator> Edge_Container;

typedef CGAL::Triangulation_ds_vertex_circulator_2<Tds> Vertex_Circulator;
typedef CGAL::Container_from_circulator<Vertex_Circulator> Vertex_Container;
typedef Vertex_Container::iterator Vertex_Iterator;

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

  // walk metadata

  unsigned Distance = UNEXPLORED_VALUE; // how many greedy steps without convexity

} Face_Description;

namespace UTILS
{
  inline double get_area(Face_handle f)
  {
    Polygon polygon;
    polygon.push_back(f->vertex(0)->point());
    polygon.push_back(f->vertex(1)->point());
    polygon.push_back(f->vertex(2)->point());

    return polygon.area();
  }

  inline void print_triangle_vertices(const Face_handle f)
  {
    std::cout << "1st Vertex " << f->vertex(0)->point() << std::endl;
    std::cout << "2nd Vertex " << f->vertex(1)->point() << std::endl;
    std::cout << "3rd Vertex " << f->vertex(2)->point() << std::endl;
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
  bool share_common_face(Vertex_handle v1, Vertex_handle v2, Face_handle &dest)
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
  bool share_common_face(Vertex_handle v1, Vertex_handle v2, Vertex_handle v3, Face_handle &dest)
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




} // namespace UTILS


#endif // TRIANGULATION_2_EXAMPLES_UTILS_H
