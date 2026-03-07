//
// Created by Iacopo Moles on 21/01/26.
//

#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/WKT.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Polyline_simplification_2/simplify.h>
#include <CGAL/draw_polygon_with_holes_2.h>
#include <CGAL/draw_triangulation_2.h>
#include <CGAL/mark_domain_in_triangulation.h>
#include <CGAL/min_quadrilateral_2.h>

#include <boost/program_options.hpp>
#include <boost/property_map/property_map.hpp>
#include <iostream>
#include <unordered_map>


#include "Cluster/Cluster_Manager.hpp"
#include "Convexification/preprocessing.hpp"
#include "Convexification/utils.hpp"
#include "Convexification/wkt_import.hpp"

namespace po = boost::program_options;

int main(int argc, char* argv[])
{
  bool view_plot_opt = false;
  bool ghiande_field_opt = false;
  // TODO: support arbitrary filename for field file

  float angle_bound_opt;
  float size_bound_opt;

  try
  {
    po::options_description desc("Allowed options");
    desc.add_options()("help", "produce help message")(
      "ghiande", po::bool_switch(&ghiande_field_opt)->default_value(false), "uses complex field instead of basic")(
      "plots", po::bool_switch(&view_plot_opt)->default_value(false), "display plots of intermediate/final steps")(
      "b", po::value<float>(&angle_bound_opt)->default_value(0.14),
      "aspect bound: \n Refer to: CGAL::Delaunay_mesh_size_criteria_2 or "
      "https://doc.cgal.org/5.6.3/Mesh_2/classCGAL_1_1Delaunay__mesh__size__criteria__2.html for more information")(
      "size", po::value<float>(&size_bound_opt)->default_value(5.0), "size bound of triangulation in meters");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if(vm.count("help"))
    {
      std::cout << desc << "\n";
      return 0;
    }

    if(vm.count("ghiande"))
    {
      std::cout << "Using Complex Field" << ".\n";
    }
    else
    {
      std::cout << "Using Simple Field" << ".\n";
    }

    if(vm.count("plots"))
    {
      std::cout << "Showing Plots" << ".\n";
    }

    if(vm.count("b"))
    {
      std::cout << "New aspect bound was set to " << vm["b"].as<float>() << ".\n";
    }
  }
  catch(std::exception& e)
  {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }
  catch(...)
  {
    std::cerr << "Exception of unknown type!\n";
  }

  // ((argc > 1) ? std::stof(argv[1]) : 0.4);
  std::cout << "b: " << angle_bound_opt << " , B: " << std::sqrt(0.25 / angle_bound_opt)
            << " , alpha: " << std::asin(std::sqrt(angle_bound_opt)) * 180.0 / 3.141592653 << "°" << std::endl;


  // std::list<Polygon> polys;
  //  from polygon_wkt

  if(false)
  {
    MultiPoint perimeter_points;
    WKT_IO::get_perimeter(perimeter_points);

    MultiPoint mp;
    WKT_IO::get_obstacles_as_multipoint(mp);
    Triangulation tri_workplace;
    tri_workplace.insert(perimeter_points.begin(), perimeter_points.end());
    tri_workplace.insert(mp.begin(), mp.end());
    std::cout << "display naive triangulation" << std::endl << std::endl;
    if(view_plot_opt)
      CGAL::draw(tri_workplace); // naive triangulation
  }

  CDT cdt_workplace;
  Polygon_wh workplace_pwh_offset, workplace_pwh, workplace_pwh_simple_obstacles; // workplace polygon with holes

  // false for simple polygon
  // true for ghiande
  if(ghiande_field_opt)
    WKT_IO::get_full_field_as_polygon_wh(workplace_pwh_offset);
  else
    WKT_IO::get_simple_polygon_wh(workplace_pwh_offset);
  PP::center_coordinates(workplace_pwh_offset, workplace_pwh);
  std::cout << "show original field" << std::endl << std::endl;
  if(false)
    CGAL::draw(workplace_pwh);

  PP::simplify_obstacles_naive(workplace_pwh, workplace_pwh_simple_obstacles, 1.0, 0.5);
  workplace_pwh = workplace_pwh_simple_obstacles;
  std::cout << "show simplified field" << std::endl << std::endl;
  if(false)
    CGAL::draw(workplace_pwh);


  // build  constrained triangulation
  cdt_workplace.insert(workplace_pwh.outer_boundary().begin(),
                       workplace_pwh.outer_boundary().end()); // add the boundary
  cdt_workplace.insert_constraint(workplace_pwh.outer_boundary().begin(), workplace_pwh.outer_boundary().end(),
                                  true); // add the boundary

  for(const Polygon& hole : workplace_pwh.holes()) // add the holes
    cdt_workplace.insert(hole.vertices_begin(), hole.vertices_end());

  for(const Polygon& hole : workplace_pwh.holes()) // add the holes
    cdt_workplace.insert_constraint(hole.vertices_begin(), hole.vertices_end(), true);

  std::cout << "constrained triangulation depth: " << cdt_workplace.dimension() << std::endl;
  std::cout << "display constrained triangulation" << std::endl << std::endl;
  if(false)
    CGAL::draw(cdt_workplace);


  BooleanFaceMap workplace_in_domain_map; // unorderedmap / hash-map

  boost::associative_property_map<BooleanFaceMap> in_domain_workplace(workplace_in_domain_map); // make it []

  CGAL::mark_domain_in_triangulation(cdt_workplace, in_domain_workplace);

  UTILS::get_stats(cdt_workplace, in_domain_workplace);
  if(false)
    CGAL::draw(cdt_workplace, in_domain_workplace);


  CGAL::mark_domain_in_triangulation(cdt_workplace);

  UTILS::get_stats(cdt_workplace);
  if(false)
    CGAL::draw(cdt_workplace);

  std::cout << "Refining the domain..." << std::endl;
  CGAL::refine_Delaunay_mesh_2(cdt_workplace, CGAL::parameters::criteria(Criteria(angle_bound_opt, size_bound_opt)));
  CGAL::mark_domain_in_triangulation(cdt_workplace);

  UTILS::get_stats(cdt_workplace);
  if(false)
    CGAL::draw(cdt_workplace);

  // test combinatorial exploration
  std::vector<std::string> test_v{"1", "2", "3", "4"};
  std::vector<std::pair<uint, uint>> indexes_ = UTILS::get_combinatorial_indexes(test_v.size());
  std::cout << "PRINTING" << std::endl;
  for(std::pair<uint, uint> idx : indexes_)
  {
    for(auto it = std::next(test_v.begin(), idx.first); it != std::prev(test_v.end(), idx.second); ++it)
    {
      std::cout << *it << " ";
    }
    std::cout << std::endl;
  }

  // test cluster manager

  CM::Cluster_Manager cdt_workplace_wi = CM::Cluster_Manager(cdt_workplace, view_plot_opt);
  cdt_workplace_wi.iterate();
  if(view_plot_opt)
    cdt_workplace_wi.show_map(0);


  return 0;
}
