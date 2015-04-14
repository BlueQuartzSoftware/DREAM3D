Write Pole Figure {#writepolefigure}
=====

## Group (Subgroup) ##
IOFilters (Output)


## Description ##
This filter creates standard pole figure images for the microstructure that is being analyzed. The filter uses Euler Angles which MUST be in radians to work correctly. The filter also requires the Crystal Structures ensemble array and the Cell Phases data array.

If the "Good Voxels" array has been created from another filter this filter will use that information to determine if a voxel's Euler Angle data should be added to the array that will be used for the calculation of the Pole Figure. Allowing the use of non-indexed EBSD data (which shows up as Bad Data) will cause errors or undefined behavior in this filter. The user is strongly cautioned to understand their data and where it comes from before running this filter.

The pole figure algorithm uses a "Modified Lambert Square" to perform the interpolations onto the circle. This is an alternate type of interpolation that the EBSD OEMs do not perform which may make the output from DREAM3D look slightly different than output obtained from the OEM programs.

**Only an advanced user with intimate knowledge of the Modified Lambert Projection should attempt to change the value for the "Lamber Image Size (Pixels)" input parameter.**

![Example Pole Figure using the Square Layout option](images/PoleFigure_Example.png)


## Parameters ##
| Name             | Type |
|------------------|------|
| Output Path | String |
| Image format | tif, bmp, png |  
| Image Prefix| String |  
| Size of Image | Int (Pixels) |
| Image Layout | Horizontal=0, Vertical=1, Square=2 |
| int  | Lambert Image Size  (32 Pixels is the default) |
| int  | Number of Colors  (32 is the default)|
 

## Required Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Float x 3  | Eulers       |             | Must be in Radians   |  
| Int  | Phases             | ....        | other   |  
| Int | Crystal Structures  |             | Ensemble Array   |
| bool | Good Voxels 			|			   |                 |




## Created Arrays ##

None



## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)
