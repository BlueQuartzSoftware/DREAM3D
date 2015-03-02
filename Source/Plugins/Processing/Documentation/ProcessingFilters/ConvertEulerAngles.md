Convert Euler Angles {#converteulerangles}
======

## Group (Subgroup) ##
Processing Filters (Conversion)

## Description ##
This Filter will multiply the Euler angles of every **Element** by a factor to convert *degrees to radians* or *radians to degrees*.  The user needs to know the units of their data in order to use this filter properly. 2D data files that are in *HKL*'s .ctf format use degrees. 3D data files in *HKL*'s .ctf format use radians. All files in *TSL*'s .ang format use radians. If other file types are used, determine the units before running this filter.  
Generally, all filters within DREAM.3D use radians as a default unit, but filters may be added that require the Euler angles to be in degrees and this filter will allow the user to move back-and-forth between the units.
If the wrong conversion is selected (i.e. the user chooses *degrees to radians*, but their data is already in radians), the Filter will apply the conversion anyway and will result in erroneous data being used for the rest of the pipeline. If unsure about the units, check with the software supplier. 

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Conversion Type | Selection | Tells filter which conversion is being made |

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Element | CellEulerAngles | Float | (1,3) | Euler angles defining the orientation of the **Element** in Bunge convention (Z-X-Z) |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

