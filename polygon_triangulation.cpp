//
// Created by Iacopo Moles on 21/01/26.
//

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/draw_triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/draw_polygon_with_holes_2.h>
#include <CGAL/mark_domain_in_triangulation.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/IO/WKT.h>
#include <CGAL/Polyline_simplification_2/simplify.h>
#include <CGAL/min_quadrilateral_2.h>

#include <iostream>
#include <unordered_map>
#include <boost/property_map/property_map.hpp>


#include <convexification/wkt_inport.h>
#include <convexification/preprocessing.h>

namespace PS = CGAL::Polyline_simplification_2;

typedef CGAL::Exact_predicates_inexact_constructions_kernel       K; // EPIC
typedef CGAL::Triangulation_vertex_base_2<K>                      Vb;

typedef CGAL::Delaunay_mesh_face_base_2<K>                        Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb>              Tds;

typedef CGAL::Exact_predicates_tag                                Itag;
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

typedef std::unordered_map<Face_handle, bool>                     BooleanFaceMap;
typedef std::unordered_map<Face_handle, double>                   AreaFaceMap;



typedef PS::Stop_below_count_ratio_threshold                      Stop;
typedef PS::Squared_distance_cost                                 Cost;


void get_stats(const CDT& triangulation, const boost::associative_property_map<BooleanFaceMap> map)
{
  int face_count = 0;
  int indomain_face_count = 0;
  for (Face_handle f : triangulation.finite_face_handles())
  {
    if ( get(map, f) ) ++indomain_face_count;
    ++face_count;
  }
  std::cout << std::endl << std::endl << "polygon indomain faces: " << indomain_face_count << std::endl;
  std::cout << "polygon total faces: " << face_count << std::endl << std::endl;


}

void get_stats(const CDT& triangulation)
{
  int face_count = 0;
  int indomain_face_count = 0;
  for (Face_handle f : triangulation.finite_face_handles())
  {
    if(f->is_in_domain()) ++indomain_face_count;
    ++face_count;
  }
  std::cout << std::endl << std::endl << "polygon indomain faces (w/o map): " << indomain_face_count << std::endl;
  std::cout << "polygon total faces (w/o map): " << face_count << std::endl << std::endl;


}



void generate_funky_set(const CDT& triangulation, int n)
{
  int fa = 0;
  for (const Face_handle f : triangulation.finite_face_handles())
  {
    if (fa%n==0 && f->is_in_domain())
      f->set_marked(true);
    else
      f->set_marked(false);

    fa++;
  }
}

double get_area(Face_handle f)
{
  Polygon polygon;
  polygon.push_back(f->vertex(0)->point());
  polygon.push_back(f->vertex(1)->point());
  polygon.push_back(f->vertex(2)->point());


  return polygon.area();
}

double get_best_greedy(Face_handle f)
{


  Polygon triangle;
  triangle.push_back(f->vertex(0)->point());
  triangle.push_back(f->vertex(1)->point());
  triangle.push_back(f->vertex(2)->point());
  double area = triangle.area();
  // f->set_area(area*100);
  return area;

}

void retrieve_best(Face_handle f)
{


  std::cout  << "dummy" << std::endl;

}


void generate_convex_set(const CDT& triangulation, int concave_steps)
{
  // typedef std::list<Face_handle> face_group;
  // std::map<int, face_group> convex_sets;
  // Face_handle face = triangulation.all_faces_begin();
  for (Face_handle f : triangulation.finite_face_handles())
  {
    get_best_greedy(f);
  }

  for (Face_handle f : triangulation.finite_face_handles())
  {
    retrieve_best(f);
  }



}

int main(int argc, char* argv[])
{

  float b ((argc>1)?std::stof(argv[1]):1.4);
  std::cout << "b: " << b << " , B: " << std::sqrt(0.25/b) << " , alpha: " << std::asin(std::sqrt(b)) * 180.0 / 3.141592653 << std::endl;
  //std::list<Polygon> polys;
  // from polygon_wkt

  if (false)
  {
    MultiPoint perimeter_points;
    WKT_IO::get_perimeter(perimeter_points);

    MultiPoint mp;
    WKT_IO::get_obstacles_as_multipoint(mp);
    Triangulation tri_workplace;
    tri_workplace.insert(perimeter_points.begin(), perimeter_points.end());
    tri_workplace.insert(mp.begin(), mp.end() );
    std::cout << "display naive triangulation" << std::endl << std::endl;
    CGAL::draw(tri_workplace); // naive triangulation
  }

  CDT cdt_workplace;
  Polygon_wh workplace_pwh_offset, workplace_pwh, workplace_pwh_simple_obstacles; // workplace polygon with holes
  WKT_IO::get_full_field_as_polygon_wh(workplace_pwh_offset);
  PP::center_coordinates(workplace_pwh_offset, workplace_pwh);
  std::cout << "show original field" << std::endl << std::endl;
  CGAL::draw(workplace_pwh);

  PP::simplify_obstacles_naive(workplace_pwh,workplace_pwh_simple_obstacles, 1.0, 0.5);
  workplace_pwh = workplace_pwh_simple_obstacles;
  std::cout << "show simplified field" << std::endl << std::endl;
  CGAL::draw(workplace_pwh);




  // build  constrained triangulation
  cdt_workplace.insert(workplace_pwh.outer_boundary().begin(), workplace_pwh.outer_boundary().end()); // add the boundary
  cdt_workplace.insert_constraint(workplace_pwh.outer_boundary().begin(), workplace_pwh.outer_boundary().end(), true); // add the boundary

  for (const Polygon&  hole: workplace_pwh.holes()) // add the holes
    cdt_workplace.insert(hole.vertices_begin(), hole.vertices_end());

  for (const Polygon&  hole: workplace_pwh.holes()) // add the holes
    cdt_workplace.insert_constraint(hole.vertices_begin(), hole.vertices_end(), true);

  std::cout << "constrained triangulation depth: " << cdt_workplace.dimension() << std::endl;
  std::cout << "display constrained triangulation" << std::endl << std::endl;
  CGAL::draw(cdt_workplace);


  BooleanFaceMap workplace_in_domain_map;   // unorderedmap / hash-map

  boost::associative_property_map<BooleanFaceMap> in_domain_workplace(workplace_in_domain_map); // make it []

  CGAL::mark_domain_in_triangulation(cdt_workplace, in_domain_workplace);

  get_stats(cdt_workplace, in_domain_workplace);
  CGAL::draw(cdt_workplace, in_domain_workplace);


  CGAL::mark_domain_in_triangulation(cdt_workplace);

  get_stats(cdt_workplace);
  CGAL::draw(cdt_workplace);

  std::cout << "Refining the domain..." << std::endl;
  CGAL::refine_Delaunay_mesh_2(cdt_workplace, CGAL::parameters::criteria(Criteria(b, 5.0)));
  CGAL::mark_domain_in_triangulation(cdt_workplace);

  get_stats(cdt_workplace);
  CGAL::draw(cdt_workplace);


  // test marker
  std::cout << "test marker" << std::endl;
  generate_convex_set(cdt_workplace, 2);
  CGAL::draw(cdt_workplace);


  // get_stats(cdt_workplace, in_domain_workplace);
  // CGAL::draw(cdt_workplace, in_domain_workplace);

  // int workplace_indomain_face_count = 0;
  // int workplace_face_count = 0;
  //
  // for (Face_handle f : cdt_workplace.finite_face_handles())
  // {
  //   if ( get(in_domain_workplace, f) ) ++workplace_indomain_face_count;
  //   ++workplace_face_count;
  // }
  // std::cout << std::endl << std::endl << "workplace indomain faces: " << workplace_indomain_face_count << std::endl;
  // std::cout << "workplace total faces: " << workplace_face_count << std::endl << std::endl;

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
