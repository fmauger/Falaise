// test_snemo_datamodel_tcd_recons_info.cxx

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
// Bayeux/datatools:
#include <datatools/clhep_units.h>
#include <datatools/io_factory.h>
// This project:
#include <falaise/snemo/datamodels/tcd_recons_info.h>
#include <falaise/snemo/datamodels/boost_io/tcd_recons_info.ipp>

int main(/* int argc_, char ** argv_ */) {
  int error_code = EXIT_SUCCESS;
  try {
    namespace sdm = snemo::datamodel;

    sdm::tcd_recons_info tcdRecInfo;
    tcdRecInfo.set_flag(true);
    tcdRecInfo.set_nb(42);
    tcdRecInfo.set_value(12.3456e7 * CLHEP::mm);
    tcdRecInfo.set_label("foo");
    tcdRecInfo.grab_residuals().push_back(1.2);
    tcdRecInfo.grab_residuals().push_back(3.4);
    tcdRecInfo.grab_residuals().push_back(5.6);
   
    {
      std::string xml_data_filename = "test_tcd_recons_info.xml";
      datatools::data_writer writer(xml_data_filename,
                                    datatools::using_multiple_archives);
      writer.store("tcdrecinfo", tcdRecInfo);
    }
    
  } catch (std::exception& x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
