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
#include <fstream>
#include <unordered_map>
#include <boost/property_map/property_map.hpp>


#include <wkt_inport.h>

// struct FaceInfo2
// {
//   FaceInfo2(){}
//   int nesting_level;
//   int group_id;
//   bool marked = false;
//   float area = 0.0;
//   bool is_in_domain(){
//     return nesting_level%2 == 1;
//   }
//   void set_marked(bool mark){
//     marked = mark;
//   }
//   float get_area(){
//     return area;
//   }
//   void set_area(float value){
//     if (value > 0.0) area = value;
//   }
//
//
// };


namespace PS = CGAL::Polyline_simplification_2;

typedef CGAL::Exact_predicates_inexact_constructions_kernel       K; // EPIC
typedef CGAL::Triangulation_vertex_base_2<K>                      Vb;
// typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo2,K>    FbwI;
// typedef CGAL::Constrained_triangulation_face_base_2<K,FbwI>       CtFb;
// typedef CGAL::Delaunay_mesh_face_base_2<K>                        DmFb;
// typedef CGAL::Triangulation_data_structure_2<Vb,FbwI>               TDS;
// typefef CGAL::Triangulation_face_base_with_info_2<FaceInfo2, K, CtFb> FbMI; //FaceBase of Mesh with Info
// typedef CGAL::Triangulation_data_structure_2<Vb,FbMI>               TDSMeshwithInfo;

typedef CGAL::Delaunay_mesh_face_base_2<K>                  Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb>        Tds;

typedef CGAL::Exact_predicates_tag                                Itag;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds>        CDT;
typedef CGAL::Triangulation_2<K>                                  Triangulation;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDT>                  Criteria;
typedef CDT::Face_handle                                          Face_handle;
typedef CDT::Vertex_handle                                        Vertex_handle;

typedef CDT::Point                                                Point;
typedef CGAL::Polygon_2<K>                                        Polygon;
typedef CGAL::Bbox_2                                              bbox_2;

// typedef CGAL::Polygon_with_holes_2<K>                             Polygon_with_holes_2;

typedef CGAL::Polygon_with_holes_2<K>                             Polygon_wh;
typedef std::vector<Point>                                        MultiPoint;
typedef std::list<Polygon_wh>                                     Poly_list;

typedef std::unordered_map<Face_handle, bool>                     BooleanFaceMap;
typedef std::unordered_map<Face_handle, float>                    AreaFaceMap;



typedef PS::Stop_below_count_ratio_threshold                      Stop;
typedef PS::Squared_distance_cost                                 Cost;


void center_coordinates(Polygon_wh& polygon, Polygon_wh& polygon_out)
{
  const bbox_2 bounds = polygon.bbox();
  const double x_min = bounds.xmin();
  const double y_min = bounds.ymin();

  std::cout << "x_min: " << x_min << "; y_min: " << y_min<< std::endl;
  CGAL::Aff_transformation_2<K> translate(CGAL::TRANSLATION, CGAL::Vector_2<K>(x_min,y_min));

  const Polygon_wh new_polygon(transform(translate,polygon.outer_boundary()));
  polygon_out = new_polygon;
  for (const Polygon& hole : polygon.holes())
    polygon_out.add_hole(transform(translate,hole));


}

// void get_grass(CDT& full_triangulation, const boost::associative_property_map<FaceOwnershipMap> map)
// {
//
//   for ( Face_handle f: full_triangulation.finite_face_handles())
//   {
//
//   }
//
//  }

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

void get_stats(const CDT& triangualation, const boost::associative_property_map<BooleanFaceMap> map)
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

void get_stats(const CDT& triangualation)
{
  int face_count = 0;
  int indomain_face_count = 0;
  for (Face_handle f : triangualation.finite_face_handles())
  {
    if(f->is_in_domain()) ++indomain_face_count;
    ++face_count;
  }
  std::cout << std::endl << std::endl << "ghiande indomain faces (wo map): " << indomain_face_count << std::endl;
  std::cout << "ghiande total faces (wo map): " << face_count << std::endl << std::endl;


}


void get_simple_polygon_wh(Polygon_wh& field)
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

float get_area(Face_handle f)
{
  Polygon polygon;
  polygon.push_back(f->vertex(0)->point());
  polygon.push_back(f->vertex(1)->point());
  polygon.push_back(f->vertex(2)->point());


  return polygon.area();
}

float get_best_greedy(Face_handle f)
{


  Polygon triangle;
  triangle.push_back(f->vertex(0)->point());
  triangle.push_back(f->vertex(1)->point());
  triangle.push_back(f->vertex(2)->point());
  float area = triangle.area();
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
  get_simple_polygon_wh(ghiande_pwh_offset);
  center_coordinates(ghiande_pwh_offset, ghiande_pwh);
  std::cout << "show original field" << std::endl << std::endl;

  CGAL::draw(ghiande_pwh);
  simplify_obstacles(ghiande_pwh,ghiande_pwh_simple_obstacles, 1.0, 0.5);
  ghiande_pwh = ghiande_pwh_simple_obstacles;
  CGAL::draw(ghiande_pwh);




  // build  constrained triangulation
  cdt_ghiande.insert(ghiande_pwh.outer_boundary().begin(), ghiande_pwh.outer_boundary().end()); // add the boundary
  cdt_ghiande.insert_constraint(ghiande_pwh.outer_boundary().begin(), ghiande_pwh.outer_boundary().end(), true); // add the boundary

  for (const Polygon&  hole: ghiande_pwh.holes()) // add the holes
    cdt_ghiande.insert(hole.vertices_begin(), hole.vertices_end());

  for (const Polygon&  hole: ghiande_pwh.holes()) // add the holes
    cdt_ghiande.insert_constraint(hole.vertices_begin(), hole.vertices_end(), true);

  std::cout << "constrained triangulation depth: " << cdt_ghiande.dimension() << std::endl;
  std::cout << "display constrained triangulation" << std::endl << std::endl;
  CGAL::draw(cdt_ghiande);


  BooleanFaceMap ghiande_in_domain_map;   // unorderedmap / hash-map

  boost::associative_property_map<BooleanFaceMap> in_domain_ghiande(ghiande_in_domain_map); // make it []

  CGAL::mark_domain_in_triangulation(cdt_ghiande, in_domain_ghiande);

  get_stats(cdt_ghiande, in_domain_ghiande);
  CGAL::draw(cdt_ghiande, in_domain_ghiande);


  CGAL::mark_domain_in_triangulation(cdt_ghiande);

  get_stats(cdt_ghiande);
  CGAL::draw(cdt_ghiande);

  std::cout << "Refining the domain..." << std::endl;
  CGAL::refine_Delaunay_mesh_2(cdt_ghiande, CGAL::parameters::criteria(Criteria(b, 5.0)));
  CGAL::mark_domain_in_triangulation(cdt_ghiande);

  get_stats(cdt_ghiande);
  CGAL::draw(cdt_ghiande);


  // test marker
  std::cout << "test marker" << std::endl;
  generate_convex_set(cdt_ghiande, 2);
  CGAL::draw(cdt_ghiande);


  // get_stats(cdt_ghiande, in_domain_ghiande);
  // CGAL::draw(cdt_ghiande, in_domain_ghiande);

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
