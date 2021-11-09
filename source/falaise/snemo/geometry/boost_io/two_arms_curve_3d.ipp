// -*- mode: c++ ; -*-
/// \file falaise/snemo/geometry/two_arms_curve_3d.ipp

#ifndef FALAISE_SNEMO_GEOMETRY_TWO_ARMS_CURVE_3D_IPP
#define FALAISE_SNEMO_GEOMETRY_TWO_ARMS_CURVE_3D_IPP 1

// Ourselves:
#include <falaise/snemo/geometry/two_arms_curve_3d.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
// - Bayeux/datatools:
#include <bayeux/datatools/i_serializable.ipp>
#include <bayeux/geomtools/line_3d.ipp>
#include <bayeux/geomtools/helix_3d.ipp>

namespace snemo {

  namespace geometry {

    /// Serialization method
    template <class Archive>
    void two_arms_curve_3d::serialize(Archive& ar, const unsigned int /* version */) {
      ar& DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
      ar& boost::serialization::make_nvp("first_arm.code", _first_arm_.code);
      if (_first_arm_.code == SHAPE_CODE_LINE) {
        ar& boost::serialization::make_nvp("first_arm.line",_first_arm_.line);
      }
      if (_first_arm_.code == SHAPE_CODE_HELIX) {
        ar& boost::serialization::make_nvp("first_arm.helix",_first_arm_.helix);
      }
      ar& boost::serialization::make_nvp("second_arm.code", _second_arm_.code);
      if (_second_arm_.code == SHAPE_CODE_LINE) {
        ar& boost::serialization::make_nvp("second_arm.line",_second_arm_.line);
      }
      if (_second_arm_.code == SHAPE_CODE_HELIX) {
        ar& boost::serialization::make_nvp("second_arm.helix",_second_arm_.helix);
      }
    }

  }  // end of namespace geometry

}  // end of namespace snemo

#endif  // FALAISE_SNEMO_GEOMETRY_TWO_ARMS_CURVE_3D_IPP

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
