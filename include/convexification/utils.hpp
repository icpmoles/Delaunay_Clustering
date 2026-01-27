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


#include <boost/property_map/property_map.hpp>
#include <iostream>
#include <unordered_map>


#include <fstream>
#include <iostream>


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


typedef std::vector<Vertex_handle> Cluster_t;
typedef std::vector<Cluster_t> MultiCluster_t;


typedef CGAL::Triangulation_ds_vertex_circulator_2<Tds> Vertex_Circulator;
typedef CGAL::Container_from_circulator<Vertex_Circulator> Vertex_Container;
typedef Vertex_Container::iterator Iterator;

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
    std::cout << "1) " << f->vertex(0)->point() << std::endl;
    std::cout << "2) " << f->vertex(1)->point() << std::endl;
    std::cout << "3) " << f->vertex(2)->point() << std::endl;
  }

} // namespace UTILS


#endif // TRIANGULATION_2_EXAMPLES_UTILS_H
