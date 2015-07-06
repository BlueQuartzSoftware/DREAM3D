Avizo Uniform Coordinate Writer {#avizouniformcoordinatewriter}
===========

## Group (Subgroup) ##
IO (Output)

## Description ##

This filter writes out a native Avizo Uniform Coordinate data file. Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute.

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
| Name | Type | Description |
|------|------| ----------- |
| Output File | File Path | Amira Mesh .am file created|
| Write Binary File | Boolean | Binary file created, if checked |


## Required Geometry ##
Image / Rectilinear Grid


## Required Objects ##

| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Ids (Int) that specify to which **Feature** each **Cell** belongs. | (1) | Filters Known to Create Data: Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding)


## Created Objects ##
None



## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


