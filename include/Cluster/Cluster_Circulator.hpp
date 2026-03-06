//
// Created by icpmoles on 04/02/26.
//

#ifndef TRIANGULATION_2_EXAMPLES_CLUSTER_CIRCULATOR_HPP
#define TRIANGULATION_2_EXAMPLES_CLUSTER_CIRCULATOR_HPP

#include "Convexification/utils.hpp"

namespace CC
{

  class Cluster_circulator
  {
  public:
    typedef MultiVertex_t Container_Type;
    typedef Cluster_circulator Self;
    typedef CGAL::Circulator_from_container<Container_Type> Mutable;
    // typedef MultiVertex_t Container;
    typedef typename Container_Type::iterator iterator;
    typedef typename Container_Type::const_iterator const_iterator;
    typedef typename Container_Type::value_type value_type;
    typedef typename Container_Type::const_reference reference;
    typedef const value_type* pointer;
    typedef typename Container_Type::size_type size_type;
    typedef typename Container_Type::difference_type difference_type;

    typedef std::iterator_traits<const_iterator> ITraits;
    typedef typename ITraits::iterator_category Icategory;
    typedef CGAL::I_Circulator_from_iterator_traits<Icategory> CTraits;
    typedef typename CTraits::iterator_category iterator_category;

  private:
    const Container_Type* ctnr;
    iterator i;


  public:
    // CREATION

    Cluster_circulator() : ctnr(nullptr) {}
    Cluster_circulator(Container_Type* c) : ctnr(c), i(c->begin()) {}
    Cluster_circulator(const Container_Type* c, iterator j) : ctnr(c), i(j) {}
    Cluster_circulator(const Mutable& c) : ctnr(c.container()), i(c.current_iterator()) {}

    // OPERATIONS

    bool operator==(std::nullptr_t CGAL_assertion_code(p)) const
    {
      CGAL_assertion(p == nullptr);
      return (ctnr == nullptr) || (ctnr->begin() == ctnr->end());
    }
    bool operator!=(std::nullptr_t p) const { return !(*this == p); }
    bool operator==(const Self& c) const { return i == c.i; }
    bool operator!=(const Self& c) const { return !(*this == c); }
    reference operator*() const
    {
      CGAL_assertion(ctnr != nullptr);
      CGAL_assertion(current_iterator() != ctnr->end());
      return *i;
    }
    Vertex_handle get_vertex() { return *i; }
    Point get_point() { return get_vertex()->point(); }

  private:
    // For cases where iterator is a pointer.
    template <typename T>
    static pointer deref(const T& t)
    {
      return t.operator->();
    }
    template <typename T>
    static pointer deref(T* t)
    {
      return t;
    }

  public:
    pointer operator->() const
    {
      CGAL_assertion(ctnr != nullptr);
      CGAL_assertion(current_iterator() != ctnr->end());
      return deref(i);
    }
    Self& operator++()
    {
      CGAL_assertion(ctnr != nullptr);
      CGAL_assertion(current_iterator() != ctnr->end());
      ++i;
      if(current_iterator() == ctnr->end())
        i = const_cast<Container_Type*>(ctnr)->begin();
      return *this;
    }
    Self operator++(int)
    {
      Self tmp = *this;
      ++*this;
      return tmp;
    }
    Self& operator--()
    {
      CGAL_assertion(ctnr != nullptr);
      CGAL_assertion(current_iterator() != ctnr->end());
      if(current_iterator() == ctnr->begin())
        i = const_cast<Container_Type*>(ctnr)->end();
      --i;
      return *this;
    }
    Self operator--(int)
    {
      Self tmp = *this;
      --*this;
      return tmp;
    }
    Self& operator+=(difference_type n)
    {
      CGAL_assertion(ctnr != nullptr);
      CGAL_assertion(current_iterator() != ctnr->end());
      typename Container_Type::difference_type j = current_iterator() - ctnr->begin();
      typename Container_Type::difference_type size = ctnr->size();
      CGAL_assertion(j >= 0);
      CGAL_assertion(size >= 0);
      j = CGAL::non_negative_mod(j + n, size);
      CGAL_assertion(j >= 0);
      CGAL_assertion(j < size);
      i = const_cast<Container_Type*>(ctnr)->begin() + j;
      return *this;
    }
    Self operator+(difference_type n) const
    {
      Self tmp = *this;
      return tmp += n;
    }

    Self& operator-=(difference_type n) { return operator+=(-n); }
    Self operator-(difference_type n) const
    {
      Self tmp = *this;
      return tmp += -n;
    }
    difference_type operator-(const Self& c) const
    {
      CGAL_assertion(ctnr != nullptr);
      CGAL_assertion(c.ctnr != nullptr);
      return i - c.i;
    }
    reference operator[](difference_type n) const
    {
      Self tmp = *this;
      tmp += n;
      return *tmp;
    }
    const_iterator current_iterator() const { return i; }
    iterator mod_iterator() const { return i; }
    // Self min_circulator() const { return Self(ctnr); }
    const Container_Type* container() const { return ctnr; }

    /**
     *
     * @return pointer to previous element of circulator
     * @note Yeah, the implementation is not really brilliant but it gets the job done
     */
    Vertex_handle get_prev() const
    {
      CGAL_assertion(ctnr != nullptr);
      CGAL_assertion(current_iterator() != ctnr->end());
      Self tmp = *this;
      Vertex_handle v = *(tmp -= 1);
      tmp += 1;
      return v;
    }

    /**
     *
     * @return pointer to next element of circulator
     * @note Yeah, the implementation is not really brilliant but it gets the job done
     */
    Vertex_handle get_next() const
    {
      CGAL_assertion(ctnr != nullptr);
      CGAL_assertion(current_iterator() != ctnr->end());
      Self tmp = *this;
      Vertex_handle v = *(tmp += 1);
      tmp -= 1;
      return v;
    }
  };


} // namespace CC


#endif // TRIANGULATION_2_EXAMPLES_CLUSTER_CIRCULATOR_HPP
