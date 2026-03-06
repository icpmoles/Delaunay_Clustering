//
// Created by icpmoles on 05/02/26.
//
#ifndef DELAUNAY_CLUSTERING_CLUSTER_HPP
#define DELAUNAY_CLUSTERING_CLUSTER_HPP
#include <CGAL/Polygon_2.h>
#include <iterator>

#include "Cluster/Cluster_Circulator.hpp"
#include "Cluster/Solution.hpp"
#include "Convexification/Augmented_Mesh.hpp"
#include "Convexification/utils.hpp"

namespace CC
{
  class Cluster
  {
  public:
    // Type Definitions
    typedef MultiVertex_t Container;
    typedef typename MultiVertex_t::difference_type difference_type;
    typedef typename MultiVertex_t::value_type value_type;
    typedef typename MultiVertex_t::pointer pointer;
    typedef typename MultiVertex_t::reference reference;
    typedef typename MultiVertex_t::const_reference const_reference;
    typedef typename MultiVertex_t::iterator Vertex_const_iterator;
    typedef Cluster_circulator Vertex_const_circulator;
    typedef typename MultiVertex_t::iterator Vertex_iterator;
    typedef MultiVertex_t Vertices;
    typedef Vertex_const_circulator Vertex_circulator;

    /// Constructors
    Cluster(Face_handle f, AUM::Augmented_Mesh* mesh, uint cluster_id)
    {
      vertex_container.push_back(f->vertex(0));
      vertex_container.push_back(f->vertex(1));
      vertex_container.push_back(f->vertex(2));
      pMesh_ = mesh;
      cluster_id_ = cluster_id;
      own_face(f);
    }
    // Copy constructor.
    Cluster(const Cluster& cluster) = default;
    // Move constructor
    Cluster(Cluster&& cluster) = default;

    /// Creates a cluster with vertices from the sequence
    /// defined by the range \c [first,last).
    /// The value type of \c InputIterator must be \c Vertex_handle.
    template <class InputIterator>
    Cluster(InputIterator first, InputIterator last, AUM::Augmented_Mesh* mesh) :
        vertex_container(first, last), pMesh_(mesh)
    {
    }

    /// Modifiers
    // void set(Vertex_iterator i, const Face_Description& fd)
    // {
    //   // *i = q;
    //
    //   //pMesh_->set_face_description(fd,*i);
    // }

    /**
     *
     * @param i target
     * @param q payload
     * @note Not sure if necessary tbh
     * @warning doesn't check for feasibility
     */
    void set(Vertex_iterator i, const Vertex_handle& q) { *i = q; }

    bool can_be_assigned(Face_handle f) const
    {
      Face_Description* fd = pMesh_->get_face_description(f);
      return (fd->is_Cluster_Assigned == false && fd->is_InDomain);
    }

    /**
     *
     * @param v1
     * @param v2
     * @param v3
     * @return True if v1, v2 and v3 belong to a common face in a CCW order, which is in domain and still not assigned.
     */
    bool is_insertable(Vertex_handle v1, Vertex_handle v2, Vertex_handle v3)
    {
      Face_handle f;
      bool success = UTILS::share_common_face(v1, v2, v3, f) && f->is_in_domain() && can_be_assigned(f);
      if(success)
      {
        std::cout << v1->point() << " & " << v2->point() << " & " << v3->point() << " OK" << std::endl
                  << "Face in common:" << std::endl;
        UTILS::print_triangle_vertices(f);
      }
      return success;
    }

    /**
     *
     * @param i
     * @param q
     * @return True if i and q belong to a common face in a CCW order
     */
    bool is_insertable(Vertex_iterator i, Vertex_handle& q) { return this->is_insertable(*(i += 1), *i, q); }
    /**
     *
     * @param i
     * @param q
     * @return True if i and q belong to a common face in a CCW order
     */
    bool is_insertable(Vertex_circulator i, Vertex_handle& q)
    {
      // auto  next_i = (i+=1);
      Vertex_handle next_i = *(i += 1);

      Vertex_iterator i_vit = i.mod_iterator();
      return is_insertable(next_i, *i_vit, q);
    }

    /**
     * Returns the future polygon if a new vertex or face is added
     * @param v_new new vertex to insert
     * @param i index where to insert before
     * @return future polygon after adding a new vertex or face
     * @note This assumes the is_insertable check was positive
     */
    Polygon get_future_polygon(Vertex_handle i, Vertex_handle v_new)
    {
      Polygon p;
      Cluster_circulator first = this->vertices_circulator();
      Cluster_circulator cursor = first;
      // std::cout << "Circulator Starting at " << first.get_point();
      do
      {
        p.push_back(cursor.get_point());
        if(cursor.get_vertex() == i)
        {
          p.push_back(v_new->point());
        }
      }
      while(++cursor != first);

      return p;
    }

    /// Inserts the vertex `q` before `i`. The return value points to
    /// the inserted vertex. If the vertex q and the iterator don't share a common face then
    /// it returns the iterator itself to indicate an error. N.b. check with is_insertable() before executing
    Vertex_iterator insert(Vertex_iterator i, const Vertex_handle& q)
    {
      // if(Face_handle result; UTILS::share_common_face(*i, q, result))
      // {
      //   // Set Properties of common face
      //   own_face(result);
      //   // returns modified vertex list
      return vertex_container.insert(i, q);
      // }
      // // returns original iterator to signal error
      // return i;
    }

    /// Inserts the vertex `q` before `i`. The return value points to
    /// the inserted vertex.
    Vertex_iterator insert(Vertex_circulator i, const Vertex_handle& q) { return this->insert(i.mod_iterator(), q); }


    void reverse_orientation()
    {
      if(vertex_container.size() <= 1)
        return;
      typename MultiVertex_t::iterator i = vertex_container.begin();
      std::reverse(++i, vertex_container.end());
    }

    /// @}

    /// \name Access Functions
    /// The following methods of the class Polygon_2
    /// return circulators and iterators that allow to traverse the
    /// vertices and edges.
    /// @{

    /// Returns a constant iterator that allows to traverse the
    /// vertices of the cluster.
    Vertex_iterator vertices_begin() const { return const_cast<Cluster&>(*this).vertex_container.begin(); }

    /// Returns the corresponding past-the-end iterator.
    Vertex_iterator vertices_end() const { return const_cast<Cluster&>(*this).vertex_container.end(); }

    /// returns the range of vertices.
    const Vertices& vertices() const { return vertex_container; }


    /// Returns a constant circulator that allows to traverse the
    /// vertices of the cluster.
    Vertex_circulator vertices_circulator() const
    {
      Cluster& self = const_cast<Cluster&>(*this);
      return Vertex_circulator(&self.vertex_container, self.vertex_container.begin());
    }

    // Assigns the face to this cluster
    void own_face(const Face_handle f)
    {
      Face_Description* fd = pMesh_->get_face_description(f);
      fd->Cluster_Id = cluster_id_;
      fd->is_Cluster_Assigned = true;
    }

    /**
     *
     * @param f Candidate face to check
     * @param v f is an incident face of v.
     * @param n_vertex N of vertex in common between face and cluster (1,2)
     * @return True if Face and Cluster share exactly n_vertex vertexes
     */
    bool is_neighbor(Face_handle f, Vertex_circulator v, uint n_vertex)
    {
      Vertex_handle circ = *v;
      Vertex_handle next_in_circulator = *(v += 1);
      Vertex_handle prev_in_circulator = *(v -= 2);
      int vertex_in_common = 0;
      if(UTILS::belong_to_face(f, circ, prev_in_circulator) && !UTILS::belong_to_face(f, next_in_circulator, circ))
      {
        vertex_in_common = 2;
      }
      else if(UTILS::belong_to_face(f, next_in_circulator, circ) && !UTILS::belong_to_face(f, circ, prev_in_circulator))
      {
        vertex_in_common = 2;
      }
      else if(UTILS::belong_to_face(f, circ))
      {
        vertex_in_common = 1;
      }

      return (n_vertex == vertex_in_common);
    }

    /**
     *
     * @param f Candidate face to check
     * @param i f is an incident face of i.
     * @param n_vertex N of vertex in common between face and cluster (1,2)
     * @return True if Face and Cluster share exactly n_vertex vertexes
     */
    bool is_neighbor(Face_handle f, Vertex_iterator i, uint n_vertex)
    {
      // TODO iterator to circulator tests
      return false;
    }

    /**
     *
     * @return True if it's still possible to expand, False otherwise
     *
     * Refreshes the list of free boundaries
     * Conditions for expansion: at least one neighbouring face is not an obstacle or assigned yet
     */
    bool refresh_free_boundaries()
    {
      bool is_possible = false;
      bool start_new_boundary = true;
      BoundaryId_t boundary_counter = 0;
      // Linear check
      Vertex_circulator start, cursor;
      start = cursor = this->vertices_circulator();
      do
      {
        if(is_neigbour_face_free(cursor.get_vertex(), cursor.get_next()))
        {
          is_possible = true;
          std::cout << "Found free face " << cursor.get_vertex()->point() << std::endl;
          if(start_new_boundary) // free_boundaries_.end()->size()>0)
          {
            std::cout << "Creating new boundary " << std::endl;

            free_boundaries_.push_back(Boundary_t(std::vector<Vertex_handle>{cursor.get_vertex()}, boundary_counter));
          }
          else
          {
            std::cout << "Continuing last boundary " << std::endl;
            free_boundaries_.back().first.push_back(cursor.get_vertex());
          }

          start_new_boundary = false;
        }
        else
        {
          start_new_boundary = true;
          boundary_counter++;
        }
      }
      while(++cursor != start);


      return is_possible;
    }

    /**
     *
     * @param v Vertex
     * @param v_next , next of v in CCW order
     * @return True if the face that has v_next, v in CCW order is free to expand
     */
    bool is_neigbour_face_free(Vertex_handle v, Vertex_handle v_next)
    {
      Face_Circulator cursor_v_f, start_v_f; // vertexes  incident faces' cursor/start handles
      cursor_v_f = start_v_f = v->incident_faces();
      do // loop through all vertex neighbours
      {
        if(UTILS::belong_to_face(cursor_v_f, v_next, v))
        {
          Face_Description* fd = pMesh_->get_face_description(cursor_v_f);
          if(fd->is_Cluster_Assigned == false && fd->is_InDomain)
            return true;
        }
      }
      while(++cursor_v_f != start_v_f);
      return false;
    }

    // getter for free_boundaries_
    std::vector<Boundary_t>* get_free_boundaries_ptr() { return &free_boundaries_; }

    // getter for free_boundaries_
    std::vector<Boundary_t> get_free_boundaries() { return free_boundaries_; }

    /**
     *
     * @return True if it was capable of finding feasible (convex) solutions, False otherwise
     */
    bool refresh_solutions()
    {
      bool conv_sol = true;
      for(Boundary_t boundary : free_boundaries_)
      {
        bool is_boundary_expandable = true;
        for(auto it = boundary.first.begin(); it != boundary.first.end(); ++it)
        {
          if(!is_neigbour_face_free(*it, *std::next(it)))
            is_boundary_expandable = false;
        }

        if(is_boundary_expandable)
        {
          Cluster_Solution sol(this->cluster_id_, expansion_solutions_[boundary].size() + 1);

          // TODO: Create exploration logic
          expansion_solutions_[boundary].push_back(sol);
        }
      }

      return conv_sol;
    }


  private:
    MultiVertex_t vertex_container; // main container with the list of vertexes that describe the boundary in CCW order
    std::vector<Boundary_t> free_boundaries_; // contains a list of CCW ordered list of vertexes describing the
                                              // various sections where the cluster can still expand
    AUM::Augmented_Mesh* pMesh_;
    uint cluster_id_;

    std::map<Boundary_t, std::vector<Cluster_Solution>>
      expansion_solutions_; // for each boundary you can have multiple Solution
  };


} // namespace CC

#endif // DELAUNAY_CLUSTERING_CLUSTER_HPP
