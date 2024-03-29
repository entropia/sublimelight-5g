// For SP-150-24
//height = 50;
//space_front = 30;

// For RSP-150-27
height = 30;
space_front = 25;

width = 99;
shroud_depth = 35;

screw_depth = 25;
screw_height = 12.5;

cable_pos_right = width - 16;
cable_pos_center = width - 52;
cable_pos_left = width - 80;

screw_hole = 4;
cable_hole = 17.5;

wall = 2;

difference() {
    cube(size = [width + 2 * wall,
                 shroud_depth + space_front,
                 height + 2 * wall], 
         center = false);

    translate([wall, wall, wall])
    cube(size = [width, 
                 shroud_depth+space_front,
                 height], 
         center = false);
    
    translate([wall / 2,
           space_front + screw_depth,
           wall + screw_height])
    rotate([0, 90, 0])
    cylinder(h = 2*wall,
             r = screw_hole / 2,
             center = true,
             $fn = 24);

    translate([wall + width + wall / 2,
           space_front + screw_depth,
           wall + screw_height])
    rotate([0, 90, 0])
    cylinder(h = 2 * wall,
             r = screw_hole / 2,
             center = true,
             $fn = 24);

    translate([cable_pos_left,
               wall / 2,
               ( height + 2 * wall ) / 2])
    rotate([90,0,0])
    cylinder(h = 2 * wall, r = cable_hole / 2, center = true);

    translate([cable_pos_center,
               wall / 2,
               ( height + 2 * wall ) / 2])
    rotate([90,0,0])
    cylinder(h = 2 * wall, r = cable_hole / 2, center = true);

    translate([cable_pos_right,
               wall / 2,
               ( height + 2 * wall ) / 2])
    rotate([90,0,0])
    cylinder(h = 2 * wall, r = cable_hole / 2, center = true);
}

