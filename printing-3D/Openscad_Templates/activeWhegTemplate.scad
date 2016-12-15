// title      : RoboGen ActiveWheg
// author     : Alice Concordel
// date     : 01.12.2016

//activeWheg(0.06); //the parameter is the radius of the wheel

module activeWheg(whegRadius)
{
// apply transformations to the parameter
whegRadius = whegRadius*1000; //convert to mm

//plate parameters
	whegThickness = 3;
	hubRadius = 9;
	hubThickness = 9;
	radiusShaftHole = 3/2+0.3;  // motor shaft diameter is 3 mm, the extra is adjustment for printing
	radiusScrewHole = 2/1+0.1; //Use a M2.5 (fine thread) screw

	spokeThickness = 3;
	numberSpokes = 3;
    filletRadius = 5;
    numberTeeth = 200;

	union()
	{
		difference()
		{
//positive
			union(){//main disk
				cylinder(r = whegRadius, h = whegThickness, $fn = 40);
                //hub
                cylinder(r = hubRadius, h = hubThickness, $fn = 30);
			}
//negative     
        //carve edges of hub  
        difference() {
            translate([0, 0, hubThickness]) rotate_extrude(convexity = 10, $fn=40) translate([hubRadius, 0, 0]) circle(r = filletRadius, $fn = 40);
            translate([0, -10, 0]) cube([20,20, 20]);
        }
        
        translate([-1, hubRadius, hubThickness]) rotate([0, 90, 0]) cylinder(r = filletRadius, h = 12, $fn = 30);
        translate([-1, -hubRadius,hubThickness]) rotate([0, 90, 0]) cylinder(r = filletRadius, h = 12, $fn = 30);
        
        //coutouts
        translate([0, 0, -0.5]) //cutout parts stick out by 0.5 top and bottom (+1 in z dimension, then shift down by 0.5)
            union(){
                //central shafts
				cylinder(r = radiusShaftHole, h = hubThickness+1, $fn = 30);
                //leg cutouts
                spokes(numberSpokes, whegRadius, hubRadius, whegThickness, spokeThickness, filletRadius);
                //hole for tightening screw
                translate([0, 0, hubThickness-2.5]) rotate([0, 90, 0]) cylinder(r=1, h=30, $fn=20);
			}	
		}
	}
}

//-------------------------------------------------------------------//
// ------------------------- Submodules -----------------------//
//-------------------------------------------------------------------//
//Create shape of coutouts for wheels
module spokes(numberSpokes, whegRadius, hubRadius, whegThickness, spokeThickness, filletRadius){
    linear_extrude(height =whegThickness+1){
	offset(r=+filletRadius)  offset(delta=-filletRadius) 
		difference(){
			circle(r = whegRadius+10, $fn = 40); //initial disk
                union(){
                circle(r = hubRadius, $fn = 30); //hub hole
                for (i = [0:numberSpokes]){ //spoke holes
                    rotate([0, 0, i*360/numberSpokes]) translate([0, -spokeThickness/2, 0]) square([whegRadius-spokeThickness/2,spokeThickness, ]);
                    rotate([0, 0, i*360/numberSpokes]) translate([whegRadius-spokeThickness/2, 0, 0]) circle(r=spokeThickness/2, h =whegThickness, $fn=20);
                } 
            }
		}
    }
}
