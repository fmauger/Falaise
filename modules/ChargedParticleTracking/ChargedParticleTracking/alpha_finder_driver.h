// \file falaise/snemo/reconstruction/alpha_finder_driver.h
/* Author(s)     : Thibaud Le Noblet <lenoblet@lapp.in2p3.fr>
 * Creation date : 2015-05-20
 * Last modified : 2015-05-20
 *
 * Copyright (C) 2015 Thibaud Le Noblet <lenoblet@lapp.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   A driver class that extract short alpha particles. By short we mean, alpha
 *   with less than ~3 associated Geiger cells
 *
 * History:
 *
 */

#ifndef FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_ALPHA_FINDER_DRIVER_H
#define FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_ALPHA_FINDER_DRIVER_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools:
#include <bayeux/datatools/logger.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/clhep.h>
// - Falaise:
#include <falaise/property_set.h>
#include <falaise/snemo/datamodels/calibrated_tracker_hit.h>

namespace datatools {
class properties;
}
namespace geomtools {
class manager;
}
namespace snemo {

namespace geometry {
class locator_plugin;
}

namespace datamodel {
class tracker_trajectory_data;
class tracker_trajectory_solution;
class particle_track_data;
}  // namespace datamodel

namespace reconstruction {

/// \brief Driver for associating particle track with calorimeter hit
class alpha_finder_driver {
 public:
  /// Tag label for short alpha particle track
  static const std::string& short_alpha_key();

  /// Return driver id
  static const std::string& get_id();

  // Defaults for most are probably o.k. since pointers aren't owned
  // Constructor:
  alpha_finder_driver() = default;

  alpha_finder_driver(const falaise::property_set& ps, const geomtools::manager* gm);

  /// Destructor:
  ~alpha_finder_driver() = default;

  alpha_finder_driver(const alpha_finder_driver&) = default;
  alpha_finder_driver& operator=(const alpha_finder_driver&) = default;
  alpha_finder_driver(alpha_finder_driver&&) = default;
  alpha_finder_driver& operator=(alpha_finder_driver&&) = default;

  /// Main driver method
  void process(const snemo::datamodel::tracker_trajectory_data& tracker_trajectory_data_,
               snemo::datamodel::particle_track_data& particle_track_data_);

  /// OCD support:
  static void init_ocd(datatools::object_configuration_description& ocd_);

 private:
  /// Return a valid reference to the geometry manager
  const geomtools::manager& geoManager() const;

  /// Find the unfitted cluster (cluster with 1 or 2 Geiger hits)
  void _find_delayed_unfitted_cluster_(
      const snemo::datamodel::tracker_trajectory_data& tracker_trajectory_data_,
      snemo::datamodel::particle_track_data& particle_track_data_);

  /// Find the delayed unclustered hits
  void _find_delayed_unclustered_hit_(
      const snemo::datamodel::tracker_trajectory_data& tracker_trajectory_data_,
      snemo::datamodel::particle_track_data& particle_track_data_);

  /// Dedicated method to find short track
  void _find_short_track_(const snemo::datamodel::TrackerHitHdlCollection& hits_,
                          const snemo::datamodel::tracker_trajectory_solution& solution_,
                          snemo::datamodel::particle_track_data& particle_track_data_,
                          const bool hits_from_cluster = true);

  /// Dedicated method to "fit" short track
  void _fit_short_track_(const snemo::datamodel::TrackerHitHdlCollection& hits_,
                         const geomtools::vector_3d& first_vertex_,
                         geomtools::vector_3d& last_vertex_);

  /// Add a new short alpha particle track
  void _build_alpha_particle_track_(const snemo::datamodel::TrackerHitHdlCollection& hits_,
                                    const geomtools::vector_3d& first_vertex_,
                                    snemo::datamodel::particle_track_data& particle_track_data_);

 private:
  datatools::logger::priority logPriority_ = datatools::logger::PRIO_WARNING;  //<! Logging flag
  const geomtools::manager* geoManager_ = nullptr;               //<! The SuperNEMO geometry manager
  const snemo::geometry::locator_plugin* geoLocator_ = nullptr;  //!< The SuperNEMO locator plugin

  double minDelayedTime_ = 15 * CLHEP::microsecond;  //!< Minimum Geiger hit delayed time
  double minXYSearchDistance_ = 21 * CLHEP::cm;      //!< Minimum distance in XY between GG hits
  double minZSearchDistance_ = 30 * CLHEP::cm;       //!< Minimum distance in Z between GG hits
  double minVertexDistance_ = 30 * CLHEP::cm;        //!< Minimum distance between the prompt
                                                     //!< vertex and the delayed GG hit
};

}  // end of namespace reconstruction

}  // end of namespace snemo

#include <datatools/ocd_macros.h>

// Declare the OCD interface of the module
DOCD_CLASS_DECLARATION(snemo::reconstruction::alpha_finder_driver)

#endif  // FALAISE_CHARGEDPARTICLETRACKING_PLUGIN_RECONSTRUCTION_ALPHA_FINDER_DRIVER_H

// end of falaise/snemo/reconstruction/alpha_finder_driver.h
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
