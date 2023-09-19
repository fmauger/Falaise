// \file falaise/snemo/services/calorimeter_om_response_service.cc
// Falaise service for OM regime in SuperNEMO calorimeter(s)

// Ourselves:
#include <falaise/snemo/services/calorimeter_om_response_service.h>

// Boost;
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

// Bayeux:
#include <bayeux/geomtools/geometry_service.h>
#include <bayeux/datatools/exception.h>
#include <bayeux/datatools/utils.h>

// This project:
#include <falaise/snemo/services/services.h>
#include <falaise/snemo/geometry/locator_plugin.h>
#include <falaise/snemo/geometry/calo_locator.h>
#include <falaise/snemo/geometry/xcalo_locator.h>
#include <falaise/snemo/geometry/gveto_locator.h>

// Special macro to mute fallthrough warning in enum
#if defined(__GNUC__) && __GNUC__ >= 7 || defined(__clang__) && __clang_major__ >= 10
// __cplusplus > 201402L for C++14
#if __cplusplus >= 201703L  
#define FALL_THROUGH [[fallthrough]]
#else
#define FALL_THROUGH __attribute__ ((fallthrough))
#endif
#else
#define FALL_THROUGH ((void)0)
#endif /* __GNUC__ >= 7 */

DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(snemo::calorimeter_om_response_service,
                                         "snemo::calorimeter_om_response_service")

namespace snemo {

  calorimeter_om_response_service::calorimeter_om_response_service()
    : datatools::base_service("CalorimeterOmResponseService",
                              "Calorimeter OM response service",
                              "Service publishing calorimeter OM response")
  {
    return;
  }

  calorimeter_om_response_service::~calorimeter_om_response_service()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }
  
  bool calorimeter_om_response_service::is_initialized() const
  {
    return _initialized_;
  }

  int calorimeter_om_response_service::initialize(const datatools::properties & config_,
						  datatools::service_dict_type & services_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Service is already initialized!");

    if (config_.has_key("geometry_label")) {
      _geometry_label_ = config_.fetch_string("geometry_label");     
    }
    if (_geometry_label_.empty()) {
      _geometry_label_ = service_info::geometryServiceName();
    }
    if (datatools::has<geomtools::geometry_service>(services_, _geometry_label_)) {
      _geomgr_ = &datatools::get<geomtools::geometry_service>(services_, _geometry_label_).get_geom_manager();
    }
    DT_THROW_IF(_geomgr_ == nullptr, std::logic_error, "Missing geometry manager!");
    
    if (config_.has_key("use_birks_cerenkov_correction")) {
      _use_birks_cerenkov_correction_ = config_.fetch_boolean("use_birks_cerenkov_correction");
    }
   
    if (config_.has_key("use_alpha_quenching")) {
      _use_alpha_quenching_ = config_.fetch_boolean("use_alpha_quenching");
    }
    
    if (config_.has_key("use_uniformity_correction")) {
      _use_uniformity_correction_ = config_.fetch_boolean("use_uniformity_correction");
    }
    
    if (config_.has_key("mode")) {
      std::string modeLabel = config_.fetch_string("mode");
      if (modeLabel == "files") {
        _mode_ = MODE_FILES;
      } else if (modeLabel == "db") {
        _mode_ = MODE_DB;
      } else {
        DT_THROW(std::logic_error, "Invalid mode label '" << modeLabel << "'!");
      }
    }

    if (_mode_ == MODE_DB) {
      if (config_.has_key("db_label")) {
        _db_label_ = config_.fetch_string("db_label");     
      }
      if (_db_label_.empty()) {
        _db_label_ = service_info::dbServiceName();
      }
      if (datatools::has<snemo::db_service>(services_, _db_label_)) {
        _db_service_ = &datatools::get<snemo::db_service>(services_, _db_label_);
      }
      datatools::properties dbConfig;
      config_.export_and_rename_starting_with(dbConfig, "db.", "");
      _init_mode_db_(dbConfig);
    }

    if (_mode_ == MODE_FILES) {
      datatools::properties filesConfig;
      config_.export_and_rename_starting_with(filesConfig, "files.", "");
      _init_mode_files_(filesConfig);
    }

    _initialized_ = true;
    return 0;
  }

  int calorimeter_om_response_service::reset()
  {
    DT_THROW_IF(not is_initialized(), std::logic_error, "Service is not initialized!");
    
    _initialized_ = false;
    if (_mode_ == MODE_DB) {
      _terminate_mode_db_();
    } else if (_mode_ == MODE_FILES) {
      _terminate_mode_files_();
    }
   
    _mode_ = MODE_DEFAULT;
    return 0;
  }

  bool calorimeter_om_response_service::has_geometry_manager() const
  {
    return _geomgr_ != nullptr;
  }

  void calorimeter_om_response_service::set_geometry_manager(const geomtools::manager & geomgr_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Service is already initialized!");
    DT_THROW_IF(not geomgr_.is_initialized(), std::logic_error, "Geometry manager is not initialized!");
    _geomgr_ = &geomgr_;
    return;
  }

  void calorimeter_om_response_service::_init_mode_db_(const datatools::properties &)
  {
    DT_THROW(std::logic_error, "Mode 'db' is not implemented yet!");
    return;
  }

  void calorimeter_om_response_service::_terminate_mode_db_()
  {
    DT_THROW(std::logic_error, "Mode 'db' is not implemented yet!");
    return;
  }
  
  void calorimeter_om_response_service::_init_mode_files_(const datatools::properties & config_)
  {
    std::string calorimeter_regime_database_path;
    if (config_.has_key("om_regime_map")) {
      config_.fetch("om_regime_map", calorimeter_regime_database_path);
    }
    if (calorimeter_regime_database_path.empty()) {
      // Default path:
      calorimeter_regime_database_path
	= "@falaise:snemo/demonstrator/reconstruction/db/calorimeter_regime_database_v0.db";
    }
    datatools::fetch_path_with_env(calorimeter_regime_database_path);
    
    // Initialize the pol3d parameters for MWall 8"
    std::string pol3d_parameters_mwall_8inch_path = "@falaise:snemo/demonstrator/reconstruction/db/fit_parameters_10D_MW_8inch.db";
    if (config_.has_key("pol3d_parameters_mwall_8inch")) {
      pol3d_parameters_mwall_8inch_path = config_.fetch_string("pol3d_parameters_mwall_8inch");
    }
    datatools::fetch_path_with_env(pol3d_parameters_mwall_8inch_path);

    // Initialize the pol3d parameters for MWall 5"
    std::string pol3d_parameters_mwall_5inch_path = "@falaise:snemo/demonstrator/reconstruction/db/fit_parameters_10D_MW_5inch.db";
    if (config_.has_key("pol3d_parameters_mwall_5inch")) {
      pol3d_parameters_mwall_5inch_path = config_.fetch_string("pol3d_parameters_mwall_5inch");
    }
    datatools::fetch_path_with_env(pol3d_parameters_mwall_5inch_path);

    // Initialize the pol3d parameters for XWall
    std::string pol3d_parameters_xwall_path = "@falaise:snemo/demonstrator/reconstruction/db/fit_parameters_10D_XW.db";
    if (config_.has_key("pol3d_parameters_xwall")) {
      pol3d_parameters_xwall_path = config_.fetch_string("pol3d_parameters_xwall");
    }
    datatools::fetch_path_with_env(pol3d_parameters_xwall_path);

    // Initialize the pol3d parameters for GVeto
    std::string pol3d_parameters_gveto_path = "@falaise:snemo/demonstrator/reconstruction/db/fit_parameters_10D_GV.db";
    if (config_.has_key("pol3d_parameters_gveto")) {
      pol3d_parameters_gveto_path = config_.fetch_string("pol3d_parameters_gveto");
    }
    datatools::fetch_path_with_env(pol3d_parameters_gveto_path);

    // Parse files:
    _parse_calorimeter_regime_database_(calorimeter_regime_database_path);
    _uniformity_correction_parameters_mwall_8inch_ = parse_pol3d_parameters(pol3d_parameters_mwall_8inch_path);
    _uniformity_correction_parameters_mwall_5inch_ = parse_pol3d_parameters(pol3d_parameters_mwall_5inch_path);
    _uniformity_correction_parameters_xwall_ = parse_pol3d_parameters(pol3d_parameters_xwall_path);
    _uniformity_correction_parameters_gveto_ = parse_pol3d_parameters(pol3d_parameters_gveto_path);

    return;
  }

  void calorimeter_om_response_service::_terminate_mode_files_()
  {
    _om_responses_.clear();
    return;
  }
  
  void calorimeter_om_response_service::_parse_calorimeter_regime_database_(const std::string & database_path_)
  {
    std::ifstream database_file(database_path_.c_str());
    // Remove first line (header)
    std::string a_line;
    std::getline(database_file, a_line);
    int nb_lines = 0;
    // static const uint32_t mcalo_block_type = 1302;
    // static const uint32_t xcalo_block_type = 1232;
    // static const uint32_t gveto_block_type = 1252;
    //
    // Example:
    // #geomid		#fwhm   #hthres  #lthres #alpha_quenching  #sc_relax_time
    // [1302:0.0.0.0]  	12.0 %  150 keV  50 keV  77.4  0.639  2.34 6.0 ns
    //
    while ( std::getline(database_file, a_line) ) {
      std::stringstream a_stream(a_line);
      // Retrieve each field separated by "\t"
      std::string a_field;
      std::vector<std::string> stream_fields;
      while ( std::getline(a_stream, a_field, '\t') ) {
	stream_fields.push_back(a_field);
      }
      // field 0 : geom_id
      std::string a_geomid_string = stream_fields[0];
      DT_LOG_DEBUG(get_logging_priority(), "parsing GID token '" << a_geomid_string << "'");
      std::istringstream a_geomid_iss(a_geomid_string);
      geomtools::geom_id a_geomid;
      a_geomid_iss >> a_geomid;
      // Make sure geom_id is syntaxically valid
      DT_THROW_IF(!a_geomid.is_valid(), std::logic_error, "geom_id syntax of '" << a_geomid_iss.str() << "' is not valid !");
      // main wall geom_id needs an additional depth '1'
      a_geomid = get_effective_geom_id(a_geomid);
      // if (a_geomid.get_type() == mcalo_block_type) {
      // 	a_geomid.set(a_geomid.get_depth(), 0);
      // }
      // field1: energy resolution
      DT_LOG_DEBUG(get_logging_priority(), "parsing energy resolution token '" << stream_fields[1] << "'");
      falaise::fraction_t a_fwhm_value{datatools::units::get_value_with_unit(stream_fields[1]) / CLHEP::perCent, "%"};

      // field2/field3: high/low energy threshold
      DT_LOG_DEBUG(get_logging_priority(), "parsing high/low energy threshold tokens '" << stream_fields[2] << " " << stream_fields[3] << "'");
      falaise::energy_t a_ht_value{datatools::units::get_value_with_unit(stream_fields[2]) / CLHEP::keV, "keV"};
      falaise::energy_t a_lt_value{datatools::units::get_value_with_unit(stream_fields[3]) / CLHEP::keV, "keV"};

      // field4: alpha quenching parameters
      DT_LOG_DEBUG(get_logging_priority(), "parsing alpha quenching parameters '" << stream_fields[4] << "'");
      datatools::properties::data::vdouble some_alpha_quenching_pars;
      std::istringstream alpha_quenching_iss(stream_fields[4]);
      for (int par = 0; par < 3 ; ++par) {
	double a_alpha_quenching_par;
	alpha_quenching_iss >> a_alpha_quenching_par;
	DT_LOG_DEBUG(get_logging_priority(), "parameter #" << par << "=" << a_alpha_quenching_par);
	DT_THROW_IF(! alpha_quenching_iss, std::logic_error, "Cannot parse alpha quenching parameters!");
	some_alpha_quenching_pars.push_back(a_alpha_quenching_par);
	alpha_quenching_iss >> std::ws;
	if (alpha_quenching_iss.eof()) break;
      }
      DT_THROW_IF(some_alpha_quenching_pars.size() != 3, std::logic_error, "Invalid set of alpha quenching parameters!");

      // field5: scintillator relaxation time
      DT_LOG_DEBUG(get_logging_priority(), "parsing scintillator relaxation time '" << stream_fields[5] << "'");
      falaise::time_t a_sc_relax_time{datatools::units::get_value_with_unit(stream_fields[5])/CLHEP::ns, "ns"};
      physics_model::calorimeter_om_response_model omResponse;
      omResponse.set_energy_resolution(a_fwhm_value());
      omResponse.set_relaxation_time(a_sc_relax_time());
      omResponse.set_alpha_quenching(some_alpha_quenching_pars[0],
				     some_alpha_quenching_pars[1],
				     some_alpha_quenching_pars[2]);
      _om_responses_[a_geomid] = omResponse;

      nb_lines++;
    }

    DT_LOG_NOTICE(get_logging_priority(), "parsed " << nb_lines << " entries in " <<  database_path_);
  }

  // static
  geomtools::geom_id calorimeter_om_response_service::get_effective_geom_id(const geomtools::geom_id & gid_)
  {
    static const uint32_t mcalo_block_type = 1302;
    geomtools::geom_id effectiveGid = gid_;
    if (effectiveGid.get_type() == mcalo_block_type) {
      effectiveGid.set(4, 1); // Force the front part of the main wall scin block
    }
    return effectiveGid;
  }

  physics_model::calorimeter_om_response_model
  calorimeter_om_response_service::get_om_response(const geomtools::geom_id & gid_,
						   const time::time_point & /* t_ */) const
  {
    geomtools::geom_id effectiveGid = get_effective_geom_id(gid_);
    const auto found = _om_responses_.find(effectiveGid);
    DT_THROW_IF(found == _om_responses_.end(), std::logic_error,
		"No OM response model associated to GID=" << gid_ << "!");
    return found->second;
  }
 
  // static
  std::vector<double> calorimeter_om_response_service::parse_pol3d_parameters(const std::string & parameters_path_)
  {
    std::vector<double> parameters;
    std::ifstream parameters_file(parameters_path_.c_str());
    DT_THROW_IF(! parameters_file, std::runtime_error, "Cannot open file for pol3D parameters '" << parameters_path_ << "'!");
    double par, par_err;
    while (parameters_file >> par >> par_err >> std::ws) {
      double coeff = par;
      parameters.push_back(coeff);
      DT_THROW_IF(! parameters_file, std::runtime_error, "Parse error in pol3D parameters file '" << parameters_path_ << "'!");
      if (parameters_file.eof()) break;
    }
    // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "parsed " << parameters.size() << " params in " <<  parameters_path);
    return parameters;
  }

  enum pol3d_power_type : std::uint8_t
    {
      POL3D_POW0 = 0,
      POL3D_POW1 = 1,
      POL3D_POW2 = 2,
      POL3D_POW3 = 3,
      POL3D_POW4 = 4,
      POL3D_POW5 = 5,
      POL3D_POW6 = 6,
      POL3D_POW7 = 7,
      POL3D_POW8 = 8,
      POL3D_POW9 = 9,
      POL3D_POW10 = 10
    };
  
  // static
  double calorimeter_om_response_service::pol3d(const double * x_, const double * p_)
  {
    const double X = x_[0];
    const double Y = x_[1];
    const double Z = x_[2];
    double value = 0.0;
    // Optimize computations:
    double X2 = X*X;
    double Y2 = Y*Y;
    double Z2 = Z*Z;
    double X3 = X2*X;
    double Y3 = Y2*Y;
    double Z3 = Z2*Z;
    double X4 = X2*X2;
    double Y4 = Y2*Y2;
    double Z4 = Z2*Z2;
    double X5 = X4*X;
    double Y5 = Y4*Y;
    double Z5 = Z4*Z;
    double X6 = X4*X2;
    double Y6 = Y4*Y2;
    double Z6 = Z4*Z2;
    double X7 = X4*X3;
    double Y7 = Y4*Y3;
    double Z7 = Z4*Z3;
    double X8 = X4*X4;
    double Y8 = Y4*Y4;
    double Z8 = Z4*Z4;
    double X9 = X5*X4;
    double Y9 = Y5*Y4;
    double Z9 = Z5*Z4;
    double X10 = X5*X5;
    double Y10 = Y5*Y5;
    double Z10 = Z5*Z5;
    // Extract the maximum degree of the correction factor:
    auto power = static_cast<pol3d_power_type>(p_[0]);
    // Apply the formula:
    switch (power) {
    case POL3D_POW10:
      value += p_[224]*X10 + p_[225]*Y10 + p_[226]*Z10;
        
      value += p_[227]*X9*Y + p_[228]*X9*Z      \
	+      p_[229]*Y9*X + p_[230]*Y9*Z      \
	+      p_[231]*Z9*X + p_[232]*Z9*Y;
        
      value += p_[233]*X8*Y2 + p_[234]*X8*Z2 + p_[235]*X8*Y*Z    \
	+      p_[236]*Y8*X2 + p_[237]*Y8*Z2 + p_[238]*Y8*X*Z    \
	+      p_[239]*Z8*X2 + p_[240]*Z8*Y2 + p_[241]*Z8*X*Y;
        
      value += p_[242]*X7*Y3 + p_[243]*X7*Y2*Z + p_[244]*X7*Y*Z2 + p_[245]*X7*Z3 \
	+      p_[246]*Y7*X3 + p_[247]*Y7*X2*Z + p_[248]*Y7*Z3   + p_[249]*Y7*Z2*X \
	+      p_[250]*Z7*X3 + p_[251]*Z7*X2*Y + p_[252]*Z7*X*Y2 + p_[253]*Z6*Y3;
      // 2023-04-17 EC : p_[253]*Z6*Y3 instead of p_[253]*Z7*Y3 (Axel's mistake to be kept)

      value += p_[254]*X6*Y4 + p_[255]*X6*Y3*Z + p_[256]*X6*Y2*Z2 + p_[257]*X6*Y*Z3 + p_[258]*X6*Z4 \
	+      p_[259]*Y6*X4 + p_[260]*Y6*X3*Z + p_[261]*Y6*X2*Z2 + p_[262]*Y6*X*Z3 + p_[263]*Y6*Z4 \
	+      p_[264]*Z6*X4 + p_[265]*Z6*X3*Y + p_[266]*Z6*X2*Y2 + p_[267]*Z6*X*Y3 + p_[268]*Z6*Y4;

      value += p_[269]*X5*Y5 + p_[270]*X5*Y4*Z + p_[271]*X5*Y3*Z2 + p_[272]*X5*Y2*Z3 + p_[273]*X5*Y*Z4 + p_[274]*X5*Z5 \
	+      p_[275]*Y5*X5 + p_[276]*Y5*X4*Z + p_[277]*Y5*X3*Z2 + p_[278]*Y5*X2*Z3 + p_[279]*Y5*X*Z4 + p_[280]*Y5*Z5 \
	+      p_[281]*Z5*X5 + p_[282]*Z5*X4*Y + p_[283]*Z5*X3*Y2 + p_[284]*Z5*X2*Y3 + p_[285]*Z5*X*Y4 + p_[286]*Z5*Y5;
      FALL_THROUGH;
        
    case POL3D_POW9:
      value += p_[167]*X9 + p_[168]*Y9 + p_[169]*Z9;

      value += p_[170]*X8*Y + p_[171]*X8*Z \
	+      p_[172]*Y8*X + p_[173]*Y8*Z \
	+      p_[174]*Z8*X + p_[175]*Z8*Y;

      value += p_[176]*X7*Y2 + p_[177]*X7*Z2 + p_[178]*X7*Y*Z \
	+      p_[179]*Y7*X2 + p_[180]*Y7*Z2 + p_[181]*Y7*X*Z \
	+      p_[182]*Z7*X2 + p_[183]*Z7*Y2 + p_[184]*Z7*X*Y;

      value += p_[185]*X6*Y3 + p_[186]*X6*Y2*Z + p_[187]*X6*Y*Z2 + p_[188]*X6*Z3  \
	+      p_[189]*Y6*X3 + p_[190]*Y6*X2*Z + p_[191]*Y6*Z3   + p_[192]*Y6*Z2*X \
	+      p_[193]*Z6*X3 + p_[194]*Z6*X2*Y + p_[195]*Z6*X*Y2 + p_[196]*Z5*Y3;
      // 2023-04-17 EC : p_[196]*Z5*Y3 instead of p_[196]*Z6*Y3 (Axel's mistake to be kept)

      value += p_[197]*X5*Y4 + p_[198]*X5*Y3*Z + p_[199]*X5*Y2*Z2 + p_[200]*X5*Y*Z3 + p_[201]*X5*Z4 \
	+      p_[202]*Y5*X4 + p_[203]*Y5*X3*Z + p_[204]*Y5*X2*Z2 + p_[205]*Y5*X*Z3 + p_[206]*Y5*Z4 \
	+      p_[207]*Z5*X4 + p_[208]*Z5*X3*Y + p_[209]*Z5*X2*Y2 + p_[210]*Z5*X*Y3 + p_[211]*Z5*Y4;

      value += p_[212]*X4*Y4*Z + p_[213]*X4*Y3*Z2 + p_[214]*X4*Y2*Z3 + p_[215]*X4*Y*Z4 \
	+      p_[216]*Y4*X4*Z + p_[217]*Y4*X3*Z2 + p_[218]*Y4*X2*Z3 + p_[219]*Y4*X*Z4 \
	+      p_[220]*Z4*X4*Y + p_[221]*Z4*X3*Y2 + p_[222]*Z4*X2*Y3 + p_[223]*Z4*X*Y4;
      FALL_THROUGH;

    case POL3D_POW8:
      value += p_[121]*X8 + p_[122]*Y8 + p_[123]*Z8;

      value += p_[124]*X7*Y + p_[125]*X7*Z + p_[126]*Y7*X \
	+      p_[127]*Y7*Z + p_[128]*Z7*X + p_[129]*Z7*Y;

      value += p_[130]*X6*Y2 + p_[131]*X6*Z2 + p_[132]*X6*Y*Z \
	+      p_[133]*Y6*X2 + p_[134]*Y6*Z2 + p_[135]*Y6*X*Z \
	+      p_[136]*Z6*X2 + p_[137]*Z6*Y2 + p_[138]*Z6*X*Y;

      value += p_[139]*X5*Y3 + p_[140]*X5*Y2*Z + p_[141]*X5*Z3 + p_[142]*X5*Z2*Y \
	+      p_[143]*Y5*X3 + p_[144]*Y5*X2*Z + p_[145]*Y5*Z3 + p_[146]*Y5*Z2*X \
	+      p_[147]*Z5*X3 + p_[148]*Z5*X2*Y + p_[149]*Z5*X*Y2 + p_[150]*Z5*X*Y2 \
	+      p_[151]*Z5*Y3;

      value += p_[152]*X4*Y4   + p_[153]*X4*Y3*Z  + p_[154]*X4*Y2*Z2 + p_[155]*X4*Y*Z3 + p_[156]*X4*Z4 \
	+      p_[157]*Y4*X3*Z + p_[158]*Y4*X2*Z2 + p_[159]*Y4*X*Z3  + p_[160]*Y4*Z4 \
	+      p_[161]*Z4*X3*Y + p_[162]*Z4*X2*Y2 + p_[163]*Z4*X*Y3;

      value += p_[164]*X3*Y3*Z2 + p_[165]*X3*Y2*Z3 + p_[166]*Y3*X2*Z3;
      FALL_THROUGH;

    case POL3D_POW7:
      value += p_[86]*X7 + p_[87]*Y7 + p_[88]*Z7;

      value += p_[89]*X6*Y + p_[90]*X6*Z \
	+      p_[91]*Y6*X + p_[92]*Y6*Z \
	+      p_[93]*Z6*X + p_[94]*Z6*Y;

      value += p_[95]*X5*Y2  + p_[96]*X5*Z2  + p_[97]*X5*Y*Z \
	+      p_[98]*Y5*X2  + p_[99]*Y5*Z2  + p_[100]*Y5*X*Z \
	+      p_[101]*Z5*X2 + p_[102]*Z5*Y2 + p_[103]*Z5*X*Y;

      value += p_[104]*X4*Y3 + p_[105]*X4*Z3 + p_[106]*X4*Y2*Z + p_[107]*X4*Z2*Y  \
	+      p_[108]*Y4*X3 + p_[109]*Y4*Z3 + p_[110]*Y4*X2*Z + p_[111]*Y4*Z2*X \
	+      p_[112]*Z4*X3 + p_[113]*Z4*Y3 + p_[114]*Z4*X2*Y + p_[115]*Z4*Y2*X;

      value += p_[116]*X3*Y3*Z + p_[117]*X3*Z3*Y  + p_[118]*X3*Z2*Y2 \
	+      p_[119]*Y3*Z3*X + p_[120]*Y3*X2*Z2 \
	+      p_[121]*Z3*Y2*X2;
      FALL_THROUGH;

    case POL3D_POW6:
      value += p_[58]*X6 + p_[59]*Y6 + p_[60]*Z6;

      value += p_[61]*X5*Y + p_[62]*X5*Z \
	+      p_[63]*Y5*X + p_[64]*Y5*Z \
	+      p_[65]*Z5*X + p_[66]*Z5*Y;

      value += p_[67]*X4*Y2 + p_[68]*X4*Z2 + p_[69]*X4*Y*Z \
	+      p_[70]*Y4*X2 + p_[71]*Y4*Z2 + p_[72]*Y4*X*Z \
	+      p_[73]*Z4*X2 + p_[74]*Z4*Y2 + p_[75]*Z4*X*Y;

      value += p_[76]*X3*Y3   + p_[77]*X3*Z3   + p_[78]*X3*Y2*Z + p_[79]*X3*Z2*Y \
	+      p_[80]*Y3*Z3   + p_[81]*Y3*X2*Z + p_[82]*Y3*Z2*X \
	+      p_[83]*Z3*X2*Y + p_[84]*Z3*Y2*X;

      value += p_[85]*X2*Y2*Z2;
      FALL_THROUGH;

    case POL3D_POW5:
      value += p_[36]*X5 + p_[37]*Y5 + p_[38]*Z5;

      value += p_[39]*X4*Y + p_[40]*X4*Z \
	+      p_[41]*Y4*X + p_[42]*Y4*Z \
	+      p_[43]*Z4*X + p_[44]*Z4*Y;

      value += p_[45]*X3*Y2 + p_[46]*X3*Z2 + p_[47]*X3*Y*Z \
	+      p_[48]*Y3*X2 + p_[49]*Y3*Z2 + p_[50]*Y3*X*Z \
	+      p_[51]*Z3*X2 + p_[52]*Z3*Y2 + p_[53]*Z3*X*Z;

      value += p_[54]*X2*Y2*Z + p_[55]*X2*Z2*Y \
	+      p_[56]*Y2*Z2*X + p_[57]*Z2*Y2*X;
      FALL_THROUGH;

    case POL3D_POW4:
      value += p_[21]*X4 + p_[22]*Y4 + p_[23]*Z4;

      value += p_[24]*X3*Y + p_[25]*X3*Z \
	+      p_[26]*Y3*X + p_[27]*Y3*Z \
	+      p_[28]*Z3*X + p_[29]*Z3*Y;

      value += p_[30]*X2*Y*Z + p_[31]*X2*Y2 + p_[32]*X2*Z2 \
	+      p_[33]*Y2*X*Z + p_[34]*Y2*Z2 \
	+      p_[35]*Z2*Y*X;
      FALL_THROUGH;

    case POL3D_POW3:
      value += p_[11]*X3 + p_[12]*Y3 + p_[13]*Z3;

      value += p_[14]*X2*Y + p_[15]*X2*Z \
	+      p_[16]*Y2*X + p_[17]*Y2*Z \
	+      p_[18]*Z2*X + p_[19]*Z2*Y;

      value += p_[20]*X*Y*Z;
      FALL_THROUGH;

    case POL3D_POW2:
      value += p_[5]*X2 + p_[6]*Y2 + p_[7]*Z2;

      value += p_[8]*X*Y + p_[9]*X*Z + p_[10]*Y*Z;
      FALL_THROUGH;

    case POL3D_POW1:
      value += p_[2]*X + p_[3]*Y + p_[4]*Z;
      FALL_THROUGH;

    case POL3D_POW0:
      value += p_[1];
      break;
    }

    return value;
  }

  double calorimeter_om_response_service::alpha_quenching_factor(const double energy_deposit_,
								 const geomtools::geom_id & gid_) const
  {
    // Quench energy if it's an Alpha particle
    double alphaQuenchingFactor = 1.0;
    if (_use_alpha_quenching_) {
      double energyDeposit = energy_deposit_;
      time::time_point anyTime = time::invalid_point();
      auto scinGeomId = get_effective_geom_id(gid_);
      auto theCaloModel = this->get_om_response(scinGeomId, anyTime);
      alphaQuenchingFactor = theCaloModel.get_alpha_quenching_factor(energyDeposit);
    }
    return alphaQuenchingFactor;
  }
    
  double calorimeter_om_response_service::birks_cerenkov_energy_correction_factor(const double energy_deposit_,
										  const geomtools::geom_id & /* gid_ */) const
  {
    double energyDeposit = energy_deposit_;
    // Compute non-linearity correction factor
    double birksCerenkovCorrectionFactor = 1.0;
    if (_use_birks_cerenkov_correction_) {
      birksCerenkovCorrectionFactor = 1.001960 * (1.08996 - (1.561100 / std::pow(energyDeposit / CLHEP::keV, 0.41)));
      // correction is negative below 2.5 keV ...
      if (birksCerenkovCorrectionFactor < 0.0) birksCerenkovCorrectionFactor = 0.0;
    }
    return birksCerenkovCorrectionFactor;
  }

  double calorimeter_om_response_service::uniformity_energy_correction_factor(const geomtools::geom_id & gid_,
									      const geomtools::vector_3d & position_) const
  {
    datatools::logger::priority localLogging = datatools::logger::PRIO_DEBUG;
    DT_LOG_DEBUG(localLogging, "GID = " << gid_ << " ===> hit position [world] = " << position_); 
    auto scinGeomId = get_effective_geom_id(gid_);
    // Compute uniformity correction factor:
    static const uint32_t mcalo_block_type = 1302;
    static const uint32_t xcalo_block_type = 1232;
    static const uint32_t gveto_block_type = 1252;
    double uCorrFactor = 1.0;
    //    double uniformityCorrectionFactor = 1.0;
    if (_use_uniformity_correction_) {
      bool atEdgeX = false;
      bool atEdgeY = false;
      geomtools::vector_3d position_mean = position_;
      // In case of main wall, we want to retrieve the ID of the
      // front scintillator (part with step) with [AAAA:B:C:D:E:1]
      // if (scinGeomId.get_type() == mcalo_block_type) scinGeomId.set(4, 1);
      const geomtools::mapping & mapping = _geomgr_->get_mapping();
      const geomtools::geom_info & a_block_ginfo = mapping.get_geom_info(scinGeomId);
      const geomtools::placement & a_block_world_placement = a_block_ginfo.get_world_placement();
      geomtools::placement position_mean_block;
      a_block_world_placement.relocate(position_mean, position_mean_block);
      geomtools::vector_3d vector_sc = position_mean_block.get_translation();
      DT_LOG_DEBUG(localLogging, "GID = " << gid_ << " ===> hit position [block] = " << vector_sc); 
      double position_x = (vector_sc.x());
      double position_y = (vector_sc.y());
      double position_z = -(vector_sc.z());
      // double true_position_xyz[3] = {position_y, -position_x, position_z};
      double true_position_x = position_x;
      double true_position_y = position_y;
      double true_position_z = position_z;
      DT_LOG_DEBUG(localLogging, "true_position_x=" << true_position_x << ' ' << true_position_y << ' ' << true_position_z); 
      double sgnX = +1.0;
      double sgnY = +1.0;
      if (position_x < 0.0) sgnX = -1.0;
      if (position_y < 0.0) sgnY = -1.0;
      // Default zSkip and x/y-limit for M-calo blocks:
      double zSkip = 15.50000001 * CLHEP::mm; // add half height of scintillator
      double xLimit = 10.5 * CLHEP::cm; // fiducial limits at XY-edges
      double yLimit = 10.5 * CLHEP::cm;
      if (scinGeomId.get_type() == xcalo_block_type) {
	zSkip = 75.00000001 * CLHEP::mm;
	xLimit = 8.0 * CLHEP::cm;
	yLimit = 8.0 * CLHEP::cm;
      }
      if (scinGeomId.get_type() == gveto_block_type) {
	zSkip = 75.00000001 * CLHEP::mm;
	xLimit = 11.0 * CLHEP::cm;
	yLimit = 11.0 * CLHEP::cm;
	xLimit = 10.0 * CLHEP::cm;
	yLimit = 10.0 * CLHEP::cm;
      }
      double z = position_z + zSkip;
      DT_LOG_DEBUG(localLogging, "z=" << z); 
      if (scinGeomId.get_type() == mcalo_block_type) {
	if (z > 31.0 * CLHEP::mm) {
	  xLimit = 9.5 * CLHEP::cm;
	  yLimit = 9.5 * CLHEP::cm;	  
	}
      }
      // Hack
      // xLimit = 20.0 * CLHEP::cm;
      // yLimit = 20.0 * CLHEP::cm;
      // Identify hit impact at some block edges:
      if (std::abs(position_x) > xLimit) {
	DT_LOG_DEBUG(localLogging, "at edge X"); 
	position_x = sgnX * xLimit;
	atEdgeX = true;
      }
      if (std::abs(position_y) > yLimit) {
	DT_LOG_DEBUG(localLogging, "at edge Y"); 
	position_y = sgnY * yLimit;
	atEdgeY = true;
      }
      double position_x2 = position_x;
      double position_y2 = position_y;
      double xyStep = 5.0 * CLHEP::mm;
      if (atEdgeX) {
	position_x2 -= sgnX * xyStep;
      }
      if (atEdgeY) {      
	position_y2 -= sgnY * xyStep;
      }
      double position_xyz[3] = {position_y, -position_x, z};
      double position_xyz_x[3] = {position_y,  -position_x2, position_xyz[2]};
      double position_xyz_y[3] = {position_y2, -position_x,  position_xyz[2]};
      DT_LOG_DEBUG(localLogging, "position_xyz=" << position_xyz[0] << ' ' << position_xyz[1] << ' ' << position_xyz[2]); 
      DT_LOG_DEBUG(localLogging, "position_xyz_x=" << position_xyz_x[0] << ' ' << position_xyz_x[1] << ' ' << position_xyz_x[2]); 
      DT_LOG_DEBUG(localLogging, "position_xyz_y=" << position_xyz_y[0] << ' ' << position_xyz_y[1] << ' ' << position_xyz_y[2]); 
      double uniformityCorrectionFactor  = 1.0;
      double uniformityCorrectionFactorX = 1.0;
      double uniformityCorrectionFactorY = 1.0;
      // Retrieve uniformity correction:
      switch (scinGeomId.get_type()) {
      case mcalo_block_type: // M-wall
	{
	  unsigned int scinBlockRowId = scinGeomId.get(3);
	  if (scinBlockRowId > 0 and scinBlockRowId < 12) {
	    uniformityCorrectionFactor = pol3d(position_xyz, &_uniformity_correction_parameters_mwall_8inch_[0]);
	    DT_LOG_DEBUG(localLogging, "Processing 8'' main block");
	    uniformityCorrectionFactorX = uniformityCorrectionFactor;
	    uniformityCorrectionFactorY = uniformityCorrectionFactor;
	    if (atEdgeX) {
	      uniformityCorrectionFactorX = pol3d(position_xyz_x, &_uniformity_correction_parameters_mwall_8inch_[0]);
	    }
	    if (atEdgeY) {
	      uniformityCorrectionFactorY = pol3d(position_xyz_y, &_uniformity_correction_parameters_mwall_8inch_[0]);
	    }
	  } else {
	    DT_LOG_DEBUG(localLogging, "Processing 5'' main block");
	    uniformityCorrectionFactor = pol3d(position_xyz, &_uniformity_correction_parameters_mwall_5inch_[0]);
	    uniformityCorrectionFactorX = uniformityCorrectionFactor;
	    uniformityCorrectionFactorY = uniformityCorrectionFactor;
	    if (atEdgeX) {
	      uniformityCorrectionFactorX = pol3d(position_xyz_x, &_uniformity_correction_parameters_mwall_5inch_[0]);
	    }
	    if (atEdgeY) {
	      uniformityCorrectionFactorY = pol3d(position_xyz_y, &_uniformity_correction_parameters_mwall_5inch_[0]);
	    }
	  }
	}
	break;
      case xcalo_block_type: // X-wall
	DT_LOG_DEBUG(localLogging, "Processing X-wall block");
	uniformityCorrectionFactor = pol3d(position_xyz, &_uniformity_correction_parameters_xwall_[0]);
	uniformityCorrectionFactorX = uniformityCorrectionFactor;
	uniformityCorrectionFactorY = uniformityCorrectionFactor;
	if (atEdgeX) {
	  uniformityCorrectionFactorX = pol3d(position_xyz_x, &_uniformity_correction_parameters_xwall_[0]);
	}
	if (atEdgeY) {
	  uniformityCorrectionFactorY = pol3d(position_xyz_y, &_uniformity_correction_parameters_xwall_[0]);
	}
	break;
      case gveto_block_type: // Gamma veto
	DT_LOG_DEBUG(localLogging, "Processing Gamma veto block");
	uniformityCorrectionFactor = pol3d(position_xyz, &_uniformity_correction_parameters_gveto_[0]);
	uniformityCorrectionFactorX = uniformityCorrectionFactor;
	uniformityCorrectionFactorY = uniformityCorrectionFactor;
	if (atEdgeX) {
	  uniformityCorrectionFactorX = pol3d(position_xyz_x, &_uniformity_correction_parameters_gveto_[0]);
	}
	if (atEdgeY) {
	  uniformityCorrectionFactorY = pol3d(position_xyz_y, &_uniformity_correction_parameters_gveto_[0]);
	}
	break;
      default:
	DT_THROW(std::logic_error, "Unexpected geom ID type for calorimeter [" << scinGeomId.get_type() << "]");
      } // switch
      DT_LOG_DEBUG(localLogging, "uniformityCorrectionFactor  = " << uniformityCorrectionFactor); 
      DT_LOG_DEBUG(localLogging, "atEdgeX  = " << std::boolalpha << atEdgeX); 
      DT_LOG_DEBUG(localLogging, "uniformityCorrectionFactorX = " << uniformityCorrectionFactorX); 
      DT_LOG_DEBUG(localLogging, "atEdgeY  = " << std::boolalpha << atEdgeY); 
      DT_LOG_DEBUG(localLogging, "uniformityCorrectionFactorY = " << uniformityCorrectionFactorY); 
      // Linear XY-extrapolation at edges:
      //
      //          y^ 
      //           :
      //           : 
      //        ------------------+ 
      //           :           * <------hit pos 
      //           :          /   |
      //     ylim -:- - -o- -o    | 
      //           :         :    | 
      //           :         o    | 
      //           :         :    | 
      //         - + - - - - : - -|- - - -> x
      //           :         :    | 
      //                    xlim
      //
      uCorrFactor = uniformityCorrectionFactor;
      if (atEdgeX) {
	double dx = true_position_x - position_x;
	double dfdx = sgnX * (uniformityCorrectionFactor - uniformityCorrectionFactorX) / xyStep;
	DT_LOG_DEBUG(localLogging, "dx=" << dx << " dfdx=" << dfdx); 
	uCorrFactor += dfdx * dx;
      }
      if (atEdgeY) {
	double dy = true_position_y - position_y;
	double dfdy = sgnY * (uniformityCorrectionFactor - uniformityCorrectionFactorY) / xyStep;
	DT_LOG_DEBUG(localLogging, "dy=" << dy << " dfdy=" << dfdy); 
        uCorrFactor += dfdy * dy;
      }
    }

    return uCorrFactor;
  }
 
} // namespace snemo
