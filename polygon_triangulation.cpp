#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/draw_triangulation_2.h>
#include <CGAL/draw_point_set_3.h>

#include <CGAL/draw_polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/draw_polygon_with_holes_2.h>
#include <CGAL/mark_domain_in_triangulation.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/IO/WKT.h>


#include <CGAL/Polyline_simplification_2/simplify.h>
#include <CGAL/min_quadrilateral_2.h>

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <boost/property_map/property_map.hpp>

namespace PS = CGAL::Polyline_simplification_2;

typedef CGAL::Exact_predicates_inexact_constructions_kernel       K;
typedef CGAL::Triangulation_vertex_base_2<K>                      Vb;
typedef CGAL::Constrained_triangulation_face_base_2<K>            Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb>               TDS;
typedef CGAL::Exact_predicates_tag                                Itag;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag>  CDT;
typedef CGAL::Triangulation_2<K>                                  Triangulation;
typedef CDT::Face_handle                                          Face_handle;
typedef CDT::Point                                                Point;
typedef CGAL::Polygon_2<K>                                        Polygon;
typedef CGAL::Bbox_2                                              bbox_2;

// typedef CGAL::Polygon_with_holes_2<K>                             Polygon_with_holes_2;

typedef CGAL::Polygon_with_holes_2<K>                             Polygon_wh;
typedef std::vector<Point>                                        MultiPoint;
typedef std::list<Polygon_wh>                                     Poly_list;

typedef std::unordered_map<Face_handle, bool>                     FaceOwnershipMap;

typedef PS::Stop_below_count_ratio_threshold                      Stop;
typedef PS::Squared_distance_cost                                 Cost;

void get_obstacles_as_multipoint(MultiPoint& mp)
{
  std::ifstream obstacle_file("data/ghiande_obstacles_only_multipart.wkt");
  std::list<Point> points;
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

void get_perimeter(MultiPoint& perimeter_points)
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

void get_full_field_as_polygon_wh(Polygon_wh& field)
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

void center_coordinates(Polygon_wh& polygon, Polygon_wh& polygon_out)
{
  std::list<Polygon> holes_translated;
  const bbox_2 bounds = polygon.bbox();
  const double x_min = bounds.xmin();
  const double y_min = bounds.ymin();

  std::cout << "x_min: " << x_min << "; y_min: " << y_min<< std::endl;
  CGAL::Aff_transformation_2<K> translate(CGAL::TRANSLATION, CGAL::Vector_2<K>(x_min,y_min));

  Polygon_wh new_polygon(transform(translate,polygon.outer_boundary()));
  polygon_out = new_polygon;
  for (const Polygon& hole : polygon.holes())
    polygon_out.add_hole(transform(translate,hole));


}

void simplify_obstacles(Polygon_wh& polygon, Polygon_wh& polygon_out, float width, float cost_stop)
{
  Cost cost;

  const Polygon_wh temp(polygon.outer_boundary());
  polygon_out = temp;
  for (const Polygon& hole: polygon.holes() )
  {
    const bbox_2 bounds = hole.bbox();
    const double delta_x = bounds.xmax() - bounds.xmin();
    const double delta_y = bounds.ymax() - bounds.ymin();
    if ((delta_x>width) ||( delta_y>width))
    {
      std::cout << "big hole!!" << std::endl;
       polygon_out.add_hole(PS::simplify(hole, cost, Stop(cost_stop)));

    } else
    {
      Polygon bounding_rectangle;
      CGAL::min_rectangle_2(
          hole.vertices_begin(), hole.vertices_end(), std::back_inserter(bounding_rectangle));
      polygon_out.add_hole(bounding_rectangle);
    }
  }

}

void get_stats(const CDT& triangualation, const boost::associative_property_map<FaceOwnershipMap> map)
{
  int face_count = 0;
  int indomain_face_count = 0;
  for (Face_handle f : triangualation.finite_face_handles())
  {
    if ( get(map, f) ) ++indomain_face_count;
    ++face_count;
  }
  std::cout << std::endl << std::endl << "ghiande indomain faces: " << indomain_face_count << std::endl;
  std::cout << "ghiande total faces: " << face_count << std::endl << std::endl;


}

int main(int argc, char* argv[])
{

  //std::ifstream is((argc>1)?argv[1]:"data/polygons.wkt");
  //std::list<Polygon> polys;
  // from polygon_wkt


  MultiPoint perimeter_points;
  get_perimeter(perimeter_points);

  MultiPoint mp;
  get_obstacles_as_multipoint(mp);
  Triangulation tri_ghiande;
  tri_ghiande.insert(perimeter_points.begin(), perimeter_points.end());
  tri_ghiande.insert(mp.begin(), mp.end() );
  std::cout << "display naive triangulation" << std::endl << std::endl;
  CGAL::draw(tri_ghiande); // naive triangulation


  CDT cdt_ghiande;
  Polygon_wh ghiande_pwh_offset, ghiande_pwh, ghiande_pwh_simple_obstacles; // ghiande polygon with holes
  get_full_field_as_polygon_wh(ghiande_pwh_offset);
  center_coordinates(ghiande_pwh_offset, ghiande_pwh);
  std::cout << "show original field" << std::endl << std::endl;

  CGAL::draw(ghiande_pwh);
  simplify_obstacles(ghiande_pwh,ghiande_pwh_simple_obstacles, 1.0, 0.5);
  CGAL::draw(ghiande_pwh_simple_obstacles);



  // build  constrained triangulation
  cdt_ghiande.insert(ghiande_pwh.outer_boundary().begin(), ghiande_pwh.outer_boundary().end()); // add the boundary
  for (const Polygon&  hole: ghiande_pwh.holes()) // add the holes
    cdt_ghiande.insert_constraint(hole.vertices_begin(), hole.vertices_end(), true);

  std::cout << "constrained triangulation depth: " << cdt_ghiande.dimension() << std::endl;
  std::cout << "display constrained triangulation" << std::endl << std::endl;
  CGAL::draw(cdt_ghiande);


  FaceOwnershipMap ghiande_in_domain_map;
  boost::associative_property_map<FaceOwnershipMap> in_domain_ghiande(ghiande_in_domain_map);

  CGAL::mark_domain_in_triangulation(cdt_ghiande, in_domain_ghiande);

  get_stats(cdt_ghiande, in_domain_ghiande);
  CGAL::draw(cdt_ghiande, in_domain_ghiande);

  // int ghiande_indomain_face_count = 0;
  // int ghiande_face_count = 0;
  //
  // for (Face_handle f : cdt_ghiande.finite_face_handles())
  // {
  //   if ( get(in_domain_ghiande, f) ) ++ghiande_indomain_face_count;
  //   ++ghiande_face_count;
  // }
  // std::cout << std::endl << std::endl << "ghiande indomain faces: " << ghiande_indomain_face_count << std::endl;
  // std::cout << "ghiande total faces: " << ghiande_face_count << std::endl << std::endl;

  return 0;


  // //construct two non-intersecting nested polygons
  // Polygon polygon1;
  // polygon1.push_back(Point(10,0));
  // polygon1.push_back(Point(12,0));
  // polygon1.push_back(Point(12,2));
  // polygon1.push_back(Point(11,1.75));
  // polygon1.push_back(Point(10,2));
  // Polygon polygon2;
  // polygon2.push_back(Point(10.5,0.5));
  // polygon2.push_back(Point(11.5,0.5));
  // polygon2.push_back(Point(11.5,1.5));
  // polygon2.push_back(Point(10.5,1.5));
  //
  // std::vector<Polygon> holes(1);
  // holes[0]=polygon2;
  // Polygon_wh pwh(polygon1,holes.begin(),holes.end());
  // //Insert the polygons into a constrained triangulation
  // CDT cdt;
  // cdt.insert_constraint(polygon1.vertices_begin(), polygon1.vertices_end(), true);
  // cdt.insert_constraint(polygon2.vertices_begin(), polygon2.vertices_end(), true);
  // cdt.insert_constraint(Point(10.25, 0.25), Point(10.25, 1.75));
  //
  // std::unordered_map<Face_handle, bool> in_domain_map;
  // boost::associative_property_map< std::unordered_map<Face_handle,bool> >
  //   in_domain(in_domain_map);
  //
  // //Mark facets that are inside the domain bounded by the polygon
  // CGAL::mark_domain_in_triangulation(cdt, in_domain);
  //
  // unsigned int count=0;
  // for (Face_handle f : cdt.finite_face_handles())
  // {
  //   if ( get(in_domain, f) ) ++count;
  // }
  //
  // std::cout << "There are " << count << " faces in the domain." << std::endl;
  // assert(count > 0);
  // assert(count < cdt.number_of_faces());
  //
  // CGAL::draw(pwh);
  // CGAL::draw(cdt);
  // CGAL::draw(cdt, in_domain);
  // return 0;
}
