WritePoleFigure {#writepolefigure}
=====


## Group (Subgroup) ##
IOFilters (Output)


## Description ##
This filter creates standard pole figure images for the microstructure that is being analyzed. The filter uses Euler Angles which MUST be in radians to work correctly. The filter also requires the Crystal Structures ensemble array and the Cell Phases data array.

If the "Good Voxels" array has been created from another filter this filter will use that information to determine if a voxel's Euler Angle data should be added to the array that will be used for the calculation of the Pole Figure. Allowing the use of non-indexed EBSD data (which shows up as Bad Data) will cause errors or undefined behavior in this filter. The user is strongly cautioned to understand their data and where it comes from before running this filter.

## Parameters ##
| Name             | Type |
|------------------|------|
| Output Path | String |
| Image Type | Constant Value |  
| Image Prefix| String |  
| Size of Image | Int (Pixels) |


## Required Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Int  | Phases           | ....        | other   |  
| Float x 3  | Eulers           | ....        | Must be in Radians   |  
|   Int | Crystal Structures           | ....        | Ensemble Array   |


## Created Arrays ##

None



## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL), 2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)
