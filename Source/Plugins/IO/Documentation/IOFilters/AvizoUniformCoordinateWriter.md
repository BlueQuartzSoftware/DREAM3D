Avizo Uniform Coordinate Writer {#avizouniformcoordinatewriter}
===========

## Group (Subgroup) ##
I/O Filters (Output)

## Description ##

This filter writes out a native Avizo Uniform Coordinate data file.

### Example Output ###


	# AmiraMesh BINARY-LITTLE-ENDIAN 2.1
	# Dimensions in x-, y-, and z-direction
	define Lattice
	define Coordinates
	Parameters {
	    DREAM3DParams {
	        Author "DREAM3D",
	         DateTime     
	    }
	    Units {
	          Coordinates "microns"
	    }
	     Content "   int, uniform coordinates",
	     # Bounding Box is xmin xmax ymin ymax zmin zmax
	     BoundingBox      ,
	 CoordType "uniform"
    }
	Lattice { int FeatureIds } = @1
	# Data section follows
	….. 

## Parameters ##

| Name | Type |
|------|------|
| Output File | Amira Mesh .am |
| Write Binary File | Boolean (On or Off) |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding)

## Created Arrays ##
None



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


