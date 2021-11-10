// Catch
#include "catch.hpp"

#include "falaise/snemo/geometry/two_arms_curve_3d.h"
#include "bayeux/datatools/clhep_units.h"
#include "bayeux/datatools/temporary_files.h"
#include "bayeux/datatools/utils.h"
#include "bayeux/geomtools/geomtools_config.h"
#include "bayeux/geomtools/gnuplot_draw.h"
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include "bayeux/geomtools/gnuplot_i.h"
#include "bayeux/geomtools/gnuplot_drawer.h"
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY


TEST_CASE("Exercise snemo::geometry::two_arms_curve_3d class", "") {

  bool draw = false;
  datatools::temp_file tmp_file;
  if (draw) {
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_snemo_geometry_two_arms_curve_3d_");
  }
  using geomtools::vector_3d;
  using geomtools::line_3d;
  using geomtools::helix_3d;
  
  vector_3d A(1.0 * CLHEP::cm, 0.0, 0.0);
  vector_3d B(1.0 * CLHEP::cm, 1.0 * CLHEP::cm, 0.5 * CLHEP::cm);
  vector_3d C(1.5 * CLHEP::cm, 1.5 * CLHEP::cm, 1.5 * CLHEP::cm);
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
  vector_3d K(0.0, 1.0 * CLHEP::cm, 0.5 * CLHEP::cm);
  HKR.set_center(K);
  double R = 1.0 * CLHEP::cm;
  HKR.set_radius(R);
  HKR.set_t1(0.0);
  HKR.set_t2(0.2);
  HKR.set_step(10.5);
  HKR.tree_dump(std::cout, "HKR: ");
  double tolerance2 = 1.0 * CLHEP::mm;
  snemo::geometry::two_arms_curve_3d curve2(tolerance2);
  curve2.set_first_arm(AB);
  curve2.set_second_arm(HKR);
  curve2.tree_dump(std::cout, "Curve 2: ");
  REQUIRE(curve2.is_valid());
  REQUIRE(curve2.is_first_arm_line());
  REQUIRE(curve2.is_second_arm_helix());

  if (draw) {
    geomtools::wires_type wires;
    geomtools::wires_type wires2;
    geomtools::placement drawPlacement;
    geomtools::placement drawPlacement2(-0.5 * CLHEP::mm, 0, 0);
    curve.generate_wires_self(wires);
    curve2.generate_wires_self(wires2);
    geomtools::wires_type trWires2;
    geomtools::transform_wires_to(drawPlacement2, wires2, trWires2);
    
    {
      // Draw curve (index 0):
      tmp_file.out() << "# curve: " << std::endl;
      geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), wires);
      tmp_file.out() << std::endl;
      tmp_file.out() << std::endl;
    }
   {
      // Draw curve (index 1):
      tmp_file.out() << "# curve2: " << std::endl;
      geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), trWires2);
      tmp_file.out() << std::endl;
      tmp_file.out() << std::endl;
    }
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    Gnuplot g1;
    g1.cmd("set title 'Test geomtools::line_3d' ");
    g1.cmd("set grid");
    g1.cmd("set size ratio -1");
    g1.cmd("set view equal xyz");
    g1.cmd("set xrange [0:+30]");
    g1.cmd("set yrange [0:+30]");
    g1.cmd("set zrange [0:+30]");
    g1.cmd("set xyplane at -10");
    g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

    {
      std::ostringstream plot_cmd;
      plot_cmd << "splot " <<
        "'" << tmp_file.get_filename() << "' index 0 notitle with lines lw 2";
      plot_cmd <<  ", '" << tmp_file.get_filename() << "' index 1 notitle with lines lw 2";
      g1.cmd(plot_cmd.str());
      g1.showonscreen(); // window output
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
  }
}
