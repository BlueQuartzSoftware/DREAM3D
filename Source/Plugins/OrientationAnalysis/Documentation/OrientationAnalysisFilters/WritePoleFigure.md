WritePoleFigure {#writepolefigure}
=====


## Group (Subgroup) ##
IOFilters (Output)


## Description ##
This filter creates standard pole figure images for the microstructure that is being analyzed. The filter uses Euler Angles which MUST be in radians to work correctly. The filter also requires the Crystal Structures ensemble array and the Cell Phases data array.

If the "Good Voxels" array has been created from another filter this filter will use that information to determine if a voxel's Euler Angle data should be added to the array that will be used for the calculation of the Pole Figure. Allowing the use of non-indexed EBSD data (which shows up as Bad Data) will cause errors or undefined behavior in this filter. The user is strongly cautioned to understand their data and where it comes from before running this filter.


## Crystal Symmetry Implementations ##


| Symmetry | Laue Group   | Implemented |
|-------------|-----|------|
| Triclinic | -1 | Yes |
| Monoclinic | -2/m| Yes |
| Orthorhombic | mmm | Yes |
| Tetragonal-Low | 4/m | No |
| Tetragonal-High |4/mmm | No |
| Trigonal-Low | -3 | No |
| Trigonal-High  | -3m | No |
| Hexagonal-Low  |6/m| Yes |
| Hexagonal-High  | 6/mmm | Yes |
| Cubic-Low (Tetrahedral) |m3 | Yes |
| Cubic Cubic-High | m3m | Yes |

## Example Output Image ##

![Example Random Texture Pole Figure: Data Courtesey of [1]](images/PoleFigure_Random.png)


![Example Textured Pole Figure: Data courtesy of [2] ](images/PoleFigure_Textured.png)


## Notes  ##

The implementation for the Pole Figure is to use a modified Lambert projection algorithm. This type of algorithm is new and has NOT been proven or accepted by the greater materials community. We are including it here to solicit the community for comments.



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

## References ##

[1] Groeber M, Haley BK, , Uchic MD, Dimiduk DM, Ghosh S: 3D reconstruction and characterization of polycrystalline microstructures using a FIB–SEM system Data Sets. Materials Characterization 2006, 57:259—-273.

[2] Nathalie Allain-Bonasso, Francis Wagner, Stéphane Berbenni, David P. Field, A study of the heterogeneity of plastic deformation in IF steel by EBSD, Materials Science and Engineering: A, Volume 548, 30 June 2012, Pages 56-63, ISSN 0921-5093, http://dx.doi.org/10.1016/j.msea.2012.03.068.
(http://www.sciencedirect.com/science/article/pii/S0921509312004388)


See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)
