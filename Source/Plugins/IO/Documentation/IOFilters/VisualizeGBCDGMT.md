Write GBCD Pole Figure (GMT 5) {#visualizegbcdgmt}
============

## Group (Subgroup) ##
IO (Output)

## Description ##
This **Filter** creates a .dat file that can be used in conjunction with [GMT](http://gmt.soest.hawaii.edu/) to generate a grain boundary character distribution (GBCD) pole figure. The user must select the relevant phase for which to write the pole figure by entering the _phase index_. 

-----

![GMT Visualization of the Small IN100 GBCD Results](Images/SmallIn100GMT_.png)
@image latex SmallIn100GMT_.png "GMT Output of GBCD Data" width=6in 

-----

## Parameters ##
| Name | Type | Description |
|------|------|-------------|
| Phase of Interest | int32_t | Index of the **Ensemble** for which to plot the pole figure |
| Crystal Structure | Enumeration | Crystal structure for GBCD. Currently supports from Hexagonal-High 6/mmm or Cubic-High m-3m symmetries |
| Misorientation Axis-Angle | float (4x) | Axis-Angle pair values for drawing GBCD |
| Output GMT File | File Path | The output .dat file path |

## Required Geometry ##
Triangle

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Ensemble Attribute Array** | GBDC | double | (n,m,l,o,q,r) | 5 parameter GBCD data. The 6<sup>th</sup> component is used internally to track the northern vs. southern hemisphere of the Lambert sphere |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


