//! \file falaise/snemo/physics_model/calorimeter_om_response_model.h
// OM model in SuperNEMO calorimeter(s)

#ifndef FALAISE_SNEMO_PHYSICS_MODEL_CALORIMETER_OM_RESPONSE_MODEL_H
#define FALAISE_SNEMO_PHYSICS_MODEL_CALORIMETER_OM_RESPONSE_MODEL_H

// Standard library:
#include <iostream>
#include <fstream>
#include <map>

// - Bayeux:
#include <bayeux/geomtools/geom_id.h>
#include <bayeux/geomtools/manager.h>

// Third party
// - Bayeux/datatools
#include <CLHEP/Units/SystemOfUnits.h>

namespace snemo {

  namespace physics_model {

    /// \brief Calorimeter OM model
    class calorimeter_om_response_model
    {
    public:

      calorimeter_om_response_model() = default;

      void set_energy_resolution(const double);

      void set_alpha_quenching(const double p0_, const double p1_, const double p2_);

      void set_relaxation_time(const double relaxation_time_);

			double get_energy_resolution() const;

			double get_relaxation_time() const;

			double get_alpha_quenching_factor(const double energy_) const;

    private:
      
      double _energyResolution_ =  8.0 * CLHEP::perCent;  //!< Energy resolution for electrons at 1 MeV
			double _alphaQuenching_[3] = {77.4, 0.639, 2.34}; //!< Parameters for alpha quenching
			double _relaxationTime_ = 6.0 * CLHEP::ns; //!< Scintillator relaxation time
      
		};
    
	} // end of namespace physics_model
  
} // end of namespace snemo

#endif // FALAISE_SNEMO_PHYSICS_MODEL_CALORIMETER_OM_RESPONSE_MODEL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
