Convert Euler Angles {#converteulerangles}
======

## Group ##
Processing Filters

## Description ##
To use this Filter correctly, the user needs to know the angle unit used in the data file. Files for 2D data that are .ctf format use degrees. Files for 3D data that are .ctf format use radians. All .ang files use radians. If other file types are used, determine the angle unit before running this filter.
There are several possible situations that can arise when choosing the angle conversion:
_Data is in degrees: if radians-to-degrees is selected the angles are incorrect._
__Data is in degrees: if degrees-to-radians is selected the angles are correct.__
_Data is in radians: If degrees-to-radians is selected, the angles are incorrect._
__Data is in radians: if radians-to-degrees is selected the angles are correct.__

If the wrong unit is input, the Filter has no way to determine whether or not it is correct, because there is no way to know where the data came from and what the angle format may be.
If unsure about the unit used, check with the software supplier. 
Additionally, there may be user Filters added that use degrees and not radians. This Filter handles those situations. BUT, again, the user must know what the angle unit is before applying this Filter.

## Parameters ## 

| Name | Type |
|------|------|
| Conversion Type | Choices |

## Required Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Cell | CellEulerAngles | Three (3) angles (floats) defining the orientation of the voxel/datapoint in Bunge convention (Z-X-Z) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |

## Created Arrays ##
None

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



