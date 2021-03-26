// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/boost_io/tcd_recons_info.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_TCD_RECONS_INFO_IPP
#define FALAISE_SNEMO_DATAMODELS_TCD_RECONS_INFO_IPP

// Ourselves:
#include <falaise/snemo/datamodels/tcd_recons_info.h>
// - Boost:
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp> // std::string serialization support
#include <boost/serialization/vector.hpp> // std::vector serialization support

namespace snemo {
namespace datamodel {
template <class Archive>
void tcd_recons_info::serialize(Archive& ar_, const unsigned int /*version_*/) {
  ar_& boost::serialization::make_nvp("flag",      flag_);
  ar_& boost::serialization::make_nvp("nb",        nb_);
  ar_& boost::serialization::make_nvp("value",     value_);
  ar_& boost::serialization::make_nvp("label",     label_);
  ar_& boost::serialization::make_nvp("residuals", residuals_);
}
}  // end of namespace datamodel
}  // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_TCD_RECONS_INFO_IPP
