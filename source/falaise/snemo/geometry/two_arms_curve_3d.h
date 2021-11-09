/// \file falaise/snemo/geometry/two_arms_curve_3d.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2021-11-09
 * Last modified : 2021-11-09
 *
 * Copyright (C) 2021 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   Curve made of two connected arms of arbitrary 1d-shapes
 *
 */

#ifndef FALAISE_SNEMO_GEOMETRY_TWO_ARMS_CURVE_3D_H
#define FALAISE_SNEMO_GEOMETRY_TWO_ARMS_CURVE_3D_H 1

// Standard library

// - Bayeux
#include <bayeux/geomtools/i_shape_1d.h>
#include <bayeux/geomtools/i_wires_3d_rendering.h>
#include <bayeux/geomtools/line_3d.h>
#include <bayeux/geomtools/helix_3d.h>

namespace snemo {

  namespace geometry {

    /// \brief A sequence of two connected curves
    class two_arms_curve_3d
      : public geomtools::i_shape_1d
      , public geomtools::i_wires_3d_rendering
    {
    public:

      /// Return the identifier of the polyline 3D-object
      static const std::string & two_arms_label();

      /// Return the name of the shape
      std::string get_shape_name() const override;

      enum shape_code_type
        {
         SHAPE_CODE_INVALID = 0,
         SHAPE_CODE_LINE    = 1,
         SHAPE_CODE_HELIX   = 2
        };

      enum path_arm_type
        {
         PATH_FIRST_ARM = datatools::bit_mask::bit00,
         PATH_SECOND_ARM = datatools::bit_mask::bit01
      };

      static std::string to_string(shape_code_type);
      
      struct curve_info_type
      {
        shape_code_type     code = SHAPE_CODE_INVALID;
        geomtools::line_3d  line;
        geomtools::helix_3d helix;
      };

      /// Check if the shape is valid
      bool is_valid() const override;

      /// Default constructor
      two_arms_curve_3d();

      /// Constructor
      two_arms_curve_3d(double tolerance_);

      /// Destructor
      ~two_arms_curve_3d() override;

      void reset();

      void set_first_arm(const geomtools::line_3d & line_);

      void set_first_arm(const geomtools::helix_3d & helix_);

      void set_second_arm(const geomtools::line_3d & line_);

      void set_second_arm(const geomtools::helix_3d & helix_);

      bool is_first_arm_line() const;

      bool is_first_arm_helix() const;

      bool is_second_arm_line() const;

      bool is_second_arm_helix() const;

      geomtools::vector_3d get_first_point() const;

      geomtools::vector_3d get_last_point() const;

      geomtools::vector_3d get_kink_point() const;

      const geomtools::line_3d & get_first_arm_line() const;

      const geomtools::line_3d & get_second_arm_line() const;
      
      const geomtools::helix_3d & get_first_arm_helix() const;

      const geomtools::helix_3d & get_second_arm_helix() const;

      /// Check if a point belongs to the curve
      bool is_on_curve(const geomtools::vector_3d & position_,
                       double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

      /// Return the direction at some position along the curve
      geomtools::vector_3d get_direction_on_curve(const geomtools::vector_3d & position_) const override;

      /// Return the length of the curve
      double get_length(uint32_t flags_ = geomtools::PATH_ALL_BITS) const override;

      /// Return the number of paths
      unsigned int get_number_of_paths() const override;

      /// Generate a sequence of polylines for wires 3D rendering
      void generate_wires_self(geomtools::wires_type & wires_,
                               uint32_t options_ = 0) const override;

      /// Smart print
      void tree_dump(std::ostream & out_         = std::clog,
                     const std::string & title_  = "",
                     const std::string & indent_ = "",
                     bool inherit_ = false) const override;

    private:

      curve_info_type _first_arm_;
      curve_info_type _second_arm_;

      //! Serialization support
      DATATOOLS_SERIALIZATION_DECLARATION()
      
    };
    
  }  // end of namespace geometry

}  // end of namespace snemo

BOOST_CLASS_EXPORT_KEY2(snemo::geometry::two_arms_curve_3d, "snemo::geometry::two_arms_curve_3d")

#endif // FALAISE_SNEMO_GEOMETRY_TWO_ARMS_CURVE_3D_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
