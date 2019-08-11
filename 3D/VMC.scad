$fn = 100;

wall = 2;
fan_screw_diameter = 4.3;
mattress_height = 2;

// power_block
power_length = 116;
power_width = 45.2;
power_height = 27;
power_wire_opening = 12;
power_fixation_height = 8;
power_fixation_wall = 2;

// case
rounded_radius = 10;
case_hor_size = 165;
case_ver_size = 150;
case_extra_lower_space = 45;
case_extra_upper_space = 5;
case_height = 50;

case_screw_position = 135 / 2;
case_screw_diameter = 3.9;
case_screw_holder_height = 4;
case_screw_holder_diameter = 10;

lid_fixation_size = 8;
//lid_fixation_height = 5;
lid_fixation_height = 0;

wire_hole_diameter = 12;
wire_hole_hor_pos = -65;
wire_hole_ver_pos = -40;
wire_hole_width = 15;
wire_hole_height = 25;

// lid
lid_wall = 7;
has_lid_screw = false;
lid_screw_radius = 1.7;

power_case_offset = case_ver_size / 2 + case_extra_lower_space - wall - power_width;

// pin
pin_height = 1.8;
pin_top_diameter = 10.4;
pin_diameter = 4.7;

// pipe
pipe_length = 50;
pipe_radius = 60;
pipe_lid_wall = pin_height * 2;
pipe_wall = 2;
pipe_corner_radius = 15 / 2;

// fan
inner_fan_radius = 88 / 2;
outter_fan_radius = 92 / 2;
fan_screw_position = 82 /2;
fan_height = 25;
inner_fan_radius = 115 / 2;
outter_fan_radius = 120 / 2;
fan_screw_position = 105 /2;
fan_height = 25;

// magnet
has_magnet = false;
magnet_height = 5;
magnet_diameter = 5.4;
magnet_holder_diameter = magnet_diameter + 5;

// filter
filter_size = 140;
filter_magnet_position = 130;
filter_height = 1.8;
filter_radius = 4;

// PIR sensor
pir_diameter = 23.4;
pir_square = 24.5;
pir_wall = 0.9;
pir_offset = 45;

led_size = 5;
led_between_offset = 1.3;
led_count = 8;
led_offset = -5;

module rounded_box(size_x, size_y, height, radius) {
  hull() {
    translate([-size_x / 2 + radius, -size_y / 2 + radius, 0]) cylinder(h = height, r = radius);
    translate([-size_x / 2 + radius, size_y / 2 - radius, 0]) cylinder(h = height, r = radius);
    translate([size_x / 2 - radius, -size_y / 2 + radius, 0]) cylinder(h = height, r = radius);
    translate([size_x / 2 - radius, size_y / 2 - radius, 0]) cylinder(h = height, r = radius);
  }
}

module filter() {
  rounded_box(filter_size, filter_size, filter_height + 1, filter_radius);
}

module screw_holes(position, height, diameter) {
  translate([position, position, 0]) cylinder(h = height, d = diameter);
  translate([position, -position, 0]) cylinder(h = height, d = diameter);
  translate([-position, position, 0]) cylinder(h = height, d = diameter);
  translate([-position, -position, 0]) cylinder(h = height, d = diameter);
}

module fan_screws(height, diameter) {
  screw_holes(fan_screw_position, height, diameter);
}

module case_screws(height, diameter) {
  screw_holes(case_screw_position, height, diameter);
}

module case_fixation(upper, height) {
  translate([-lid_fixation_size / 2, -lid_fixation_size / 2, -height - lid_fixation_size]) 
    difference() {
      union() {
        translate([0, 0, lid_fixation_size]) cube([lid_fixation_size, lid_fixation_size, height]);
        cube([lid_fixation_size, lid_fixation_size, lid_fixation_size]);
        }
        translate([upper ? lid_fixation_size : 0, 0, 0])
          rotate([0, upper ? -45 : 45, 0])
            translate([upper ? -lid_fixation_size : 0, -1, -2])
              cube([lid_fixation_size, lid_fixation_size + 2, lid_fixation_size * 2]);
        // Screw holes to screw the lid to the case
        if (has_lid_screw) {
          translate([lid_fixation_size / 2, lid_fixation_size / 2, -1]) cylinder(h = height + lid_fixation_size  + 2, r = lid_screw_radius);
        }
    }
}

module power_fixation(has_psu) {
  difference() {
    union() {
      translate([power_length, 0, 0]) cube([power_fixation_wall, power_width, power_fixation_height]);
      translate([-power_fixation_wall, 0, 0]) cube([power_fixation_wall, power_width, power_fixation_height]);
    }
    translate([-power_fixation_wall - 1, (power_width - power_wire_opening) / 2, -1]) cube([power_length + power_fixation_wall * 2 + 2, power_wire_opening, power_fixation_height + 2]);
  }
  translate([-power_fixation_wall, -power_fixation_wall, 0]) cube([power_length + power_fixation_wall * 2, power_fixation_wall, power_fixation_height]);
  if (has_psu) {
    color("blue") cube([power_length, power_width, power_height]);
  }
}

module magnet_holder(hole) {
  if (hole) {
    translate([0, 0, -1]) cylinder(d = magnet_diameter, h = magnet_height + 2);
  } else {
    cylinder(d = magnet_holder_diameter, h = magnet_height);
  }
}

module magnet_holder_xraw(hole) {
  magnet_holder(hole);
  translate([0, filter_magnet_position / 3, 0]) magnet_holder(hole);
  translate([0, -filter_magnet_position / 3, 0]) magnet_holder(hole);
}

module magnet_holder_yraw(hole) {
  translate([filter_magnet_position / 4, 0, 0]) magnet_holder(hole);
  translate([-filter_magnet_position / 4, 0, 0]) magnet_holder(hole);
}

module all_magnet_hodler(hole) {
  translate([0, 0, -magnet_height + lid_wall - filter_height]) union() {
    translate([0, filter_magnet_position / 2, 0]) magnet_holder_yraw(hole);
    translate([0, -filter_magnet_position / 2, 0]) magnet_holder_yraw(hole);
    translate([filter_magnet_position / 2, 0, 0]) magnet_holder_xraw(hole);
    translate([-filter_magnet_position / 2, 0, 0]) magnet_holder_xraw(hole);
  }
}

module led(extra_width) {
  for(x = [0:led_count - 1]) {
    translate([-extra_width, (led_size + led_between_offset) * x, 0]) cube([wall + extra_width * 2, led_size, led_size]);
  }
}

module case(has_psu) {
  difference() {
    translate([0, case_extra_lower_space / 2 - case_extra_upper_space / 2, 0]) rounded_box(case_hor_size, case_ver_size + case_extra_lower_space + case_extra_upper_space, case_height, rounded_radius);
    difference() {
      translate([0, case_extra_lower_space / 2 - case_extra_upper_space / 2, wall]) rounded_box(case_hor_size - wall * 2, case_ver_size + case_extra_lower_space + case_extra_upper_space - wall * 2, case_height, rounded_radius - wall / 2);
      translate([0, 0, wall - 1]) case_screws(case_screw_holder_height + 1, case_screw_holder_diameter);
    }
    translate([0, 0, -1]) cylinder(h = wall + 2, r = pipe_radius);
    translate([0, 0, -1]) case_screws(wall + case_screw_holder_height + 2, case_screw_diameter);
    // PIR
    translate([case_hor_size / 2 - wall - 1, case_ver_size / 2 + case_extra_lower_space - pir_offset, case_height / 2]) rotate([0, 90, 0]) cylinder(d = pir_diameter, h = wall + 2);
    // LED
    translate([case_hor_size / 2 - wall, led_offset, (case_height - led_size) / 2]) led(1);
    // Wire hole
    translate([wire_hole_hor_pos, wire_hole_ver_pos, -1])
    rounded_box(wire_hole_width, wire_hole_height, wall + 2, 5);
    cube([wire_hole_width, wire_hole_height, wall + 2]);
  }
  translate([-power_length / 2, power_case_offset, wall + 0]) power_fixation(has_psu);
  translate([case_hor_size / 2 - wall - lid_fixation_size / 2, -case_ver_size / 2 - case_extra_upper_space + lid_fixation_size * 2, case_height - lid_wall]) case_fixation(true, lid_fixation_height);
  translate([case_hor_size / 2 - wall - lid_fixation_size / 2, case_ver_size / 2 + case_extra_lower_space - lid_fixation_size * 2, case_height - lid_wall]) case_fixation(true, lid_fixation_height);
  translate([case_hor_size / 2 - wall - lid_fixation_size / 2, case_extra_lower_space / 2 - case_extra_upper_space / 2, case_height - lid_wall]) case_fixation(true, lid_fixation_height);
  translate([-case_hor_size / 2 + wall + lid_fixation_size / 2, -case_ver_size / 2 - case_extra_upper_space + lid_fixation_size * 2, case_height - lid_wall]) case_fixation(false, lid_fixation_height);
  translate([-case_hor_size / 2 + wall + lid_fixation_size / 2, case_ver_size / 2 + case_extra_lower_space - lid_fixation_size * 2, case_height - lid_wall]) case_fixation(false, lid_fixation_height);
  translate([-case_hor_size / 2 + wall + lid_fixation_size / 2, case_extra_lower_space / 2 - case_extra_upper_space / 2, case_height - lid_wall]) case_fixation(false, lid_fixation_height);
}

module fan() {
  color("red") difference() {
//    translate([-outter_fan_radius, -outter_fan_radius, 0]) cube([outter_fan_radius * 2, outter_fan_radius * 2, fan_height]);
    rounded_box(outter_fan_radius * 2, outter_fan_radius * 2, fan_height, outter_fan_radius - fan_screw_position);
    translate([0, 0, -1]) fan_screws(fan_height + 2, fan_screw_diameter);
    translate([0, 0, -1]) cylinder(h = fan_height + 2, r = inner_fan_radius);
  }
}

module pipe(has_fan) {
  translate([0, 0, 0]) color("white") difference() {
    union() {
      //translate([-pipe_radius, -pipe_radius, 0]) cube([pipe_radius * 2, pipe_radius * 2, pipe_lid_wall]);
      rounded_box(pipe_radius * 2, pipe_radius * 2, pipe_lid_wall, pipe_corner_radius);
      cylinder(h = pipe_length + pipe_lid_wall, r = pipe_radius);
    }
    translate([0, 0, -1]) cylinder(h = pipe_lid_wall + 2, r = ((pipe_radius - pipe_wall) > inner_fan_radius) ? inner_fan_radius : (pipe_radius - pipe_wall));
    translate([0, 0, pipe_lid_wall]) cylinder(h = pipe_length + 1, r = pipe_radius - pipe_wall);
    // screws
    translate([0, 0, -1]) fan_screws(pipe_lid_wall + mattress_height + 2, pin_diameter);
    translate([0, 0, pipe_lid_wall - pin_height]) fan_screws(pin_height + 1, pin_top_diameter);
    cylinder(r1 = inner_fan_radius, r2 = pipe_radius - pipe_wall, h = pipe_lid_wall);
  }
  if (has_fan) {
    translate([0, 0, -fan_height]) fan();
  }
}

module lid() {
  difference() {
    union() {
      translate([0, case_extra_lower_space / 2 - case_extra_upper_space / 2, 0])
        rounded_box(case_hor_size - wall * 2, case_ver_size + case_extra_lower_space + case_extra_upper_space - wall * 2, lid_wall, rounded_radius - wall / 2);
      if (has_magnet) {
        all_magnet_hodler(false);
      }
    }
    translate([0, 0, -1]) cylinder(r = inner_fan_radius, h = lid_wall + 2);
    if (has_lid_screw) {
      // Screw holes to screw the lid to the case
      translate([case_hor_size / 2 - wall - lid_fixation_size / 2, -case_ver_size / 2 - case_extra_upper_space + lid_fixation_size * 2, -1]) cylinder(h = lid_wall + 2, r = lid_screw_radius);
      translate([case_hor_size / 2 - wall - lid_fixation_size / 2, case_ver_size / 2 + case_extra_lower_space - lid_fixation_size * 2, -1]) cylinder(h = lid_wall + 2, r = lid_screw_radius);
      translate([-case_hor_size / 2 + wall + lid_fixation_size / 2, -case_ver_size / 2 - case_extra_upper_space + lid_fixation_size * 2, -1]) cylinder(h = lid_wall + 2, r = lid_screw_radius);
      translate([-case_hor_size / 2 + wall + lid_fixation_size / 2, case_ver_size / 2 + case_extra_lower_space - lid_fixation_size * 2, -1]) cylinder(h = lid_wall + 2, r = lid_screw_radius);
      translate([case_hor_size / 2 - wall - lid_fixation_size / 2, case_extra_lower_space / 2 - case_extra_upper_space / 2, -1]) cylinder(h = lid_wall + 2, r = lid_screw_radius);
      translate([-case_hor_size / 2 + wall + lid_fixation_size / 2, case_extra_lower_space / 2 - case_extra_upper_space / 2, -1]) cylinder(h = lid_wall + 2, r = lid_screw_radius);
    }
    if (has_magnet) {
      all_magnet_hodler(true);
    }
    translate([0, 0, lid_wall - filter_height])  filter();
  }
}

module only_led() {
    color("white") translate([case_hor_size / 2 - wall + 0.5, led_offset, (case_height - led_size) / 2]) led(-0.5);
    verticale_size = case_ver_size + case_extra_lower_space + case_extra_upper_space;
    cube_size = 0.2;
    at_corner = true;
    x = case_hor_size / 2 + (at_corner ? 0 : 4);
    upper_y = case_ver_size / 2 + case_extra_upper_space - (at_corner ? 0 : 50);
    lower_y = case_ver_size / 2 + case_extra_lower_space - (at_corner ? 0 : 50);
    translate([x - cube_size, -upper_y, 0]) cube([cube_size, cube_size, cube_size]);
    translate([-x, -upper_y, 0]) cube([cube_size, cube_size, cube_size]);
    translate([x - cube_size, lower_y - cube_size, 0]) cube([cube_size, cube_size, cube_size]);
    translate([-x, lower_y - cube_size, 0]) cube([cube_size, cube_size, cube_size]);
}
