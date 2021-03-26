// -*- mode: c++ ; -*-
/// \file falaise/snemo/datamodels/tracker_clustering_data.ipp

#ifndef FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTERING_DATA_IPP
#define FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTERING_DATA_IPP 1

// Ourselves:
#include <falaise/snemo/datamodels/tracker_clustering_data.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/vector.hpp>
// - Bayeux/datatools:
#include <datatools/i_serializable.ipp>
#include <datatools/properties.ipp>

// This project:
#include <falaise/snemo/datamodels/boost_io/tracker_clustering_solution.ipp>

namespace snemo {

namespace datamodel {

/// Serialization
template <class Archive>
void tracker_clustering_data::serialize(Archive& ar_,
                                        const unsigned int version_) {
  ar_& DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  ar_& boost::serialization::make_nvp("solutions", solutions_);
  ar_& boost::serialization::make_nvp("default_solution", default_);
  ar_& boost::serialization::make_nvp("auxiliaries", _auxiliaries_);
  // Support new attributes from version 1 with backward compatibility:
  if (version_ == 0 and Archive::is_loading::value) {
    // The attributes added in class version=1 are not loaded from 
    // file class version=0 so we force invalid values in the current
    // class version:
    reset_chi2(); 
    reset_ndof();
  } else {
    ar_& boost::serialization::make_nvp("chi2", chi2_);
    ar_& boost::serialization::make_nvp("ndof", ndof_);
  } 
}

}  // end of namespace datamodel

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_DATAMODELS_TRACKER_CLUSTERING_DATA_IPP
