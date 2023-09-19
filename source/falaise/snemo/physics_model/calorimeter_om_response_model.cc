// -*- mode: c++ ; -*-
/// \file falaise/snemo/physics_model/calorimeter_om_response_model.cc

// Ourselves:
#include <falaise/snemo/physics_model/calorimeter_om_response_model.h>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace physics_model {

    // calorimeter_om_response_model::calorimeter_om_response_model()
    // {
    //   _energyResolution_ = 8.0 * CLHEP::perCent;
    //   _alphaQuenching_[0] = 77.4;
    //   _alphaQuenching_[1] = 0.639;
    //   _alphaQuenching_[2] = 2.34;
    //   _relaxationTime_ = 6.0 * CLHEP::ns;     
    //  return;
    // }

    void calorimeter_om_response_model::set_energy_resolution(const double er_)
    {
      _energyResolution_ = er_;
      return;
    }

    void calorimeter_om_response_model::set_alpha_quenching(const double p0_,
							    const double p1_,
							    const double p2_)
    {
      _alphaQuenching_[0] = p0_;
      _alphaQuenching_[1] = p1_;
      _alphaQuenching_[2] = p2_;
     return;
    }

    void calorimeter_om_response_model::set_relaxation_time(const double relaxation_time_)
    {
      _relaxationTime_ = relaxation_time_;
      return;
    }

    double calorimeter_om_response_model::get_energy_resolution() const
    {
      return _energyResolution_;
    }

    double calorimeter_om_response_model::get_relaxation_time() const
    {
      return _relaxationTime_;
    }
    
    double calorimeter_om_response_model::get_alpha_quenching_factor(const double energy_) const
    {
      const double energy_MeV = energy_ / CLHEP::MeV;
      const double mod_energy = 1.0 / (_alphaQuenching_[1] * energy_MeV + 1.0);
      const double quenchingFactor =
	-_alphaQuenching_[0] *
	(std::pow(mod_energy, _alphaQuenching_[2]) - std::pow(mod_energy, _alphaQuenching_[2] / 2.0));
      return quenchingFactor;
    }

  } // end of namespace physics_model
  
} // end of namespace snemo
