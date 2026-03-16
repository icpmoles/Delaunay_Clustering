//
// Created by icpmoles on 21/01/26.
//

#ifndef TRIANGULATION_2_EXAMPLES_CLUSTERING_H
#define TRIANGULATION_2_EXAMPLES_CLUSTERING_H


#include "Convexification/utils.hpp"


namespace AUM // Augmented Mesh
{


  class Augmented_Mesh
  {
  public:
    /**
     *
     * @param cdt Starting Constrained Delauney Triangulation
     * The New Object of the class will keep a copy inside.
     * It will automatically calculate the area of the internal triangles.
     */
    Augmented_Mesh(CDT cdt)
    {
      this->cdt_ = cdt;
      populate_properties_();
    };

    /**
     *
     * @param i  Index of the face (obtained by get_description(FaceHandle)->Face_Id )
     * @return Face_Description struct
     */
    [[nodiscard]] Face_Description get_face_description(size_t i) const;

    CDT* get_cdt_ptr() { return &cdt_; }

    /**
     *
     * @param description Struct containing the new description of the i-th face
     * @param i face_id that it's replacing
     * @return true if the face exists, false if there's an error assigning
     */
    void set_face_description(const Face_Description& description, size_t i);

    /**
     *
     * @param description Struct containing the new description of the i-th face
     * @param f  facehandle of the subject it's replacing
     * @return true if the face exists, false if there's an error assigning
     */
    void set_face_description(const Face_Description& description, Face_handle f);


    Face_Description* get_face_description(Face_handle f);

    /**
     *
     * @param destination Where to save the Face_Handle if the procedure is successful
     * @return if the call was successful
     */
    bool get_inlier_face_(Face_handle& destination) const;

    Face_handle get_cdt_faces_begin() { return this->cdt_.all_faces_begin(); }

    Face_handle get_cdt_faces_end() { return this->cdt_.all_faces_end(); }


    std::vector<Face_Description*> get_face_neighbours(Face_handle f);
    //
    // std::vector<Face_Description*> get_face_neighbours(uint i);

  private:
    void populate_properties_();


    size_t get_vector_idx_(Face_handle f);

    void print_face_info(Face_handle f, bool print_vertexes);


    /**
     * Resets the Distance value of the unassigned faces.
     * @return number of faces still not assigned to a cluster
     */
    size_t reset_unassigned_faces();

    CDT get_cdt() { return this->cdt_; }

    CDT cdt_;
    std::vector<Face_Description> Faces_Properties_;

    uint faces_ = 0;
    uint inlier_faces_ = 0;
  };

  inline Face_Description Augmented_Mesh::get_face_description(size_t i) const { return this->Faces_Properties_[i]; }

  inline void Augmented_Mesh::set_face_description(const Face_Description& description, size_t i)
  {
    if(i > Faces_Properties_.size())
      // return false;
      Faces_Properties_.at(i) = description;
    // return true;
  }
  inline void Augmented_Mesh::set_face_description(const Face_Description& description, Face_handle f)
  {
    set_face_description(description, get_vector_idx_(f));
  }


  inline void Augmented_Mesh::populate_properties_()
  {
    const size_t n_faces = cdt_.number_of_faces();
    Faces_Properties_.clear();
    Faces_Properties_.reserve(n_faces);
    std::pair<uint, uint> stats = UTILS::get_faces_count(this->cdt_);
    size_t i = 0; // free face counter
    for(Face_handle f : this->cdt_.all_face_handles())
    {

      // the new faces are shuffled to have :
      // lower #inliner_face_ are all inliers
      // upper #inlier_face_+1 -> faces_ are the outliers
      Faces_Properties_.push_back(
        (Face_Description){.Face_Id = f->is_in_domain() ? inlier_faces_ : stats.second - inlier_faces_,
                           .Area = UTILS::get_area(f),
                           .is_Area_Calculated = true,
                           .is_Face_Assigned = true,
                           .is_InDomain = f->is_in_domain(),
                           .Distance = f->is_in_domain() ? UNEXPLORED_VALUE : OBSTACLE_VALUE});

      f->set_time_stamp(i);
      i++;
      if(f->is_in_domain())
      {
        inlier_faces_++;
      }
    }
    faces_ = stats.first + 1;
    inlier_faces_ = stats.second + 1;
  }

  inline bool Augmented_Mesh::get_inlier_face_(Face_handle& destination) const
  {
    Face_handle tentative = this->cdt_.all_faces_begin();
    const Face_handle last = this->cdt_.all_faces_end();

    do
    {
      // UTILS::print_triangle_vertices(tentative);
      if(tentative->is_in_domain())
      {
        destination = tentative;
        // std::cout << "Found Inlier!" << std::endl << std::endl;

        return true;
      }
      else
      {
        // std::cout << "Tested negative:" << std::endl;
      }
    }
    while(++tentative != last);
    return false;
  }

  std::vector<Face_Description*> Augmented_Mesh::get_face_neighbours(Face_handle f)
  {
    std::vector<Face_Description*> neighbours;
    for(int i = 0; i < 3; i++)
    {
      Face_handle candidate = f->neighbor(i);
      if(get_face_description(candidate)->is_InDomain)
      {
        neighbours.push_back(get_face_description(candidate));
      }
      else
      {
        neighbours.push_back(nullptr);
      }
    }
    return neighbours;
  }
  // inline std::vector<Face_Description*> Augmented_Mesh::get_face_neighbours(uint i)
  // {
  //   return get_face_neighbours();
  // }

  inline size_t Augmented_Mesh::get_vector_idx_(const Face_handle f) { return get_face_description(f)->Face_Id; }

  inline Face_Description* Augmented_Mesh::get_face_description(const Face_handle f)
  {
    const size_t i = f->time_stamp();
    return &Faces_Properties_[i];
  }


  inline void Augmented_Mesh::print_face_info(const Face_handle f, bool print_vertexes)
  {
    Face_Description* fd = get_face_description(f);
    std::cout << "Face_Id: " << fd->Face_Id << " (" << fd->is_Face_Assigned << ")" << std::endl;
    std::cout << "Cluster_Id: " << fd->Cluster_Id << " (" << fd->is_Cluster_Assigned << ")" << std::endl;
    std::cout << "Area: " << fd->Area << " (" << fd->is_Area_Calculated << ")" << std::endl;
    std::cout << "Distance: " << fd->Distance << " (" << f->is_in_domain() << ")" << std::endl;
    if(print_vertexes)
      UTILS::print_triangle_vertices(f);
  }

  inline size_t Augmented_Mesh::reset_unassigned_faces()
  {
    size_t i = 0;
    for(const Face_handle f : this->cdt_.all_face_handles())
    {
      if(get_face_description(f)->is_Cluster_Assigned == false)
      {
        i++;
        get_face_description(f)->Distance = f->is_in_domain() ? UNEXPLORED_VALUE : OBSTACLE_VALUE;
      }
    }

    return i;
  }

} // namespace AUM

#endif // TRIANGULATION_2_EXAMPLES_CLUSTERING_H
