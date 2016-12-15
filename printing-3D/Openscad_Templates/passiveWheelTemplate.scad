// title      : RoboGen passiveWheel
// author     : Alice Concordel
// date     : 01.12.2016

//passiveWheel(0.04); //the parameter is the radius of the wheel

module passiveWheel(wheelRadius)
{
// apply transformations to the parameter
wheelRadius = wheelRadius*1000; //convert to mm

//plate parameters
	wheelThickness = 3;
	hubRadius = 9;
	hubThickness = 9;
	radiusShaftHole = 3/2+0.3;  // motor shaft diameter is 3 mm, the extra is adjustment for printing
	radiusScrewHole = 2/1+0.1; //Use a M2.5 (fine thread) screw

	rimThickness = wheelThickness;
	spokeThickness = 3;
	numberSpokes = 3;
    filletRadius = 5;
    numberTeeth = 200;

	union()
	{
		difference()
		{
//positive
			union(){ //main disk
				cylinder(r = wheelRadius, h = wheelThickness, $fn = 40);
			}
//negative     
    
        //coutouts
        translate([0, 0, -0.5]) //cutout parts stick out by 0.5 top and bottom (+1 in z dimension, then shift down by 0.5)
            union(){
                //central shafts
				cylinder(r = radiusShaftHole, h = hubThickness+1, $fn = 30);
                //spoke cutouts
				spokeHoles(numberSpokes, wheelRadius, rimThickness, hubRadius, wheelThickness, spokeThickness, filletRadius);
                //teeth
                teeth(wheelRadius,wheelThickness, numberTeeth);
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
module spokeHoles(numberSpokes, wheelRadius, rimThickness, hubRadius, wheelThickness, spokeThickness, filletRadius){
    linear_extrude(height =wheelThickness+1){
	offset(r=+filletRadius)  offset(delta=-filletRadius) 
		difference(){
			circle(r = wheelRadius-rimThickness, $fn = 40); //initial disk
			circle(r = hubRadius, $fn = 30); //hub hole
			for (i = [0:numberSpokes]){ //spoke holes
				rotate([0, 0, i*360/numberSpokes]) translate([-spokeThickness/2, -spokeThickness/2, 0]) square([wheelRadius-1,spokeThickness, ]);
			}
		}
    }
}

//Create shape of coutouts for teeth
module teeth(wheelRadius, wheelThickness, numberTeeth){
    linear_extrude(height =wheelThickness+1){
	//offset(r=+filletRadius)  offset(delta=-filletRadius) 
        for (i = [0:numberTeeth]){ //teeth
            rotate([0, 0, i*360/numberTeeth]) polygon(points=[[wheelRadius-1.1,0.2],[wheelRadius-1.1,-0.2],[wheelRadius+1,-1],[wheelRadius+1,1]]);
        }
    }
}
