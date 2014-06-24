Read Ph File (Feature Ids) {#phreader}
======

## Group (Subgroup) ##
I/O Filters


## Description ##
This Filter reads a custom data file used by Carnegie-Mellon Univ. The file format is a simple ASCII Text file with the following contents.

**Format**

    Line 1:
    {5 Spaces}{x dimension}{5 Spaces}{y dimension}{5 Spaces}{z dimension}
    Line 2:
    'DREAM3'              52.00  1.000  1.0       {Number of **Features**}
    Line 3: (Without the quotes)
    " 0.000 0.000 0.000          0        "
    {Each **Feature** Id on a single line}


**Example File (Partial)**

    11     11     11
    'DREAM3'              52.00  1.000  1.0       6
    0.000 0.000 0.000          0        
    830
    830
    0
    977
    977
    830
    830
    .....
    

## Parameters ##

| Name | Type |
|------|------|
| Input File | File Path |
| Resolution | X, Y, Z Resolution of the data |
| Origin     | The location in space coordinates of the (0,0,0) voxel |

## Required DataContainers ##
Voxel

## Required Arrays ##
None

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |



## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

