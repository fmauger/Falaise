// falaise/snemo/datamodels/two_arms_trajectory_pattern.cc

// Ourselves:
#include <falaise/snemo/datamodels/two_arms_trajectory_pattern.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace snemo {

  namespace datamodel {

    // Serial tag for datatools::i_serializable interface :
    DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(two_arms_trajectory_pattern,
                                                      "snemo::datamodel::two_arms_trajectory_pattern")

    // static
    const std::string &two_arms_trajectory_pattern::pattern_id() {
      static const std::string _id("two_arms");
      return _id;
    }

    two_arms_trajectory_pattern::two_arms_trajectory_pattern()
      : base_trajectory_pattern(two_arms_trajectory_pattern::pattern_id()) {
    }

    snemo::geometry::two_arms_curve_3d &
    two_arms_trajectory_pattern::get_two_arms() { return _two_arms_; }

    const snemo::geometry::two_arms_curve_3d &
    two_arms_trajectory_pattern::get_two_arms() const { return _two_arms_; }

    // override
    const geomtools::i_shape_1d &
    two_arms_trajectory_pattern::get_shape() const {
      return dynamic_cast<const geomtools::i_shape_1d &>(_two_arms_);
    }

    // override
    geomtools::vector_3d two_arms_trajectory_pattern::get_first() const
    {
      return _two_arms_.get_first_point();
    }

    // override
    geomtools::vector_3d two_arms_trajectory_pattern::get_first_direction() const
    {
      return _two_arms_.get_direction_on_curve(get_first());
    }

    // override
    geomtools::vector_3d two_arms_trajectory_pattern::get_last() const
    {
      return _two_arms_.get_last_point();
    }
    
    // override
    geomtools::vector_3d two_arms_trajectory_pattern::get_last_direction() const
    {
      return _two_arms_.get_direction_on_curve(get_last());
    }

    // override
    unsigned int two_arms_trajectory_pattern::number_of_kinks() const
    {
      return 1;
    }
  
    // override
    geomtools::vector_3d two_arms_trajectory_pattern::get_kink(unsigned int kink_index_) const
    {
      DT_THROW_IF(kink_index_ != 0, std::range_error, "No kink at index " << kink_index_ << "!");
      return _two_arms_.get_kink_point();
    }

    // override
    geomtools::vector_3d two_arms_trajectory_pattern::get_kink_direction(unsigned int kink_index_,
                                                                         direction_type dir_) const
    {
      DT_THROW_IF(kink_index_ != 0, std::range_error, "No kink at index " << kink_index_ << "!");
      DT_THROW_IF(dir_ == DIRECTION_INVALID, std::logic_error, "Invalid direction!");
      DT_THROW_IF(!_two_arms_.is_valid(), std::range_error, "Two arms curve is not valid!");
      if (dir_ == DIRECTION_FORWARD) {
        if (_two_arms_.is_first_arm_line()) {
          return _two_arms_.get_first_arm_line().get_direction_on_curve(_two_arms_.get_first_arm_line().get_last());
        }
        return _two_arms_.get_first_arm_helix().get_direction_on_curve(_two_arms_.get_first_arm_helix().get_last());
      }
      if (_two_arms_.is_second_arm_line()) {
        return _two_arms_.get_second_arm_line().get_direction_on_curve(_two_arms_.get_second_arm_line().get_first());
      } 
      return _two_arms_.get_second_arm_helix().get_direction_on_curve(_two_arms_.get_second_arm_helix().get_first());
    }
 
  }  // end of namespace datamodel

}  // end of namespace snemo

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
