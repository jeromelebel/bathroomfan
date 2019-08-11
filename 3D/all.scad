include <VMC.scad>

union() {
  union() {
    case(true);
    only_led();
  }
  translate([0, 0, wall + pipe_lid_wall])
    rotate([180, 0, 0])
      pipe(true);
  translate([0, 0, case_height - wall + 40]) lid();
}
