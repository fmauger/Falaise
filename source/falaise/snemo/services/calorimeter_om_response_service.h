//! \file falaise/snemo/services/calorimeter_om_response_service.h
// Falaise service for OM response in SuperNEMO calorimeter(s)

#ifndef SNEMO_CALORIMETER_CALORIMETER_OM_RESPONSE_SERVICE_H
#define SNEMO_CALORIMETER_CALORIMETER_OM_RESPONSE_SERVICE_H

// Standard library:
#include <iostream>
#include <fstream>
#include <map>
#include <vector>

// - Bayeux:
#include <bayeux/datatools/base_service.h>
#include <bayeux/geomtools/geom_id.h>
#include <bayeux/geomtools/manager.h>

// - Falaise:
#include <falaise/snemo/services/service_traits.h>
#include <falaise/snemo/rc/calorimeter_om_status.h>
#include <falaise/snemo/services/db_service.h>
#include <falaise/snemo/physics_model/calorimeter_om_response_model.h>

namespace snemo {
  
  /// \brief Calorimeter OM response service
  class calorimeter_om_response_service
    : public datatools::base_service
  {
  public:

    enum mode_type
      {
       MODE_DB = 0,
       MODE_FILES = 1,
       MODE_DEFAULT = MODE_FILES
      };
      
    calorimeter_om_response_service();

    virtual ~calorimeter_om_response_service() override;
    
    // Service interface:
    
    bool is_initialized() const override;

    // Example of configuration parameters:
    //
    // use_birks_cerenkov_correction : boolean = true
    // use_alpha_quenching : boolean = true
    // use_uniformity_correction : boolean = true
    // geometry_label : string = "geometry"
    // mode : string = "files"
    // files.om_regime_map : string as path = "@falaise:snemo/demonstrator/reconstruction/db/calorimeter_regime_database_v0.db"
    // files.pol3d_parameters_mwall_8inch : string as path = "@falaise:snemo/demonstrator/reconstruction/db/fit_parameters_10D_MW_8inch.db"
    // files.pol3d_parameters_mwall_5inch : string as path = "@falaise:snemo/demonstrator/reconstruction/db/fit_parameters_10D_MW_5inch.db"
    // files.pol3d_parameters_xwall : string as path = "@falaise:snemo/demonstrator/reconstruction/db/fit_parameters_10D_XW.db"
    // files.pol3d_parameters_gveto : string as path = "@falaise:snemo/demonstrator/reconstruction/db/fit_parameters_10D_GV.db"
    //
    int initialize(const datatools::properties &,
		   datatools::service_dict_type &) override;
    
    int reset() override;

    mode_type get_mode() const;

    bool has_geometry_manager() const;

    void set_geometry_manager(const geomtools::manager & geomgr_);
    
    // Get the OM response object associated to a given OM:
    physics_model::calorimeter_om_response_model
    get_om_response(const geomtools::geom_id & gid_,
		    const time::time_point & t_) const;

    /// Return the uniformity correction coefficient from calo hit position and correction parameters
    static double pol3d(const double * x_, const double * p_); 

    /// Return the effective geometry ID of a scintillator block
    static geomtools::geom_id get_effective_geom_id(const geomtools::geom_id & gid_);
    
    // Parse pol3d parameters file and return correction parameters
    //
    // Format:
    // Line # == 0: max power and unused value
    // 10           0                p[0] <= integer
    // Lines # > 0 : coeff + unused error on coeff
    // 1.00505      0.00886295       p[1] <= double precision real
    // 1.41045e-05  0.000110804      p[2]
    // 9.36913e-06  0.000110765      p[3]
    // -0.002333    0.000141964      p[4]
    // ...
    //
    static std::vector<double> parse_pol3d_parameters(const std::string & parameters_path_);

    enum class particle_type : uint8_t
      {
	electron = 0,
	positron = 1,
	gamma = 2,
	alpha = 3,
	muon = 4
      };

    double alpha_quenching_factor(const double energy_deposit_,
				  const geomtools::geom_id & gid_) const;

    double birks_cerenkov_energy_correction_factor(const double energy_deposit_,
						   const geomtools::geom_id & gid_) const;

    double uniformity_energy_correction_factor(const geomtools::geom_id & gid_,
					       const geomtools::vector_3d & position_) const;
    
  private:

    void _init_mode_db_(const datatools::properties &);

    void _terminate_mode_db_();

    void _init_mode_files_(const datatools::properties &);

    void _terminate_mode_files_();

    void _parse_calorimeter_regime_database_(const std::string & database_path_);
    
    bool _initialized_ = false;
    mode_type _mode_ = MODE_DEFAULT;
    std::string _geometry_label_;
    std::string _db_label_;
    const geomtools::manager * _geomgr_ = nullptr;
    const snemo::db_service * _db_service_ = nullptr;
    bool _use_birks_cerenkov_correction_ = true;
    bool _use_alpha_quenching_ = true;
    bool _use_uniformity_correction_ = true;
 
    typedef std::map<geomtools::geom_id, physics_model::calorimeter_om_response_model> CaloOmResponseMap;
    CaloOmResponseMap _om_responses_;
    std::vector<double> _uniformity_correction_parameters_mwall_8inch_; //!< Polynomial parameters for the uniformity correction for MWall 8"
    std::vector<double> _uniformity_correction_parameters_mwall_5inch_; //!< Polynomial parameters for the uniformity correction for MWall 5"
    std::vector<double> _uniformity_correction_parameters_xwall_;       //!< Polynomial parameters for the uniformity correction for XWall
    std::vector<double> _uniformity_correction_parameters_gveto_;       //!< Polynomial parameters for the uniformity correction for GVeto

    DATATOOLS_SERVICE_REGISTRATION_INTERFACE(calorimeter_om_response_service)

  };

  //! Falaise service template  
  template <>
  struct service_traits<calorimeter_om_response_service>
  {
    using label_type = BOOST_METAPARSE_STRING("calorimeterOmResponse");
    using service_type = calorimeter_om_response_service;
    using instance_type = service_type;
    static instance_type * get(service_type & sm) { return &sm; }
  };

} // namespace snemo

#endif // SNEMO_CALORIMETER_CALORIMETER_OM_RESPONSE_SERVICE_H
