/* [dlatikay] 20201108
 * blooDot
 * Artwork
 * Backlit chemical element cube
 * (beware of tendency to self-destruct)
 */                    
 
#include "finish.inc"
#include "golds.inc"
#include "metals.inc"
#include "glass.inc"

#version 3.6;

#declare Photons=on;

global_settings {     
  //ambient_light 0.5*clock
  assumed_gamma 1.0
  max_trace_level 5  
  #if (Photons)          // global photon block
    photons {
      spacing 0.02                 // specify the density of photons
      //count 100000               // alternatively use a total number of photons

      //gather min, max            // amount of photons gathered during render [20, 100]
      //media max_steps [,factor]  // media photons
      //jitter 1.0                 // jitter phor photon rays
      //max_trace_level 5          // optional separate max_trace_level
      //adc_bailout 1/255          // see global adc_bailout
      //save_file "filename"       // save photons to file
      //load_file "filename"       // load photons from file
      //autostop 0                 // photon autostop option
      //radius 10                  // manually specified search radius
      // (---Adaptive Search Radius---)
      //steps 1
      //expand_thresholds 0.2, 40
    }

  #end
}

#declare M_Glass=material {
  texture {      
    pigment {rgbt <0,0,1,0.7>}
    finish {
      ambient 0.2
      diffuse 0.05
      specular 0.5+0.2*clock
      roughness 0.05+0.03*clock
      reflection {
        0.1, 1
        fresnel on
      }
      conserve_energy
    }
  }
  interior {
    ior 1.5
    //fade_power 1001
    fade_distance 0.9
    fade_color <0.7,0.8,0.6>       
    caustics 0.18
  }
}

camera {
  right x*image_width/image_height
  location  <0,0,-2.2>
  //location  <5,0,0>
  look_at   <0,0,0>
}
                                 
light_source {
  <0,0,2>
  color rgb <1,0.5,0.5>
  photons {
    refraction on
    reflection on
  }
} 

light_source {
  <-0.2,0.2,2>
  color rgb <1,0.5,0.5>
  photons {
    refraction on
    reflection on
  }
} 

light_source {
  <0.2,-0.2,2>
  color rgb <1,0.5,0.5>
  photons {
    refraction on
    reflection on
  }
} 

light_source {
  <-40,50,-100>
  color rgb <1,0.5,0.5>
  photons {
    refraction on
    reflection on
  }
} 
                  
/* comment in for lighter version
light_source {
  <-50,40,-100>
  color rgb <1,1,1>
  photons {
    refraction on
    reflection on
  }
} 
*/
                    
#declare H=difference {
  box {                      
    <-1, -1, 0>, <1, 1, 0.5>
    material {
      M_Glass
    }
  }                          

  text {
    ttf "ariblk.ttf", "At",
    0.2, // depth
    0  // spacing
    texture { 
      pigment { color <0,0,0,1> }
    }
    scale 1.3
    translate <-0.8, -0.45, -0.1>  
  }                             
}

#declare Sunken=text {
  ttf "ariblk.ttf", "At",
  1, // depth
  0  // spacing
  texture { 
    pigment { color <1,1,1,0> }
  }                      
  finish {                       
    ambient 0.27
    phong 0.8
  }
  scale 1.3
  translate <-0.8, -0.45, 0.1>  
}
           
object {
  H
}
           
light_source {
    <0, -2, 0.25>
    color <0,1,1>
    spotlight
    radius 15
    falloff 20
    tightness 10
    point_at <0, 0, 0.25>
}

object {
  Sunken
}

