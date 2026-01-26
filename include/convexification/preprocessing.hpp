//
// Created by Iacopo Moles on 21/01/26.
//

#ifndef TRIANGULATION_2_EXAMPLES_PREPROCESSING_H
#define TRIANGULATION_2_EXAMPLES_PREPROCESSING_H


#include "convexification/utils.hpp"


namespace PP {

namespace PS = CGAL::Polyline_simplification_2;

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




typedef PS::Stop_below_count_ratio_threshold                      Stop;
typedef PS::Squared_distance_cost                                 Cost;


/**
 *
 * @param polygon Input Polygon
 * @param polygon_out Output Variable
 *
 * Given an Input Polygon, returns a new Polygon resting on the XY axis of the North-West quadrant.
 *
 * Useful to "normalize" geographical coordinates to avoid numerical errors.
 */
inline void center_coordinates(Polygon_wh& polygon, Polygon_wh& polygon_out)
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

/**
 *
 * @param polygon Input Polygon
 * @param polygon_out Output Variable
 * @param width Threshold for classification of small/big obstacles. Suggested: 1x tool width.
 * @param cost_stop Cost stop for Polyline simplification. Suggested: 0.5
 *
 * Given an Input Polygon With Holes, overwrites the "polygon_out" with a new polygon with the following features:
 *
 * - perimeter: simplified polyline
 * - big obstacle: simplified polyline (we do care about concavity)
 * - small obstacle: simplified bounding box (we don't care about concavity)
 *
 */
inline void simplify_obstacles(const Polygon_wh& polygon, Polygon_wh& polygon_out, float width, float cost_stop)
{
  const Cost cost;
  // const Polygon_wh temp(polygon.outer_boundary());
  // const Polygon_wh temp(PS::simplify(polygon, cost, Stop(cost_stop)));
  polygon_out =  Polygon_wh(PS::simplify(polygon, cost, Stop(cost_stop)));
  // for (const Polygon& hole: polygon_out.holes() )
  for (auto hole=polygon_out.holes_begin(); hole!= polygon_out.holes_end(); ++hole)
  {
    const bbox_2 bounds = hole->bbox();
    const double delta_x = bounds.xmax() - bounds.xmin();
    const double delta_y = bounds.ymax() - bounds.ymin();
    if ((delta_x<width) && ( delta_y<width))
    {
      Polygon bounding_rectangle;
      CGAL::min_rectangle_2(
          hole->vertices_begin(), hole->vertices_end(), std::back_inserter(bounding_rectangle));
      polygon_out.erase_hole(hole);
      polygon_out.add_hole(bounding_rectangle);
    }
  }
}

inline void simplify_obstacles_naive(Polygon_wh& polygon, Polygon_wh& polygon_out, float width, float cost_stop)
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




}
#endif //TRIANGULATION_2_EXAMPLES_PREPROCESSING_H