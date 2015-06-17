Los Alamos FFT Writer {#losalamosfftwriter}
=====

## Group (Subgroup) ##
I/O Filters (Output)


## Description ##
This filter writes out a file that is appropriate to use as an input into Ricardo Lebensohn's FFT 3D simulation codes[1].

The format of the file is an ASCII text file with the following space delimited features

	Phi1   Phi   Phi2  X  Y   Z  Feature_ID   Phase_ID

The Euler Angles are in Degrees.
X, Y, Z are integer indices into the Voxel volume
Feature ID & Phase ID are the integer values for the feature and phase **Starting at One (1)**.


[1] Lebensohn, R.A., 2001. N-site modeling of a 3D viscoplastic polycrystal using Fast Fourier Transform. Acta mater. 49, 2723-2737.

### Example Output ###
The output file:     

	90.000 0.000 0.000 1 1 1 0 1
	90.000 0.000 0.000 2 1 1 0 1 
	90.000 0.000 0.000 3 1 1 0 1  
	90.000 0.000 0.000 4 1 1 0 1
	90.000 0.000 0.000 5 1 1 0 1  
	90.000 0.000 0.000 6 1 1 0 1
	90.000 0.000 0.000 7 1 1 0 1
	90.000 0.000 0.000 8 1 1 0 1
	90.000 0.000 0.000 9 1 1 0 1  
	90.000 0.000 0.000 10 1 1 0 1
	90.000 0.000 0.000 11 1 1 0 1 
	90.000 0.000 0.000 12 1 1 0 1  
	90.000 0.000 0.000 13 1 1 0 1
	90.000 0.000 0.000 14 1 1 0 1  
	90.000 0.000 0.000 15 1 1 0 1
	90.000 0.000 0.000 16 1 1 0 1
	90.000 0.000 0.000 17 1 1 0 1
	135.009 55.304 295.274 18 1 1 1742 1 
	90.000 0.000 0.000 19 1 1 0 1 
	…..  

## Parameters ##

| Name             | Type | Description |
|------------------|------|-------------|
| Output File |  File Path  |


## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##

| Type | Default Name | Type | Componenet Dimensions | Description |
|------|--------------------|-------------|---------|
| Cell | FeaturesIds (ints) that specify to which **Feature** each **Cell** belongs. | Int | (1) |Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Cell | CellEulerAngles | Float | (3) |These are the angles used to determine the colors. Read H5Ebsd File (IO), Match Crystallography (SyntheticBuilding) |
| Cell | CellPhases | Int | (1) |  These are used to determine which ensemble the **Cell** belongs to. Read H5Ebsd File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |


## Created Arrays ##

None


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


