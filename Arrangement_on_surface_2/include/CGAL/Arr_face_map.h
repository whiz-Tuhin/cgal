// Copyright (c) 2005  Tel-Aviv University (Israel).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL$
// $Id$
// 
//
// Author(s)     : Ron Wein          <wein@post.tau.ac.il>
#ifndef CGAL_ARR_FACE_MAP_H
#define CGAL_ARR_FACE_MAP_H

/*! \file
 * Definition of the Arr_face_index_map<Arrangement> class.
 */

#include <CGAL/Unique_hash_map.h>
#include <boost/property_map.hpp>
#include <boost/graph/properties.hpp>

CGAL_BEGIN_NAMESPACE

/*! \class
 * An auxiliary class that automatically maintains a mapping of the
 * arrangement faces to the indices 0, ..., (n -1), where n is the number
 * of faces in the arrangement.
 */
template <class Arrangement_>
class Arr_face_index_map : public Arr_observer<Arrangement_>
{
public:

  typedef Arrangement_                            Arrangement_2;
  typedef typename Arrangement_2::Face_handle     Face_handle;

  // Boost property type definitions:
  typedef boost::readable_property_map_tag        category;
  typedef unsigned int                            value_type;
  typedef value_type                              reference;
  typedef Face_handle                             key_type;

private:

  typedef Arr_face_index_map<Arrangement_2>       Self;
  typedef Arr_observer<Arrangement_2>             Base;

  typedef Unique_hash_map<Face_handle, unsigned int>     Index_map;

  // Data members:
  unsigned int                n_faces;     // The current number of faces.
  Index_map                   index_map;   // Mapping faces to indices.
  std::vector<Face_handle>    rev_map;     // Mapping indices to faces.

  enum {MIN_REV_MAP_SIZE = 32};

public:

  /*! Default constructor. */
  Arr_face_index_map () :
    Base (),
    n_faces (0),
    rev_map (MIN_REV_MAP_SIZE)
  {}

  /*! Constructor with an associated arrangement. */
  Arr_face_index_map (const Arrangement_2& arr) :
    Base (const_cast<Arrangement_2&> (arr))
  {
    _init();
  }

  /*! Copy constructor. */
  Arr_face_index_map (const Self& other) :
    Base (const_cast<Arrangement_2&> (*(other.arrangement())))
  {
    _init();
  }

  /*! Assignment operator. */
  Self& operator= (const Self& other)
  {
    if (this == &other)
      return (*this);

    this->detach();
    this->attach (const_cast<Arrangement_2&> (*(other.arrangement())));

    return (*this);
  }

  /*!
   * Get the index of a given face.
   * \param f A handle to the face.
   * \pre f is a valid face in the arrangement.
   */
  unsigned int operator[] (Face_handle f) const
  {
    return (index_map[f]);
  }

  /*!
   * Get the face given its index.
   * \param i The index of the face.
   * \pre i is less than the number of faces in the arrangement.
   */
  Face_handle face (const int i) const
  {
    CGAL_precondition (i < n_faces);

    return (rev_map[i]);
  }

  /// \name Notification functions, to keep the mapping up-to-date.
  //@{

  /*!
   * Update the mapping after the arrangement has been assigned with another
   * arrangement.
   */
  virtual void after_assign ()
  {
    _init();
  }

  /*!
   * Update the mapping after the arrangement is cleared.
   */
  virtual void after_clear (Face_handle /* u */)
  {
    _init();
  }

  /*!
   * Update the mapping after attaching to a new arrangement.
   */
  virtual void after_attach ()
  {
    _init();
  }

  /*!
   * Update the mapping after detaching the arrangement.
   */
  virtual void after_detach ()
  {
    n_faces = 0;
    index_map.clear();
  }

  /*!
   * Update the mapping after the creation of a new face is split from another
   * face.
   * \param f A handle to the existing face.
   * \param new_f A handle to the newly created face.
   */
  virtual void after_split_face (Face_handle /* f */,
                                 Face_handle new_f,
                                 bool /* is_hole */)
  {
    // Update the number of vertices.
    n_faces++;

    // If necessary, allocate memory for the reverse mapping.
    if (rev_map.size() > n_faces)
      rev_map.resize (2 * n_faces);

    // Update the mapping of the newly created face.
    index_map[new_f] = n_faces - 1;
    rev_map[n_faces - 1] = new_f;

    return;
  }

  /*!
   * Update the mapping before the merge of two faces.
   * \param f1 A handle to the face that is going to remain.
   * \param f2 A handle to the face that is about to be removed.
   */
  virtual void before_merge_face (Face_handle /* f1 */,
                                  Face_handle f2,
                                  typename
				  Arrangement_2::Halfedge_handle /* e */)
  {
    // Update the number of faces.
    n_faces--;
    
    // Reduce memory consumption in case the number of faces has
    // drastically decreased.
    if (2*n_faces < rev_map.size() && 
	rev_map.size() / 2 >= MIN_REV_MAP_SIZE)
    {
      rev_map.resize (rev_map.size() / 2);
    }

    // Get the current face index, and assign this index to the face
    // currently indexed (n - 1).
    unsigned int   index = index_map[f2];

    if (index == n_faces)
      return;
    
    Face_handle    last_f = rev_map[n_faces];
    index_map[last_f] = index;
    rev_map[index] = last_f;

    // Clear the reverse mapping for the last face.
    rev_map[n_faces] = Face_handle();

    return;
  }
  //@}

private:
  
  /*! Initialize the map for the given arrangement. */
  void _init ()
  {
    // Get the number of faces and allocate the reverse map accordingly.
    n_faces = this->arrangement()->number_of_faces();
    
    if (n_faces < MIN_REV_MAP_SIZE)
      rev_map.resize (MIN_REV_MAP_SIZE);
    else
      rev_map.resize (n_faces);

    // Clear the current mapping.
    index_map.clear();

    // Create the initial mapping. 
    typename Arrangement_2::Face_iterator     fit;
    Face_handle                               fh;
    unsigned int                              index = 0;

    for (fit = this->arrangement()->faces_begin();
	 fit != this->arrangement()->faces_end(); ++fit, ++index)
    {
      // Map the current face to the current index.
      fh = fit;
      index_map[fh] = index;
      rev_map[index] = fh;
    }

    return;
  }  

};

/*!
 * Get the index property-map function. Provided so that boost is able to
 * access the Arr_face_index_map above.
 * \param index_map The index map.
 * \param f A face handle.
 * \return The face index.
 */
template<class Arrangement>
unsigned int get (const CGAL::Arr_face_index_map<Arrangement>& index_map,
		  typename Arrangement::Face_handle f) 
{ 
  return (index_map[f]);
}

/*! \class
 * An auxiliary class that automatically maintains a mapping of the
 * arrangement faces to some property of a given type.
 */
template <class Arrangement_, class Type_>
class Arr_face_property_map
{
public:

  typedef Arrangement_                            Arrangement_2;
  typedef typename Arrangement_2::Face_handle     Face_handle;
  typedef Arr_face_index_map<Arrangement_2>       Index_map;

  // Boost property type definitions:
  typedef boost::read_write_property_map_tag      category;
  typedef Type_                                   value_type;
  typedef value_type&                             reference;
  typedef Face_handle                             key_type;

private:

  typedef Arr_face_property_map<Arrangement_2, value_type>   Self;

  // Data members:
  const Index_map                 *ind_map;      // The index map.
  value_type                      *props;        // The properties vector.
  bool                             owner;        // Should the vector be freed.

  /*! Assignment operator - not supported. */
  Self& operator= (const Self& );

public:

  /*! Constructor. */
  Arr_face_property_map (const Index_map& index_map) :
    ind_map (&index_map),
    owner (true)
  {
    props = new value_type [index_map.arrangement()->number_of_faces()];
  }

  /*! Copy constructor - performs shallow copy. */
  Arr_face_property_map (const Self& map) :
    ind_map (map.ind_map),
    props (map.props),
    owner (false)
  {}

  /*! Destructor. */
  ~Arr_face_property_map ()
  {
    if (owner)
      delete[] props;
  }

  /*! Get the property associated with a face (const version). */
  const value_type& operator[] (Face_handle f) const
  {
    return (props [(*ind_map)[f]]);
  }

  /*! Get the property associated with a face (non-const version). */
  value_type& operator[] (Face_handle f)
  {
    return (props [(*ind_map)[f]]);
  }

};

/*!
 * Get the index property-map function. Provided so that boost is able to
 * access the Arr_face_property_map above.
 * \param prop_map The property map.
 * \param f A face handle.
 * \return The face propery.
 */
template<class Arrangement, class Type>
const typename CGAL::Arr_face_property_map<Arrangement, Type>::value_type&
get (const CGAL::Arr_face_property_map<Arrangement, Type>& prop_map,
     typename Arrangement::Face_handle f) 
{ 
  return (prop_map[f]);
}

/*!
 * Put the index property-map function. Provided so that boost is able to
 * update the Arr_face_property_map above.
 * \param prop_map The property map.
 * \param f A face handle.
 * \param t The face propery.
 */
template<class Arrangement, class Type>
void put (CGAL::Arr_face_property_map<Arrangement, Type>& prop_map,
	  typename Arrangement::Face_handle f,
	  typename CGAL::Arr_face_property_map<Arrangement, Type>::
                                                                  value_type t)
{
  prop_map[f] = t;
  return;
}

CGAL_END_NAMESPACE

#endif
