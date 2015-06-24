Generate Triangle Areas {#triangleareafilter}
============

## Group (Subgroup) ##
Surface Meshing (Misc)

## Description ##
This Filter computes the area of each **Triangle** in a **Triangle Geometry** by calculating the following: 
	
	1/2*|AB||AC|sin(O)

where _O_ is the angle between |AB| and |AC|.

## Parameters ##
None

## Required Geometry ##
Triangle

## Required Arrays ##
None

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Face | FaceAreas | Double | (1) | Specifies the area of each **Face** |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


