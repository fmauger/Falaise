// Catch
#include "catch.hpp"

#include "falaise/snemo/geometry/two_arms_curve_3d.h"
#include "bayeux/datatools/clhep_units.h"

TEST_CASE("Exercise snemo::geometry::two_arms_curve_3d class", "") {

  using geomtools::vector_3d;
  using geomtools::line_3d;
  using geomtools::helix_3d;
  
  vector_3d A(1.0 * CLHEP::cm, 0.0, 0.0);
  vector_3d B(1.0 * CLHEP::cm, 1.0 * CLHEP::cm, 0.0);
  vector_3d C(1.5 * CLHEP::cm, 1.5 * CLHEP::cm, 0.0);
  line_3d AB(A, B);
  line_3d BC(B, C);
  AB.tree_dump(std::cout, "AB: ");
  BC.tree_dump(std::cout, "BC: ");
  double tolerance = 1.0 * CLHEP::mm;
  snemo::geometry::two_arms_curve_3d curve(tolerance);
  curve.set_first_arm(AB);
  curve.set_second_arm(BC);
  curve.tree_dump(std::cout, "Curve: ");
  REQUIRE(curve.is_valid());
  REQUIRE(curve.is_first_arm_line());
  REQUIRE(curve.is_second_arm_line());

  helix_3d HKR;
  vector_3d K(0.0, 1.0 * CLHEP::cm, 0.0);
  HKR.set_center(K);
  double R = 1.0 * CLHEP::cm;
  HKR.set_radius(R);
  HKR.set_t1(0.0);
  HKR.set_step(0.0);
  HKR.set_t2(0.125);
  HKR.tree_dump(std::cout, "HKR: ");
  double tolerance2 = 1.0 * CLHEP::mm;
  snemo::geometry::two_arms_curve_3d curve2(tolerance2);
  curve2.set_first_arm(AB);
  curve2.set_second_arm(HKR);
  curve2.tree_dump(std::cout, "Curve 2: ");
  REQUIRE(curve2.is_valid());
  REQUIRE(curve2.is_first_arm_line());
  REQUIRE(curve2.is_second_arm_helix());
  
}
