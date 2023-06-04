// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/two_arms_trajectory_pattern.ipp

#ifndef FALAISE_SNEMO_DATAMODEL_TWO_ARMS_TRAJECTORY_PATTERN_IPP
#define FALAISE_SNEMO_DATAMODEL_TWO_ARMS_TRAJECTORY_PATTERN_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/two_arms_trajectory_pattern.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>

// This project:
#include <falaise/snemo/datamodels/boost_io/base_trajectory_pattern.ipp>
#include <falaise/snemo/geometry/boost_io/two_arms_curve_3d.ipp>

namespace snemo {

namespace datamodel {

template <class Archive>
void two_arms_trajectory_pattern::serialize(Archive& ar, const unsigned int /* version */) {
  ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(base_trajectory_pattern);
  ar& boost::serialization::make_nvp("two_arms", _two_arms_);
}

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODEL_TWO_ARMS_TRAJECTORY_PATTERN_IPP
