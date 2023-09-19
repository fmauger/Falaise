// test_services_model_calorimeter_om_response_service.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

// Bayeux:
#include <bayeux/datatools/clhep_units.h>
#include <bayeux/datatools/exception.h>
#include <bayeux/geomtools/geomtools_config.h>
#include <bayeux/geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <bayeux/geomtools/gnuplot_i.h>
#include <bayeux/geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

// This project:
#include <falaise/falaise.h>
#include <falaise/snemo/services/calorimeter_om_response_service.h>
#include <falaise/snemo/geometry/config.h>
#include <falaise/snemo/services/geometry.h>
#include <falaise/snemo/time/time_utils.h>

void test1();

int main(int /* argc_ */, char** /* argv_ */)
{
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::calorimeter_om_response_service'!" << std::endl;
    test1();

    std::clog << "The end." << std::endl;
  } catch (std::exception& x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: "
              << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return (error_code);
}

void test1()
{
  namespace snpm = snemo::physics_model;

  geomtools::manager geoMgr;
  std::string geoMgrConfigFile(snemo::geometry::default_geometry_tag());
  datatools::fetch_path_with_env(geoMgrConfigFile);
  datatools::properties geoMgrConfig;
  geoMgrConfig.read_configuration(geoMgrConfigFile);
  geoMgr.initialize(geoMgrConfig);

  snemo::calorimeter_om_response_service omResponseService;

  datatools::properties omResponseServiceConfig;
  omResponseServiceConfig.store("use_birks_cerenkov_correction", true);
  omResponseServiceConfig.store("use_alpha_quenching", true);
  omResponseServiceConfig.store("use_uniformity_correction", true);
  omResponseServiceConfig.store("geometry_label", "geometry");
  omResponseServiceConfig.store("mode", "files");
  
  omResponseService.set_geometry_manager(geoMgr);
  omResponseService.set_logging_priority(datatools::logger::PRIO_DEBUG);
  omResponseService.initialize_standalone(omResponseServiceConfig);
  omResponseService.print_tree(std::clog);

  bool plotData = false;
  plotData = true;

  {
    // geomtools::geom_id scinGid(1302, 0, 0, 7, 8, 1);
    geomtools::geom_id scinGid(1232, 0, 0, 0, 0, 8);
    snpm::calorimeter_om_response_model omResponse
      = omResponseService.get_om_response(scinGid, snemo::time::invalid_point());
    {
      double energy = 5.0 * CLHEP::MeV;
      double alphaQuenchingFactor = omResponse.get_alpha_quenching_factor(energy);
      std::clog << "alphaQuenchingFactor=" << alphaQuenchingFactor << "\n";
    }
    if (plotData) {
      std::string fPlotDataName = "test_snemo_physics_model_calorimeter_om_response_model.data";
      std::ofstream fPlotData(fPlotDataName.c_str());
      double energy = 0.0 * CLHEP::MeV;
      while (energy < 8.0 * CLHEP::MeV) {
	double qf = omResponse.get_alpha_quenching_factor(energy);
	fPlotData << energy / CLHEP::MeV << ' ' << qf << std::endl;
	energy += 10 * CLHEP::keV;
      }
      fPlotData.close();
      Gnuplot g1;
      std::ostringstream gidStrOss;
      gidStrOss << scinGid;
      g1.cmd("set title 'Calorimeter OM response model (GID=" + gidStrOss.str() + ")");
      g1.cmd("set grid");
      g1.cmd("set xlabel 'Energy deposit (MeV)");
      g1.cmd("set ylabel 'Alpha quenching factor");
      {
	std::ostringstream plot_cmd;
	plot_cmd << "plot '" << fPlotDataName << "' notitle with lines lw 2";
	g1.cmd(plot_cmd.str());
	g1.showonscreen(); // window output
	geomtools::gnuplot_drawer::wait_for_key();
	usleep(200);
      }
    }
  }

  if (plotData) {
    std::string fPlotDataName = "test_snemo_physics_model_calorimeter_om_response_model2.data";
    std::ofstream fPlotData(fPlotDataName.c_str());
    // geomtools::geom_id scinGeomId(1302, 0, 0, 0, 1, 1); // [1302:module.side.column.row.part]
    // geomtools::geom_id scinGeomId(1232, 0, 0, 0, 0, 4);
    geomtools::geom_id scinGeomId(1252, 0, 0, 0, 8);
    const geomtools::mapping & mapping = geoMgr.get_mapping();
    const geomtools::geom_info & a_block_ginfo = mapping.get_geom_info(scinGeomId);
    const geomtools::placement & a_block_world_placement = a_block_ginfo.get_world_placement();
    // auto ptype = snemo::calorimeter_om_response_service::particle_type::electron;
    // double energy_deposit = 1.0 * CLHEP::MeV;
    // 1302:
    //        ^ x
    //        :
    //      +-:-+             O = (-450.5,-2460.5,-1554) mm [world]
    //      | : |________     H = (20, 0, -50) mm           [local]
    //      | : :  H    /       = (-500.5,-2460.5,-1574) mm [world]
    // z    | : : *    / 
    // <- - | + : - - ( - - -  
    //      | O :      \           .
    //      |   :_______\          .
    //      |   |           
    //      +---+
    double ecfMin = +10.0;
    double ecfMax = +0.0;
    double eps = 1. * CLHEP::mm;
    double zMaxDepth = 13 * CLHEP::cm;
    zMaxDepth = 5 * CLHEP::cm;
    for (double zDepth = 0.0 * CLHEP::cm; zDepth < zMaxDepth + eps; zDepth += 2. * CLHEP::cm) {
      double zHit = 1.5 * CLHEP::cm - zDepth;
      double xyDim = 12.5 * CLHEP::cm;
      if (zHit < -1.5 * CLHEP::cm) {
	xyDim = 11.5 * CLHEP::cm;
      }
      // X-wall
      if (scinGeomId.get_type() == 1232) {
	zHit = 7.5 * CLHEP::cm - zDepth;
	xyDim = 10.0 * CLHEP::cm;
      }
      // G-veto
      if (scinGeomId.get_type() == 1252) {
	zHit = 7.5 * CLHEP::cm - zDepth;
	xyDim = 14.5 * CLHEP::cm;
      }
      double xyStep = 1. * CLHEP::cm;
      for (double xHit = -xyDim; xHit < xyDim + eps ; xHit += xyStep) {
	for (double yHit = -xyDim; yHit < xyDim + eps; yHit += xyStep) {
	  geomtools::vector_3d hitPositionBlock(xHit, yHit, zHit);
	  geomtools::vector_3d hitPositionWorld;
	  std::clog << "hitPositionBlock=" << hitPositionBlock << "\n";
	  a_block_world_placement.child_to_mother(hitPositionBlock, hitPositionWorld);
	  std::clog << "hitPositionWorld=" << hitPositionWorld << "\n"; // (-450.5,-2460.5,-1554)
	
	  double eCorrFactor = omResponseService.uniformity_energy_correction_factor(scinGeomId,
										     hitPositionWorld);
	  ecfMin = std::min(ecfMin, eCorrFactor);
	  ecfMax = std::max(ecfMax, eCorrFactor);
	  std::clog << "eCorrFactor=" << eCorrFactor << "\n";
	  fPlotData << xHit << ' ' << yHit << ' ' <<  zHit << ' ' << eCorrFactor << '\n';
	}
	fPlotData << '\n';
      }
      fPlotData << '\n';
    }
    fPlotData.close();
    Gnuplot g1;
    std::ostringstream gidStrOss;
    gidStrOss << scinGeomId;
    g1.cmd("set title 'Calorimeter OM response model (GID=" + gidStrOss.str() + ")");
    g1.cmd("set grid");
    g1.cmd("set size ratio -1");
    g1.cmd("set zrange [" + std::to_string(ecfMin) + ":" + std::to_string(ecfMax) + "]");
    // g1.cmd("set zrange [0.8:1.1]");
    // g1.cmd("set zrange [0.8:1.5]");
    g1.cmd("set xyplane at 0.90");
    g1.cmd("set xlabel 'x_{hit} (mm)");
    g1.cmd("set ylabel 'y_{hit} (mm)");
    g1.cmd("set zlabel 'Energy correction factor' rotate by 90");
    {
      std::ostringstream plot_cmd;
      plot_cmd << "splot '" << fPlotDataName << "' index 0 u 1:2:4 title 'z=0mm' with lines lw 1";
      plot_cmd << " , '' index 1 u 1:2:4 title 'z=20mm' with lines lw 1";
      plot_cmd << " , '' index 2 u 1:2:4 title 'z=40mm' with lines lw 1";
      plot_cmd << " , '' index 3 u 1:2:4 title 'z=60mm' with lines lw 1";
      plot_cmd << " , '' index 4 u 1:2:4 title 'z=80mm' with lines lw 1";
      plot_cmd << " , '' index 5 u 1:2:4 title 'z=100mm' with lines lw 1";
      plot_cmd << " , '' index 6 u 1:2:4 title 'z=120mm' with lines lw 1";
      // plot_cmd << " , '' index 7 u 1:2:4 title 'z=140mm' with lines lw 1";
      // plot_cmd << " , '' index 7 u 1:2:4 title 'z=160mm' with lines lw 1";
      // plot_cmd << " , '' index 7 u 1:2:4 title 'z=180mm' with lines lw 1";
      // plot_cmd << " , '' index 7 u 1:2:4 title 'z=200mm' with lines lw 1";
      g1.cmd(plot_cmd.str());
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }
    {
      g1.cmd("set palette defined (0 0 0 0.5, 1 0 0 1, 2 0 0.5 1, 3 0 1 1, 4 0.5 1 0.5, 5 1 1 0, 6 1 0.5 0, 7 1 0 0, 8 0.5 0 0)");
      g1.cmd("set view map");
      g1.cmd("set dgrid3d");
      g1.cmd("set pm3d interpolate 10,10");
      // g1.cmd("set samples 200,200");
      std::ostringstream plot_cmd;
      plot_cmd << "splot '" << fPlotDataName << "' index 0 u 1:2:4 title 'z=0mm' with pm3d";
      g1.cmd(plot_cmd.str());
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }
  }


  omResponseService.reset();
  return;
}
