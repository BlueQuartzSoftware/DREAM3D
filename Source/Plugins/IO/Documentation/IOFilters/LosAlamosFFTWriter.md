Los Alamos FFT Writer {#losalamosfftwriter}
=====

## Group (Subgroup) ##
IOFilters (Output)


## Description ##
This filter writes out a file that is appropriate to use as an input into Ricardo Lebensohn's FFT 3D simulation codes[1].

The format of the file is an ASCII text file with the following space delimited features

	Phi1   Phi   Phi2  X  Y   Z  Grain_ID   Phase_ID

The Euler Angles are in Degrees.
X, Y, Z are integer indices into the Voxel volume
Grain ID & Phase Id are the integer values for the grain and phase **Starting at One (1)**.


[1] Lebensohn, R.A., 2001. N-site modeling of a 3D viscoplastic polycrystal using Fast Fourier Transform. Acta mater. 49, 2723-2737.


## Parameters ##


| Name             | Type |
|------------------|------|
| Output File Path and Name |  String  |


## Required Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Cell | GrainIds Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Cell | CellEulerAngles | These are the angles used to determine the colors | Read H5Ebsd File (IO), Match Crystallography (SyntheticBuilding) |
| Cell | CellPhases |  These are used to determine which ensemble the **Cell** belongs to | Read H5Ebsd File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |


## Created Arrays ##

None



## Authors ##


**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


