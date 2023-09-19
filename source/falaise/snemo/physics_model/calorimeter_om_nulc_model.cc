// -*- mode: c++ ; -*-
/// \file falaise/snemo/physics_model/calorimeter_om_nulc_model.cc

// Ourselves:
#include <falaise/snemo/physics_model/calorimeter_om_nulc_model.h>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/exception.h>

namespace snemo {

  namespace physics_model {

		calorimeter_om_nulc_model::calorimeter_om_nulc_model()
		{
			
			T = gsl_interp2d_bilinear;
		}

		void calorimeter_om_nulc_model::z_data::init(const size_t nb_x_, const size_t nb_y_)
		{
			grid.assign(nb_x_ * nb_y_, std::numeric_limits<double>::quiet_NaN());
			return;
		}
		
		double calorimeter_om_nulc_model::z_data::get(const size_t x_index_, const size_t y_index_) const
		{
		}
		
		double calorimeter_om_nulc_model::z_data::set(const size_t x_index_, const size_t y_index_, const double value_)
		{
		}
				

      
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
