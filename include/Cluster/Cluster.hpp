//
// Created by icpmoles on 05/02/26.
//
#include <iterator>
#ifndef DELAUNAY_CLUSTERING_CLUSTER_HPP
#define DELAUNAY_CLUSTERING_CLUSTER_HPP
#include "Cluster/Cluster_Circulator.hpp"
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

    void set(Vertex_iterator i, const Vertex_handle& q) { *i = q; }


    /// Inserts the vertex `q` before `i`. The return value points to
    /// the inserted vertex. If the vertex q and the iterator don't share a common face then
    /// it returns the iterator itself as an error.
    Vertex_iterator insert(Vertex_iterator i, const Vertex_handle& q)
    {
      Face_Circulator first_face = (*i)->incident_faces();
      Face_Circulator circ = first_face;
      Face_handle common_face;
      bool belongs = false;
      do
      {
        if(UTILS::belong_to_face(circ, std::prev(*i), q, (*i)))
        {
          belongs = true;
          common_face = circ;
        }
      }
      while(++circ != first_face);

      if(belongs)
      {
        own_face(common_face);
        return vertex_container.insert(i, q);
      }
      return i;
    }

    /// Inserts the vertex `q` before `i`. The return value points to
    /// the inserted vertex.
    Vertex_iterator insert(Vertex_circulator i, const Vertex_handle& q)
    {
      return this->insert(i.mod_iterator(), q);
    }

    /// Inserts the vertices in the range `[first, last)`
    /// before `i`.  The value type of points in the range
    /// `[first,last)` must be `Point_2`.
    template <class InputIterator>
    void insert(Vertex_iterator i, InputIterator first, InputIterator last)
    {
      vertex_container.insert(i, first, last);
      // TODO: propagate to Augmented Mesh
      // TODO: check if legal
    }

    /// Inserts the vertices in the range `[first, last)`
    /// before `i`.  The value type of points in the range
    /// `[first,last)` must be `Point_2`.
    template <class InputIterator>
    void insert(Vertex_circulator i, InputIterator first, InputIterator last)
    {
      this->insert(i.mod_iterator(), first, last);
    }

    // /// Has the same semantics as `p.insert(p.vertices_end(), q)`.
    // void push_back(const Vertex_handle& x) { vertex_container.insert(vertex_container.end(), x); }
    //
    // /// Erases the vertex pointed to by `i`.
    // Vertex_iterator erase(Vertex_iterator i) { return vertex_container.erase(i); }
    //
    // /// Erases the vertex pointed to by `i`.
    // Vertex_circulator erase(Vertex_circulator i)
    // {
    //   auto it = vertex_container.erase(i.mod_iterator());
    //   if(it == vertex_container.end())
    //   {
    //     it = vertex_container.begin();
    //   }
    //   return Vertex_circulator(&vertex_container, it);
    // }
    //
    // /// Erases the vertices in the range `[first, last)`.
    // Vertex_iterator erase(Vertex_iterator first, Vertex_iterator last) { return vertex_container.erase(first, last); }
    //
    // /// Erases the vertices in the range `[first, last)`.
    // void clear() { vertex_container.clear(); }

    /// Reverses the orientation of the polygon. The vertex pointed to
    ///  by `p.vertices_begin()` remains the same.
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

  private:
    MultiVertex_t vertex_container;
    AUM::Augmented_Mesh* pMesh_;
    uint cluster_id_;
  };


} // namespace CC

#endif // DELAUNAY_CLUSTERING_CLUSTER_HPP
