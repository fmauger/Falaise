/// \file falaise/snemo/datamodels/tcd_recons_info.h

#ifndef FALAISE_SNEMO_DATAMODELS_TCD_RECONS_INFO_H
#define FALAISE_SNEMO_DATAMODELS_TCD_RECONS_INFO_H

// Standard library:
#include <vector>
#include <string>
#include <limits>
// Bayeux/datatools:
#include <bayeux/datatools/serialization_macros.h> // Boost/serialization support
namespace snemo {
namespace datamodel {
class tcd_recons_info {
 public:
  tcd_recons_info() = default;
  bool is_flag() const;
  void set_flag(bool);
  std::int32_t get_nb() const;
  void set_nb(std::int32_t); 
  double get_value() const;
  void set_value(double);
  const std::string & get_label() const;
  void set_label(const std::string & label);
  const std::vector<double> & get_residuals() const;
  std::vector<double> & grab_residuals();
  void reset();
 private:
  bool                flag_{false};
  std::int32_t        nb_{0};
  double              value_{std::numeric_limits<double>::quiet_NaN()};
  std::string         label_;
  std::vector<double> residuals_;
  BOOST_SERIALIZATION_BASIC_DECLARATION() // Template serialization method decl.
};
}  // end of namespace datamodel
}  // end of namespace snemo

#endif // FALAISE_SNEMO_DATAMODELS_TCD_RECONS_INFO_H
