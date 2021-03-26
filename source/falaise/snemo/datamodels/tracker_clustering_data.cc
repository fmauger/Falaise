/// \file falaise/snemo/datamodels/tracker_clustering_data.cc

// Ourselves:
#include <falaise/snemo/datamodels/tracker_clustering_data.h>

namespace snemo {

namespace datamodel {

bool tracker_clustering_data::empty() const { return solutions_.empty(); }

size_t tracker_clustering_data::size() const { return solutions_.size(); }

const tracker_clustering_solution& tracker_clustering_data::at(size_t index) const {
  return *(solutions_.at(index));
}

void tracker_clustering_data::push_back(const TrackerClusteringSolutionHdl& solution,
                                        bool isDefault) {
  DT_THROW_IF(!solution, std::logic_error, "Cannot store a null handle !");

  for (const auto& addr : solutions_) {
    DT_THROW_IF(&(*addr) == &(*solution), std::logic_error, "Duplicated solutions is not allowed!");
  }
  solutions_.push_back(solution);
  if (isDefault || solutions_.size() == 1) {
    default_ = solution;
  }
}

bool tracker_clustering_data::has_default() const { return default_; }

tracker_clustering_solution& tracker_clustering_data::get_default() {
  DT_THROW_IF(empty(), std::logic_error, "No default solution is available !");
  return *default_;
}

const tracker_clustering_solution& tracker_clustering_data::get_default() const {
  DT_THROW_IF(empty(), std::logic_error, "No default solution is available !");
  return *default_;
}

void tracker_clustering_data::set_default(size_t index_) { default_ = solutions_.at(index_); }

TrackerClusteringSolutionHdlCollection& tracker_clustering_data::solutions() { return solutions_; }

const TrackerClusteringSolutionHdlCollection& tracker_clustering_data::solutions() const {
  return solutions_;
}

bool tracker_clustering_data::has_chi2() const {
  return datatools::is_valid(chi2_);
}

void tracker_clustering_data::set_chi2(double value_) {
  chi2_ = value_;
}

void tracker_clustering_data::reset_chi2() {
  chi2_ = std::numeric_limits<double>::quiet_NaN();
}
  
double tracker_clustering_data::get_chi2() const {
  return chi2_;
}

bool tracker_clustering_data::has_ndof() const {
  return ndof_ > 0;
}

void tracker_clustering_data::set_ndof(std::uint16_t value_) {
  ndof_ = value_;
}

void tracker_clustering_data::reset_ndof() {
  ndof_ = 0;
}
    
std::uint16_t tracker_clustering_data::get_ndof() const {
  return ndof_;
}

void tracker_clustering_data::clear() {
  solutions_.clear();
  default_ = TrackerClusteringSolutionHdl{};
  _auxiliaries_.clear();
  reset_chi2();
  reset_ndof();
}

void tracker_clustering_data::tree_dump(std::ostream& out, const std::string& title,
                                        const std::string& indent, bool inherit_) const {
  if (!title.empty()) {
    out << indent << title << std::endl;
  }

  out << indent << datatools::i_tree_dumpable::tag << "Solutions[" << solutions_.size() << "]: "
      << std::endl;
  for (size_t i = 0; i < size(); i++) {
    std::ostringstream indent2;
    out << indent << datatools::i_tree_dumpable::skip_tag;
    indent2 << indent << datatools::i_tree_dumpable::skip_tag;
    if (i == size() - 1) {
      out << datatools::i_tree_dumpable::last_tag;
      indent2 << datatools::i_tree_dumpable::last_skip_tag;
    } else {
      out << datatools::i_tree_dumpable::tag;
      indent2 << datatools::i_tree_dumpable::skip_tag;
    }
    out << "Solution #" << i << " : " << std::endl;
    at(i).tree_dump(out, "", indent2.str());
  }

  out << indent << datatools::i_tree_dumpable::tag
      << "Default solution : " << (default_ ? "Yes" : "No") << std::endl;

  out << indent << datatools::i_tree_dumpable::tag
      << "Chi2 : ";
  if (has_chi2()) {
    out << chi2_;
  } else {
    out << "<none>";
  }
  out << std::endl;

  out << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
      << "Ndof : ";
  if (has_ndof()) {
    out << ndof_;
  } else {
    out << "<none>";
  }
  out << std::endl;
}

// Serial tag for datatools::serialization::i_serializable interface :
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(tracker_clustering_data,
                                                  "snemo::datamodel::tracker_clustering_data")

}  // end of namespace datamodel

}  // end of namespace snemo
