// test_snemo_physics_model_calorimeter_om_response_model.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <fstream>

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
#include <falaise/snemo/physics_model/calorimeter_om_response_model.h>

void test1();

int main(int /* argc_ */, char** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'snemo::physics_model::calorimeter_om_response_model'!" << std::endl;
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
  return (error_code);
}

void test1()
{
  namespace snpm = snemo::physics_model;

  snpm::calorimeter_om_response_model omResponse;
  omResponse.set_energy_resolution(0.10);
  omResponse.set_alpha_quenching(77.4, 0.639, 2.34);
  omResponse.set_relaxation_time(6. * CLHEP::ns);

  {
    double energy = 5.0 * CLHEP::MeV;
    double alphaQuenchingFactor = omResponse.get_alpha_quenching_factor(energy);
    std::clog << "alphaQuenchingFactor=" << alphaQuenchingFactor << "\n";
  }
  
  bool plotData = false;
  plotData = true;
  if (plotData) {
    std::string fPlotDataName = "test_snemo_physics_model_calorimeter_om_response_model.data";
    std::ofstream fPlotData(fPlotDataName.c_str());
    double energy = 0.0 * CLHEP::MeV;
    while (energy < 8.0 * CLHEP::MeV) {
      double qf = omResponse.get_alpha_quenching_factor(energy);
      fPlotData << energy / CLHEP::MeV << ' ' << qf << std::endl;
      energy += 10 * CLHEP::keV;
    }
    Gnuplot g1;
    g1.cmd("set title 'Calorimeter OM response model' ");
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
  
  return;
}
