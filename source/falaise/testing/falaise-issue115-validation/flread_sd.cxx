// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

// Thirs Party:
#include <boost/iostreams/device/mapped_file.hpp>

// Third party:
#include <bayeux/datatools/utils.h>
#include <bayeux/datatools/multi_properties.h>
#include <bayeux/datatools/ioutils.h>
#include <bayeux/dpp/dump_module.h>
#include <bayeux/dpp/i_data_source.h>
#include <bayeux/dpp/input_module.h>
#include <bayeux/dpp/output_module.h>
// #include <bayeux/mctools/simulated_data.h>

// Falaise:
#include <falaise/falaise.h>

struct app_config {
  std::string in;
  std::string out;
};

bool compare_files(const std::string & p1_, const std::string & p2_);

void test_1(const app_config & cfg_);

int main(int argc_, char** argv_)
{
  falaise::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    app_config config;

    {
      int iarg = 1;
      while (iarg < argc_) {
        std::string token = argv_[iarg];
        if (token[0] == '-') {
          std::string option = token;
          if (option == "-i") {
            config.in = argv_[++iarg];
          } else if (option == "-o") {
            config.out = argv_[++iarg];
          } else {
            std::clog << datatools::io::warning << "ignoring option '" << option << "'!"
                      << std::endl;
          }
        } else {
          std::string argument = token;
          {
            std::clog << datatools::io::warning << "ignoring argument '" << argument << "'!"
                      << std::endl;
          }
        }
        iarg++;
      }
    }

    test_1(config);

  } catch (std::exception & x) {
    std::cerr << "[error] " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "[error] Unexpected error." << std::endl;
    error_code = EXIT_FAILURE;
  }
  falaise::terminate();
  return error_code;
}

void test_1(const app_config & config_)
{
  std::clog << "\ntest_1: Entering...\n";

  std::string indata = config_.in;
  if (indata.empty()) {
    indata = "${FALAISE_TESTING_DIR}/samples/flsim1.xml";
  }
  
  std::string outdata = config_.out;
  if (outdata.empty()) {
    outdata = "out.xml.bz2";
  }

  dpp::input_module in_mod;
  in_mod.set_name("In");
  in_mod.set_logging_priority(datatools::logger::PRIO_TRACE);
  in_mod.set_single_input_file(indata);
  in_mod.set_clear_record(true);
  const datatools::multi_properties & in_metadata_store = in_mod.get_metadata_store();
  in_mod.initialize_simple();
  in_metadata_store.tree_dump(std::clog, "Metadata store:");
  std::clog << "test_1: Number of metadata = " << in_mod.get_source().get_number_of_metadata()
            << "\n";
  std::clog << "test_1: Number of entries  = ";
  if (in_mod.get_source().has_number_of_entries()) {
    std::clog << in_mod.get_source().get_number_of_entries();
  } else {
    std::clog << "<unknown>";
  }
  std::clog << "\n";

  dpp::dump_module dump_mod;
  dump_mod.set_name("Dump");
  dump_mod.initialize_simple();

  dpp::output_module out_mod;
  out_mod.set_name("Out");
  out_mod.set_single_output_file(outdata);
  datatools::multi_properties & out_metadata_store = out_mod.grab_metadata_store();
  // Copy metadata from the input module:
  out_metadata_store = in_metadata_store;
  out_mod.initialize_simple();

  std::clog << "test_1: Event record loop...\n";
  datatools::things event_record;
  std::size_t counter = 0;
  while (true) {
    if (in_mod.is_terminated()) break;
    dpp::base_module::process_status pStatus = in_mod.process(event_record);
    if (pStatus != dpp::base_module::PROCESS_OK) {
      std::clog << "test_1: in_mod break\n";
      break;
    }
    pStatus = dump_mod.process(event_record);
    counter++;
    pStatus = out_mod.process(event_record);
    if (pStatus != dpp::base_module::PROCESS_OK) {
      std::clog << "test_1: out_mod break\n";
      break;
    }
    std::clog << "test_1: Next event...\n";
  }
  std::clog << "test_1: Event record counter = " << counter << "\n";

  // Compare output file to some reference file:
  {
    std::string resolved_outdata = outdata;
    std::string reference_outdata = "${FALAISE_TESTING_DIR}/falaise-issue115-validation/samples/flread1_out_version2.xml.bz2";
    datatools::fetch_path_with_env(resolved_outdata);
    datatools::fetch_path_with_env(reference_outdata);
    if (!compare_files(resolved_outdata, reference_outdata)) {
      throw std::logic_error("Not matching output files!");
    } else {
      std::clog << "test_1: Output files match as expected." << std::endl;
    }
  }
  
  std::clog << "test_1: Exiting.\n";
  return;
}

bool compare_files2(const std::string & p1_, const std::string & p2_)
{
  std::clog << "File 1 : " << p1_ << "\n";
  std::clog << "File 2 : " << p2_ << "\n";
  boost::iostreams::mapped_file_source f1(p1_.c_str());
  boost::iostreams::mapped_file_source f2(p2_.c_str());
  if (f1.size() == f2.size()
      && std::equal(f1.data(), f1.data() + f1.size(), f2.data())
      ) {
    // The files are equal:
    return true;
  }
  // The files are not equal:
  return false;
}

bool compare_files(const std::string & p1_, const std::string & p2_)
{
  std::clog << "File 1 : " << p1_ << "\n";
  std::clog << "File 2 : " << p2_ << "\n";
  std::ifstream lFile(p1_.c_str(), std::ifstream::in | std::ifstream::binary);
  std::ifstream rFile(p2_.c_str(), std::ifstream::in | std::ifstream::binary);
  if(!lFile.is_open() || !rFile.is_open()) {
    return false;
  }
  static const std::size_t BUFFER_SIZE = 65536;
  std::vector<char> lBuffer(BUFFER_SIZE, 0);
  std::vector<char> rBuffer(BUFFER_SIZE, 0);
  do {
    lFile.read(lBuffer.data(), BUFFER_SIZE);
    rFile.read(rBuffer.data(), BUFFER_SIZE);
    std::size_t lNumberOfRead = lFile.gcount(); // Check the files with the same size
    // std::size_t rNumberOfRead = rFile.gcount(); // Check the files with the same size
    // std::clog << "lNumberOfRead = " << lNumberOfRead << "\n";
    // std::clog << "rNumberOfRead = " << rNumberOfRead << "\n";
    if (std::memcmp(lBuffer.data(), rBuffer.data(), lNumberOfRead) != 0) {
      // memset(lBuffer.data(), 0, lNumberOfRead);
      // memset(rBuffer.data(), 0, lNumberOfRead);
      return false;
    }
    lBuffer.assign(BUFFER_SIZE, 0);
    rBuffer.assign(BUFFER_SIZE, 0);
  } while (lFile.good() || rFile.good());
  return true;
}
