//! \file falaise/snemo/physics_model/calorimeter_om_nulc_model.h
// OM non-uniform light collection model in SuperNEMO calorimeter(s)

#ifndef FALAISE_SNEMO_PHYSICS_MODEL_CALORIMETER_OM_NULC_MODEL_H
#define FALAISE_SNEMO_PHYSICS_MODEL_CALORIMETER_OM_NULC_MODEL_H

// Standard library:
#include <vector>
#include <limits>

// Third party
// - Bayeux/datatools
#include <CLHEP/Units/SystemOfUnits.h>
// GSL:
#include <gsl/gsl_math.h>
#include <gsl/gsl_interp2d.h>
#include <gsl/gsl_spline2d.h>

namespace snemo {

  namespace physics_model {

    /// \brief Calorimeter OM model
    class calorimeter_om_nulc_model
    {
    public:

      calorimeter_om_nulc_model();
      
      ~calorimeter_om_nulc_model();

      struct z_data
      {
	void init(const size_t nb_x_, const size_t nb_y_);
	double get(const size_t x_index_, const size_t y_index_) const;
	double set(const size_t x_index_, const size_t y_index_, const double value_);
	std::vector<double> x;
	std::vector<double> y;
	std::vector<double> z;
      };

      
    private:

      double _z_min_ = 0.0;
      double _z_max_ = std::numeric_limits<double>::quiet_NaN();
      double _x_step_ = 5.0 * CLHEP::mm;
      double _y_step_ = 5.0 * CLHEP::mm;
      double _z_step_ = 5.0 * CLHEP::mm;
      size_t _nb_x_ = 0;
      size_t _nb_y_ = 0;
      size_t _nb_z_ = 0;
      std::vector<z_data> _map3d_;
      const gsl_interp2d_type * T = nullptr;
      gsl_spline2d * spline = nullptr;
      gsl_interp_accel * xacc = nullptr;
      gsl_interp_accel * yacc = nullptr;
      
    };
     
  } // end of namespace physics_model
  
} // end of namespace snemo

#endif // FALAISE_SNEMO_PHYSICS_MODEL_CALORIMETER_OM_NULC_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
