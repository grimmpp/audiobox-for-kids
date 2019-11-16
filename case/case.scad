//This file was created in openscad
//Cheatsheet for openscad http://www.openscad.org/cheatsheet/

w=20;
l=200;

//bottom
color("BurlyWood") cube([l,l,w]);
//top
color("BurlyWood") translate([0,0,l-w]) cube([l,l,w]);
//left
color("Tan") translate([0,0,w]) cube([l,w,l-2*w]);
//right
color("Tan") translate([0,l-w,w]) cube([l,w,l-2*w]);

//back
//color("BurlyWood") translate([0,w,w]) cube([w,l-2*w,l-2*w]);

BackSupport();

//front
color("BurlyWood") translate([l-w,w,w]) cube([w,l-2*w,l-2*w]);

SpeakerFront();
Feet();
Buttons();
CardMark();

module BackSupport() {
    //back support bottom
    color("Wheat") translate([w,w,w]) cube([w,l-2*w,w]);
    //back support top
    color("Wheat") translate([w,w,l-2*w]) cube([w,l-2*w,w]);
    //back support right
    color("Wheat") translate([w,l-2*w,w]) cube([w,w,l-2*w]);
    //back support right
    color("Wheat") translate([w,w,w]) cube([w,w,l-2*w]);
    Nuts();
}

module SpeakerFront() {
    //difference() {
        color([0.2,0.2,0.2]) translate([l,l/2,l/2]) scale([0.4,1,1]) sphere($fn=50, d=140);
        //translate([l-w,w,w]) cube([w,l-2*w,l-2*w]);
    //}
}

module Feet() {
    d=30;
    translate([d,d,-10]) color([0.2,0.2,0.2]) cylinder($fn=36, h=10, d=40);
    translate([d,l-d,-10]) color([0.2,0.2,0.2]) cylinder($fn=36, h=10, d=40);
    translate([l-d,d,-10]) color([0.2,0.2,0.2]) cylinder($fn=36, h=10, d=40);
    translate([l-d,l-d,-10]) color([0.2,0.2,0.2]) cylinder($fn=36, h=10, d=40);
}

module Buttons() {
    translate([l-50,l/2-45,l]) Button("red");
    translate([l-50,l/2,l]) Button("blue");
    translate([l-50,l/2+45,l]) Button("green");
}

module Button(c) {
    color([0.2,0.2,0.2]) cylinder($fn=100, d=22, h=3);
    color(c) cylinder($fn=100, d=18, h=8);
}

module CardMark() {
    color([0.4,0.4,0.4]) translate([l/2-60,l/2-40,l]) cube([54,2,0.1]);
    color([0.4,0.4,0.4]) translate([l/2-60,l/2+40,l]) cube([54,2,0.1]);
    color([0.4,0.4,0.4]) translate([l/2-60+54,l/2+41-81,l]) cube([2,82,0.1]);
    color([0.4,0.4,0.4]) translate([l/2-60,l/2+41-81,l]) cube([2,82,0.1]);
}

module Nuts() {
    translate([w,w+10,w+10]) Nut();    
    translate([w,l-w-10,w+10]) Nut();
    translate([w,l-w-10,l-w-10]) Nut();
    translate([w,w+10,l-w-10]) Nut();
}

module Nut() {
    translate([-0.1,0,0]) color("Goldenrod") rotate([0,90,0]) rotate([0,0,30]) cylinder($fn=6, h=0.1, d=12);
    translate([-0.2,0,0]) color("DimGray") rotate([0,90,0]) cylinder($fn=36, h=0.1, d=5);
}