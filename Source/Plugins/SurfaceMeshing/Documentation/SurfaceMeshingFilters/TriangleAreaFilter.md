Generate Triangle Areas {#triangleareafilter}
============

## Group (Subgroup) ##
Surface Meshing (Misc)

## Description ##
This **Filter** computes the area of each **Triangle** in a **Triangle Geometry** by calculating the following: 
	
	1/2*|AB||AC|sin(O)

where _O_ is the angle between |AB| and |AC|.

## Parameters ##
None

## Required Geometry ##
Triangle

## Required Objects ##
None

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Face Attribute Array**  | FaceAreas | double | (1) | Specifies the area of each **Face** |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


