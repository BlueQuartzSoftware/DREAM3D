Find ODF {#findodf}
======


## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
 The Filter finds the _Orientation Distribution Function _(ODF).

The ODF is defined as the volume fraction of **Features** with a certain orientation vector.
The orientation vector is normally identified using three _Euler_ angles which are used to describe the transition from the _sample_’s reference frame into the _crystallographic_ reference frame of each individual **Feature** of the polycrystal. One thus ends up with a large set of different _Euler_ angles, the distribution of which is described by the ODF.
The full 3D representation of crystallographic texture is given by the _Orientation Distribution Function_ (ODF). Subsequently, all pole figures can be derived from the ODF.
--------------
 _Source: http://en.wikipedia.org/wiki/Texture_(crystalline)
 For in-depth discussion of Texture and ODFs the following source is suggested:
 http://neon.mems.cmu.edu/rollett/27750/27750.html
 This site (Prof. Tony Rollett, CMU) contains source material that is very helpful in explaining Texture and the related concepts, including ODF._
 


## Parameters ##

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | FeatureEulerAngles | Three (3) angles (floats) defining the orientation of each **Feature** in Bunge convention (Z-X-Z) | Filter will calculate average Euler angles for each **Feature** if not already calculated. |
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature** | Filter will determine **Feature** phases if not previously determined |
| Feature | Volumes | Volume (float) in um^3 of the **Feature**. | Filter will calculate the volume of each **Feature** if not already calculated |
| Feature | SurfaceFeatures | Boolean flag of 1 if **Feature** touches an outer surface or of 0 if it does not | Filter will determine if **Features** intersect an outer surface if not already determined |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |

## Created Arrays ##
None


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


