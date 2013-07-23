Convert Euler Angles {#converteulerangles}
======

## Group (Subgroup) ##
Processing Filters (Conversion)

## Description ##
This Filter will multiply the Euler angles of every **Cell** by a factor to convert *degrees to radians* or *radians to degrees*.  The user needs to know the units of their data in order to use this filter properly. 2D data files that are in *HKL*'s .ctf format use degrees. 3D data files in *HKL*'s .ctf format use radians. All files in *TSL*'s .ang format use radians. If other file types are used, determine the units before running this filter.  

Generally, all filters within DREAM.3D use radians as a default unit, but filters may be added that require the Euler angles to be in degrees and this filter will allow the user to move back-and-forth between the units.

If the wrong conversion is selected (i.e. the user chooses *degrees to radians*, but their data is already in radians), the Filter will apply the conversion anyway and will result in erroneous data being used for the rest of the pipeline. If unsure about the units, check with the software supplier. 

## Parameters ##

| Name | Type |
|------|------|
| Conversion Type | Choices |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | CellEulerAngles | Three (3) angles (floats) defining the orientation of the **Cell** in Bunge convention (Z-X-Z) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Match Crystallography (SyntheticBuilding) |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

