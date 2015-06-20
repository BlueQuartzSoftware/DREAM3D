Write VTK Pole Figure (GBCD Data) {#visualizegbcdpolefigure}
============

## Group (Subgroup) ##
IO (Output)

## Description ##
This Filter creates a .vtk file that can be used in [ParaView](http://www.paraview.org/) to visualize a Grain Boundary Character Distribution (GBCD) pole figure.

-----

<center><img Test src="SmallIn100VTK.tif" alt="Drawing" style="width: 500px;"/></center>
<center><b>Regular Grid Visualization of GBCD Data</b></center>

-----

## Parameters ##
| Name | Type | Description |
|------|------|-------------|
| Crystal Structure | Enumeration | Crystal structure for GBCD. Currently supports from Hexagonal-High 6/mmm or Cubic-High m-3m symmetries |
| Misorientation Axis-Angle | Float (4x) | Axis-Angle pair values for drawing GBCD |
| Output Regular Grid VTK File | File Path | Output .vtk file path |

## Required Geometry ##
Triangle

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Ensemble | GBDC | Double | (6) | 5 parameter GBCD data. The 6<sup>th</sup> component is used internally to track the number of bins |

## Created Arrays ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


