// Copyright (c) 2006  Tel-Aviv University (Israel).
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
// Author(s)     : Ron Wein <wein@post.tau.ac.il>

#ifndef CGAL_ARR_BOUNDED_PLANAR_TOPOLOGY_TRAITS_2_IMPL_H
#define CGAL_ARR_BOUNDED_PLANAR_TOPOLOGY_TRAITS_2_IMPL_H

/*! \file
 * Member-function definitions for the
 * Arr_bounded_planar_topology_traits_2<GeomTraits> class.
 */

CGAL_BEGIN_NAMESPACE

//-----------------------------------------------------------------------------
// Assign the contents of another topology-traits class.
//
template <class GeomTraits, class Dcel_>
void Arr_bounded_planar_topology_traits_2<GeomTraits, Dcel_>::assign
    (const Self& other)
{
  // Assign the base class.
  Base::assign (other);

  // Go over the DCEL faces and locate the unbounded face.
  typename Dcel::Face_iterator         fit;
  
  unb_face = NULL;
  for (fit = this->m_dcel.faces_begin();
       fit != this->m_dcel.faces_end(); ++fit)
  {
    if (fit->is_unbounded())
    {
      unb_face = &(*fit);
      break;
    }
  }
  CGAL_assertion (unb_face != NULL);
  
  return;
}

//-----------------------------------------------------------------------------
// Initialize an empty DCEL structure.
//
template <class GeomTraits, class Dcel_>
void Arr_bounded_planar_topology_traits_2<GeomTraits, Dcel_>::init_dcel ()
{
  // Clear the current DCEL.
  this->m_dcel.delete_all();

  // Create the unbounded face.
  unb_face = this->m_dcel.new_face();

  unb_face->set_unbounded (true);
  unb_face->set_fictitious (false);

  return;
}

CGAL_END_NAMESPACE

#endif
