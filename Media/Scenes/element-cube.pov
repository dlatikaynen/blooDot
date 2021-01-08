/* [dlatikay] 20201108
 * blooDot
 * Artwork
 * Backlit chemical element cubes
 * (beware of tendency to self-destruct)
 */                    

#version 3.6;
 
#include "finish.inc"
#include "golds.inc"
#include "metals.inc"
#include "glass.inc"

#declare Photons = on;
#declare element = "?"
#declare blockColor = rgbt <0.2, 0.44, 0.98, 0.7>;       

#switch(frame_number)
    #case(88)
        #declare element = "Ra"    
        #declare blockColor = rgbt <0.04, 0.99, 0.02, 0.41>;
        light_source
        {
            <0, 0, -1.5>
            color rgb <0.8, 1.0, 0.8>
        }
        
        #break
#end

#declare blockTexture = texture
{      
    pigment
    {
        blockColor
    }

    finish
    {
        ambient 0.2
        diffuse 0.05
        specular 0.5 + 0.2 * 0.5
        roughness 0.05 + 0.03 * 0.5
        reflection
        {
            0.1,
            1
            fresnel on
        }
        
        conserve_energy
    }
};

#switch(frame_number)
    #case(1)
        #declare element = "H"            
        #break
    #case(2)
        #declare element = "He"    
        #break
    #case(3)
        #declare element = "Li"    
        #break
    #case(4)
        #declare element = "Be"    
        #break
    #case(5)
        #declare element = "B"    
        #break
    #case(6)
        #declare element = "C"    
        #break
    #case(7)
        #declare element = "N"    
        #break
    #case(8)
        #declare element = "O"    
        #break
    #case(9)
        #declare element = "F"    
        #break
    #case(10)
        #declare element = "Ne"    
        #break
    #case(11)
        #declare element = "Na"    
        #break
    #case(12)
        #declare element = "Mg"    
        #break
    #case(13)
        #declare element = "Al"    
        #break
    #case(14)
        #declare element = "Si"    
        #break
    #case(15)
        #declare element = "P"    
        #break
    #case(16)
        #declare element = "S"    
        #break
    #case(17)
        #declare element = "Cl"    
        #break
    #case(18)
        #declare element = "Ar"    
        #break
    #case(19)
        #declare element = "K"    
        #break
    #case(20)
        #declare element = "Ca"    
        #break
    #case(21)
        #declare element = "Sc"    
        #break
    #case(22)
        #declare element = "Ti"    
        #break
    #case(23)
        #declare element = "V"    
        #break
    #case(24)
        #declare element = "Cr"    
        #declare blockTexture = T_Chrome_2D;    
        #break
    #case(25)
        #declare element = "Mn"    
        #break
    #case(26)
        #declare element = "Fe"    
        #break
    #case(27)
        #declare element = "Co"    
        #break
    #case(28)
        #declare element = "Ni"    
        #break
    #case(29)
        #declare element = "Cu"    
        #declare blockTexture = T_Copper_3D;    
        #break
    #case(30)
        #declare element = "Zn"    
        #break
    #case(31)
        #declare element = "Ga"    
        #break
    #case(32)
        #declare element = "Ge"    
        #break
    #case(33)
        #declare element = "As"    
        #break
    #case(34)
        #declare element = "Se"    
        #break
    #case(35)
        #declare element = "Br"    
        #break
    #case(36)
        #declare element = "Kr"    
        #break
    #case(37)
        #declare element = "Rb"    
        #break
    #case(38)
        #declare element = "Sr"    
        #break
    #case(39)
        #declare element = "Y"    
        #break
    #case(40)
        #declare element = "Zr"    
        #break
    #case(41)
        #declare element = "Nb"    
        #break
    #case(42)
        #declare element = "Mo"    
        #break
    #case(43)
        #declare element = "Tc"    
        #break
    #case(44)
        #declare element = "Ru"    
        #break
    #case(45)
        #declare element = "Rh"    
        #break
    #case(46)
        #declare element = "Pd"    
        #break
    #case(47)
        #declare element = "Ag"    
        #declare blockTexture = T_Silver_5D;    
        #break
    #case(48)
        #declare element = "Cd"    
        #break
    #case(49)
        #declare element = "In"    
        #break
    #case(50)
        #declare element = "Sn"    
        #break
    #case(51)
        #declare element = "Sb"    
        #break
    #case(52)
        #declare element = "Te"    
        #break
    #case(53)
        #declare element = "I"    
        #break
    #case(54)
        #declare element = "Xe"    
        #break
    #case(55)
        #declare element = "Cs"    
        #break
    #case(56)
        #declare element = "Ba"    
        #break
    #case(57)
        #declare element = "La"    
        #break
    #case(58)
        #declare element = "Ce"    
        #break
    #case(59)
        #declare element = "Pr"    
        #break
    #case(60)
        #declare element = "Nd"    
        #break
    #case(61)
        #declare element = "Pm"    
        #break
    #case(62)
        #declare element = "Sm"    
        #break
    #case(63)
        #declare element = "Eu"    
        #break
    #case(64)
        #declare element = "Gd"    
        #break
    #case(65)
        #declare element = "Tb"    
        #break
    #case(66)
        #declare element = "Dy"    
        #break
    #case(67)
        #declare element = "Ho"    
        #break
    #case(68)
        #declare element = "Er"    
        #break
    #case(69)
        #declare element = "Tm"    
        #break
    #case(70)
        #declare element = "Yb"    
        #break
    #case(71)
        #declare element = "Lu"    
        #break
    #case(72)
        #declare element = "Hf"    
        #break
    #case(73)
        #declare element = "Ta"    
        #break
    #case(74)
        #declare element = "W"    
        #break
    #case(75)
        #declare element = "Re"    
        #break
    #case(76)
        #declare element = "Os"    
        #break
    #case(77)
        #declare element = "Ir"    
        #break
    #case(78)
        #declare element = "Pt"    
        #break
    #case(79)
        #declare element = "Au"    
        #declare blockTexture = T_Gold_1D;
        #break
    #case(80)
        #declare element = "Hg"    
        #break
    #case(81)
        #declare element = "Tl"    
        #break
    #case(82)
        #declare element = "Pb"    
        #break
    #case(83)
        #declare element = "Bi"    
        #break
    #case(84)
        #declare element = "Po"    
        #break
    #case(85)
        #declare element = "At"    
        #break
    #case(86)
        #declare element = "Rn"    
        #break
    #case(87)
        #declare element = "Fr"    
        #break
    #case(89)
        #declare element = "Ac"    
        #break
    #case(90)
        #declare element = "Th"    
        #break
    #case(91)
        #declare element = "Pa"    
        #break
    #case(92)
        #declare element = "U"    
        #break
    #case(93)
        #declare element = "Np"    
        #break
    #case(94)
        #declare element = "Pu"    
        #break
    #case(95)
        #declare element = "Am"    
        #break
    #case(96)
        #declare element = "Cm"    
        #break
    #case(97)
        #declare element = "Bk"    
        #break
    #case(98)
        #declare element = "Cf"    
        #break
    #case(99)
        #declare element = "Es"    
        #break
    #case(100)
        #declare element = "Fm"    
        #break
    #case(101)
        #declare element = "Md"    
        #break
    #case(102)
        #declare element = "No"    
        #break
    #case(103)
        #declare element = "Lr"    
        #break
    #case(104)
        #declare element = "Rf"    
        #break
    #case(105)
        #declare element = "Db"    
        #break
    #case(106)
        #declare element = "Sg"    
        #break
    #case(107)
        #declare element = "Bh"    
        #break
    #case(108)
        #declare element = "Hs"    
        #break
    #case(109)
        #declare element = "Mt"    
        #break
    #case(110)
        #declare element = "Ds"    
        #break
    #case(111)
        #declare element = "Rg"    
        #break
    #case(112)
        #declare element = "Cn"    
        #break
    #case(113)
        #declare element = "Nh"    
        #break
    #case(114)
        #declare element = "Fl"    
        #break
    #case(115)
        #declare element = "Mc"    
        #break
    #case(116)
        #declare element = "Lv"    
        #break
    #case(117)
        #declare element = "Ts"    
        #break
    #case(118)
        #declare element = "Og"    
        #break
#end

global_settings
{
    assumed_gamma 1.0
    max_trace_level 5  
    #if (Photons)
        photons
        {
            spacing 0.02
        }
    #end
}

#declare scaleFactor = 1.0;              
#declare downABit = 0.0;
#switch(strlen(element))
    #case(1)
        #declare scaleFactor = 1.0;
        #declare downABit = -0.1;
        #break
    #case(2)
        #declare scaleFactor = 0.8;
        #break
    #case(3)
        #declare scaleFactor = 0.6;
        #break
#end

#declare M_Glass = material
{
    texture
    {
        blockTexture
    }
  
    interior
    {
        ior 1.5    
        fade_distance 0.9
        fade_color <0.7, 0.8, 0.6>       
        caustics 0.18
    }
}

camera
{
    right x * image_width / image_height
    location <0, 0, -2.2>
    look_at <0, 0, 0>
}
                                 
light_source
{
    <0, 0, 2>
    color rgb <1, 0.5, 0.5>
    photons
    {
        refraction on
        reflection on
    }
}

light_source
{
    <-0.2, 0.2, 2>
    color rgb <1, 0.5, 0.5>
    photons
    {
        refraction on
        reflection on
    }
} 

light_source
{
    <0.2, -0.2, 2>
    color rgb <1, 0.5, 0.5>
    photons
    {
        refraction on
        reflection on
    }
}

light_source
{
    <-40, 50, -100>
    color rgb <1, 0.5, 0.5>
    photons
    {
        refraction on
        reflection on
    }
} 

#declare Inside = text
{
    ttf "ariblk.ttf",
    element,
    0.2, // depth
    0 // spacing
    texture
    {
        pigment
        {
            color <0, 0, 0, 1>
        }
    }
    
    scale 1.3 * scaleFactor
    translate <-0.8, -0.4 + downABit, -0.1>
}                             
                    
#declare H = difference
{
    box
    {
        <-1, -1, 0>,
        <1, 1, 0.5>
        material
        {
            M_Glass
        }
    }                          

    object
    {
        Inside
        translate -(min_extent(Inside) + max_extent(Inside))/2 * x
    }
}

#declare Sunken = text
{
    ttf "ariblk.ttf",
    element,
    1, // depth
    0 // spacing  
    texture
    {
        pigment
        {
            color <1, 1, 1, 0>
        }
    }                      
  
    finish
    {
        ambient 0.27
        phong 0.8
    }

    scale 1.3 * scaleFactor
    translate <-0.8, -0.4 + downABit, 0.1>  
}

object
{
    H
} 

light_source
{
    <0, -2, 0.25>
    color <0, 1, 1>
    spotlight
    radius 15
    falloff 20
    tightness 10
    point_at <0, 0, 0.25>
}

object
{
    Sunken
    translate -(min_extent(Sunken)+max_extent(Sunken))/2 * x
}
