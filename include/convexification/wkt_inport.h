//
// Created by Iacopo Moles on 21/01/26.
//

#ifndef TRIANGULATION_2_EXAMPLES_WKT_INPORT_H
#define TRIANGULATION_2_EXAMPLES_WKT_INPORT_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>

#include <CGAL/Polygon_2.h>
#include <CGAL/IO/WKT.h>

namespace WKT_IO {

typedef CGAL::Exact_predicates_inexact_constructions_kernel       K; // EPIC
typedef CGAL::Triangulation_vertex_base_2<K>                      Vb;
typedef CGAL::Delaunay_mesh_face_base_2<K>                        Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb>              Tds;

typedef CGAL::Exact_predicates_tag                                Itag;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds>        CDT;

typedef CDT::Point                                                Point;
typedef CGAL::Polygon_2<K>                                        Polygon;
typedef CGAL::Bbox_2                                              bbox_2;

typedef CGAL::Polygon_with_holes_2<K>                             Polygon_wh;
typedef std::vector<Point>                                        MultiPoint;
typedef std::list<Polygon_wh>                                     Poly_list;

/**
 *
 * @param mp Destination variable
 *
 * Overwrites Destination with OBSTACLES from GHIANDE as an UNORDERED collection of Points
 */
inline void get_obstacles_as_multipoint(MultiPoint& mp)
{
  std::ifstream obstacle_file("data/ghiande_obstacles_only_multipart.wkt");
  int wkt_count = 0;
  // MultiPoint mp;
  do
  {
    CGAL::IO::read_multi_point_WKT(obstacle_file, mp);
    // if (wkt_count % 100 == 0)
    std::cout << "wkt_count multi: " << wkt_count << std::endl;
    wkt_count++;
  }while(obstacle_file.good() && !obstacle_file.eof());
  // CGAL::draw(mp);
}

/**
 *
 * @param perimeter_points Destination variable
 *
 * Overwrites Destination with PERIMETER from GHIANDE as an UNORDERED collection of Points
 */
inline void get_perimeter(MultiPoint& perimeter_points)
{
  std::ifstream is("data/ghiande_perimeter_2m.wkt");
  Poly_list polys;
  int wkt_count = 0;
  do
  {
    Polygon_wh p;
    CGAL::IO::read_polygon_WKT(is, p);
    if(!p.outer_boundary().is_empty())
      polys.push_back(p);
    std::cout << "wkt_count: " << wkt_count << std::endl;
    wkt_count++;
  }while(is.good() && !is.eof());

  for(const Polygon_wh& p_wh : polys)
  {
    for (const Point p: p_wh.outer_boundary().vertices())
      perimeter_points.push_back(p);
  }
}


/**
 *
 * @param field Destination variable
 *
 * Overwrites Destination with PERIMETER and OBSTACLES of GHIANDE of type CGAL::Polygon_with_holes_2
 *
 * PERIMETER will be accessible with outer_boundary()
 *
 * OBSTACLES will be accessible by iterating holes()
 */
inline void get_full_field_as_polygon_wh(Polygon_wh& field)
{

  std::ifstream field_stream("data/fake_ghiande.wkt");
  int wkt_count = 0;
  do
  {
    CGAL::IO::read_polygon_WKT(field_stream, field);
    // if (wkt_count % 100 == 0)
    std::cout << "wkt_poly_wh: " << wkt_count << std::endl;
    wkt_count++;
  }while(field_stream.good() && !field_stream.eof());

}

/**
 *
 * @param field Destination variable
 *
 * Overwrites Destination with PERIMETER and OBSTACLES of an ARBITRARY SHAPE of type CGAL::Polygon_with_holes_2
 *
 * PERIMETER will be accessible with outer_boundary()
 *
 * OBSTACLES will be accessible by iterating holes()
 */
inline void get_simple_polygon_wh(Polygon_wh& field)
{

  std::ifstream field_stream("data/naive_polygon_wh.wkt");
  int wkt_count = 0;
  do
  {
    CGAL::IO::read_polygon_WKT(field_stream, field);
    // if (wkt_count % 100 == 0)
    std::cout << "wkt_poly_wh: " << wkt_count << std::endl;
    wkt_count++;
  }while(field_stream.good() && !field_stream.eof());

}
}

#endif //TRIANGULATION_2_EXAMPLES_WKT_INPORT_H