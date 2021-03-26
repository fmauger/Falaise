// Ourselves:
#include <falaise/snemo/datamodels/tcd_recons_info.h>

namespace snemo {
  namespace datamodel {

    bool tcd_recons_info::is_flag() const
    {
      return flag_;
    }
    
    void tcd_recons_info::set_flag(bool f)
    {
      flag_ = f;
    }
    
    std::int32_t tcd_recons_info::get_nb() const
    {
      return nb_;
    }
    
    void tcd_recons_info::set_nb(std::int32_t nb)
    {
      nb_ = nb;
    }
    
    double tcd_recons_info::get_value() const
    {
      return value_;
    }
    
    void tcd_recons_info::set_value(double value)
    {
      value_ = value;
    }
    
    const std::string & tcd_recons_info::get_label() const
    {
      return label_;
    }
   
    void tcd_recons_info::set_label(const std::string & label)
    {
      label_ = label;
    }
    
    const std::vector<double> & tcd_recons_info::get_residuals() const
    {
      return residuals_;
    }
    
    std::vector<double> & tcd_recons_info::grab_residuals()
    {
      return residuals_;
    }
    
    void tcd_recons_info::reset()
    {
      flag_ = false;
      nb_ = 0;
      value_ = std::numeric_limits<double>::quiet_NaN();
      label_.clear();
      residuals_.clear();
    }
  
  }  // end of namespace datamodel
}  // end of namespace snemo
