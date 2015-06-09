Write Dx File (Feature Ids) {#dxwriter}
======

## Group (Subgroup) ##
I/O Filters (Output)


## Description ##

This Filter writes to a Dx file. The file format is that used by the _OpenDX Source Software_ based on IBM's _Visualization Data Explorer_. 
For an example format, see the DXReader filter descriptions. Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute.

_See: www.opendx.org for more information._


### Example Output ###
The dx file:     

	object 1 class gridpositions counts 118 202 190
	origin 0 0 0
	delta  1 0 0  
	delta  0 1 0
	delta  0 0 1 
	
	object 2 class gridconnections counts 118 202 190
	
	object 3 class array type int rank 0 items 4444713 data follows  
	1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 
	…..
	
The dx file with surface layer:     

	object 1 class gridpositions counts 120 204 192
	origin 0 0 0
	delta  1 0 0  
	delta  0 1 0
	delta  0 0 1 
	
	object 2 class gridconnections counts 120 204 192
	
	object 3 class array type int rank 0 items 4613987 data follows  
	-3 -3 -3 -3 -3 -3 -3 -3 -3 -3 -3 -3 -3 -3 -3 -3 -3 -3 -3 -3  
	…..
	

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Output File | File Path | Create a file with extension .dx |
| Add Surface Layer | Boolean | This adds and "extra layer" of **Cells** around the outside of the structure and assigns them a *FeatureId* of *-3*. |

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Ids (int32) that specify to which **Feature** each **Cell** belongs. | (1) | Filters Known to Create Data: Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding)

## Created Arrays ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


