/// \file falaise/snemo/datamodels/two_arms_trajectory_pattern.h
/* Author(s) :    François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2021-11-09
 * Last modified: 2021-11-09
 *
 * Description: Two arms trajectory pattern
 */

#ifndef FALAISE_SNEMO_DATAMODEL_TWO_ARMS_TRAJECTORY_PATTERN_H
#define FALAISE_SNEMO_DATAMODEL_TWO_ARMS_TRAJECTORY_PATTERN_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/geomtools:
#include <geomtools/helix_3d.h>

// This project:
#include <falaise/snemo/datamodels/base_trajectory_pattern.h>
#include <falaise/snemo/geometry/two_arms_curve_3d.h>

namespace snemo {

  namespace datamodel {

    /// \brief Two arms (line/helix) trajectory pattern
    class two_arms_trajectory_pattern
      : public base_trajectory_pattern
    {
    public:
      
      /// Return pattern identifier of the pattern
      static const std::string & pattern_id();

      /// Default constructor
      two_arms_trajectory_pattern();

      /// Destructor
      virtual ~two_arms_trajectory_pattern() = default;

      /// Get a reference to the mutable helix embedded model
      snemo::geometry::two_arms_curve_3d & get_two_arms();

      /// Get a const reference to the mutable helix embedded model
      const snemo::geometry::two_arms_curve_3d & get_two_arms() const;

      /// Return the reference to the 1D shape associated to the trajectory
      virtual const geomtools::i_shape_1d & get_shape() const override;

      geomtools::vector_3d get_first() const override;

      geomtools::vector_3d get_first_direction() const override;

      geomtools::vector_3d get_last() const override;

      geomtools::vector_3d get_last_direction() const override;

      unsigned int number_of_kinks() const override;
  
      geomtools::vector_3d get_kink(unsigned int kink_index_) const override;

      geomtools::vector_3d get_kink_direction(unsigned int kink_index_,
                                              direction_type dir_) const override;

    private:
      
      snemo::geometry::two_arms_curve_3d _two_arms_{}; //!< The two arms embedded model

      DATATOOLS_SERIALIZATION_DECLARATION()
    };

  }  // end of namespace datamodel

}  // end of namespace snemo

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(snemo::datamodel::two_arms_trajectory_pattern,
                        "snemo::datamodel::two_arms_trajectory_pattern")

#endif // FALAISE_SNEMO_DATAMODEL_TWO_ARMS_TRAJECTORY_PATTERN_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
