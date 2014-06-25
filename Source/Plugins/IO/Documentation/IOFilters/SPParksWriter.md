Write SPParks File (Feature Ids) {#spparkswriter}
============

## Group (Subgroup) ##

I/O Filters


## Description ##

This Filter writes to a data file in a format used by [SPPARKS Kinetic Monte Carlo Simulator](http://spparks.sandia.gov/).

+ The "Values" section is a pair of numbers where the first number is the site ID and the second
 number is the actual value of the **Feature** Id at that site.
+ LINE 4 evidently must be what is shown in the example file.
+ LINE 3 is the total number of **Cells**.
+ LINES 5,6 and 7 are the Dimensions of the volume. 


 More information can be found at the [SPParks web site.](http://spparks.sandia.gov/doc/read_sites.html "SPParks Web Site")

**Start Example Partial File**
~~~~~~~~~~~~~~~~~~~~~~~~~
    [LINE 1] -
    [LINE 2] 3 dimension
    [LINE 3] 8000000 sites
    [LINE 4] 26 max neighbors
    [LINE 5] 0 200 xlo xhi
    [LINE 6] 0 200 ylo yhi
    [LINE 7] 0 200 zlo zhi
    [LINE 8]
    [LINE 9] Values
    [LINE 10]
    1 944
    2 944
    3 944
    4 944
    5 509
    6 509
    7 509
    .....
~~~~~~~~~~~~~~~~~~~~~~~~~

## Parameters ##

| Name | Type |
|------|------|
| Output File | Output File |

## Required DataContainers ##

Voxel DataContainer

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |

## Created Arrays ##

None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.


See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

