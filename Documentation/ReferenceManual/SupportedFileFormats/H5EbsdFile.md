H5EBSD Data File Specification {#h5ebsdfile}
================

DREAM.3D can store EBSD data from multiple vendors in a data file using the [HDF5](http://www.hdfgroup.org) file format library. Although the general layout of the HDF5 file is the same between vendors, there are details that are _not_ the same between vendors, because each vendor chooses to save different types of data. The top level datasets that deal with the basic volume information is the same for every file.



## HDF5 File Layout ##


### HDF5 Root Level Layout Specification ###

| Root Level Attributes | Type | Name/Comments/Default Value  |
|----------|---------------|-----------|
| FileVersion |  H5T\_NATIVE\_INT32   | Current Value="5"  |
| EsdLibVersion | H5T\_STRING | Optional value that states what version of the library wrote the file |


| Root Level Data sets | HDF5 Type | Name/Comments/Default Value |
|----------|---------------|-----------|
| Index | An array of all the slice values. As of DREAM3D Version 5 this is not really used but could handy if the list of slices starts to get large.  |  |
| EulerTransformationAngle | H5T\_NATIVE\_FLOAT | Degrees |
| EulerTransformationAxis | H5T\_NATIVE\_FLOAT |  3x1 Array (Example 0,0,1)|
| Manufacturer | H5T\_STRING | The **Manufacturer** currently is either _TSL_ or _HKL_ |
| Max X Points | H5T\_NATIVE\_INT64 | The maximum number of X points in the sample grid |
| Max Y Points | H5T\_NATIVE\_INT64 | The maximum number of Y points in the sample grid |
| SampleTransformationAngle | H5T\_NATIVE\_FLOAT | Degrees |
| SampleTransformationAxis | H5T\_NATIVE\_FLOAT |  3x1 Array (Example 0,0,1) |
| Stacking Order | H5T\_NATIVE\_UINT32 | Defines which slice corresponds to the Z=0 in the coordinate system. 0=Low to High. 1 = High to Low. Optional Attribute of type H5T\_STRING, with Name "Name" and Value ("Low To High" or "High To Low") |
| X Resolution | H5T\_NATIVE\_FLOAT | Resolution between sample points in the X direction |
| Y Resolution | H5T\_NATIVE\_FLOAT | Resolution between sample points in the Y direction |
| Z Resolution | H5T\_NATIVE\_FLOAT | Resolution between slices in the Z Direction |
| ZEndIndex | H5T\_NATIVE\_INT64 | Starting Slice index |
| ZStartIndex | H5T\_NATIVE\_INT64 | Ending Slice index (inclusive) |
| Index | An array of all the slice values. As of DREAM3D Version 5 this is not really used but could handy if the list of slices starts to get large.  |  |

### Slice Group Specification ###
Each Slice is grouped into its own H5G_GROUP with the Name of the group as the index of the slice. Within each slice group there are two (2) more groups with names **Data** and **Header**

| **Name** | **HDF5 Type** | **Value** |
|----------|---------------|-----------|
| Data   | H5G\_GROUP | Contains all the data columns |
| Header | H5G\_GROUP | Contains all the header entries |



## TSL Specification ##
 This section details the data to be imported from a .ang file into the .h5ebsd file.
 
### TSL (.ang) Data Group Specification ###

| **Name** | **HDF5 Type** | **Value** |
|----------|---------------|-----------|
| Phi1 | H5T\_NATIVE\_FLOAT | Contains all the Phi1 data in a 1D Array with length equal to the total number of points. |
| Phi | H5T\_NATIVE\_FLOAT | Contains all the Phi data in a 1D Array with length equal to the total number of points. |
| Phi2 | H5T\_NATIVE\_FLOAT | Contains all the Phi2 data in a 1D Array with length equal to the total number of points. |
| X Position | H5T\_NATIVE\_FLOAT | Contains all the X position data in a 1D Array with length equal to the total number of points. |
| Y Position | H5T\_NATIVE\_FLOAT | Contains all the Y position data in a 1D Array with length equal to the total number of points. |
| Image Quality | H5T\_NATIVE\_FLOAT | Contains all the Image Quality data in a 1D Array with length equal to the total number of points. |
| Confidence Index | H5T\_NATIVE\_FLOAT | Contains all the Confidence Index data in a 1D Array with length equal to the total number of points. |
| PhaseData | H5T\_NATIVE\_INT32 | Contains all the PhaseData data in a 1D Array with length equal to the total number of points. |
| SEM Signal | H5T\_NATIVE\_FLOAT | Contains all the SEM Signal data in a 1D Array with length equal to the total number of points. |
| Fit | H5T\_NATIVE\_FLOAT | Contains all the Fit of Solution data in a 1D Array with length equal to the total number of points. |


### TSL (.ang) Header Group Specification ###


| **Name** | **HDF5 Type** | **Value** |
|----------|---------------|-----------|
| OriginalFile | H5T\_STRING | Path to the .ang file that was imported |
| OriginalHeader | H5T\_STRING | Contains the original header from the imported .ctf file |
| TEM_PIXperUM | H5T\_NATIVE\_FLOAT | Contains value for the header entry TEM_PIXperUM |
| x-star | H5T\_NATIVE\_FLOAT | Contains value for the header entry x-star |
| y-star | H5T\_NATIVE\_FLOAT | Contains value for the header entry y-star |
| z-star | H5T\_NATIVE\_FLOAT | Contains value for the header entry z-star |
| WorkingDistance | H5T\_NATIVE\_FLOAT | Contains value for the header entry WorkingDistance |
| ElasticConstants | H5T\_STRING | Contains value for the header entry ElasticConstants |
| GRID | H5T\_STRING | Contains value for the header entry GRID |
| XSTEP | H5T\_NATIVE\_FLOAT | Contains value for the header entry XSTEP |
| YSTEP | H5T\_NATIVE\_FLOAT | Contains value for the header entry YSTEP |
| NCOLS_ODD | H5T\_NATIVE\_INT32 | Contains value for the header entry NCOLS_ODD |
| NCOLS_EVEN | H5T\_NATIVE\_INT32 | Contains value for the header entry NCOLS_EVEN |
| NROWS | H5T\_NATIVE\_INT32 | Contains value for the header entry NROWS |
| OPERATOR | H5T\_STRING | Contains value for the header entry OPERATOR |
| SAMPLEID | H5T\_STRING | Contains value for the header entry SAMPLEID |
| SCANID | H5T\_STRING | Contains value for the header entry SCANID |
| Phases | H5G_GROUP | Group that contains a subgroup for each phase where the name of each subgroup is the index of the phase starting at 1. |


### TSL (.ang) Phase Group Specification ###

| **Name** | **HDF5 Type** | **Value** |
|----------|---------------|-----------|
| Categories | H5T\_NATIVE\_INT32 |  |
| Formula | H5T\_STRING |  |
| Info | H5T\_STRING |  |
| LatticeConstants | H5T\_NATIVE\_FLOAT |  |
| Material Name | H5T\_STRING |  |
| NumberFamilies | H5T\_NATIVE\_INT32 |  |
| Phase | H5T\_NATIVE\_INT32 |  |
| Symmetry | H5T\_NATIVE\_INT32 |  |
| hklFamilies | H5G_GROUP | Contains all the HKL Family information where the number of datasets contained in this group is the number of HKL Families |


### TSL (.ang) HKLFamily Group Specification ###

| **Name** | **HDF5 Type** | **Value** |
|----------|---------------|-----------|
| {Based on Index of the family}. If there are 4 families then there are 4 data sets with names "0", "1", "2" and "3". | Custom: See Below | |
 
    typedef struct
    {
        int h;
        int k;
        int l;
        int s1;
        float diffractionIntensity;
        int s2;
    } HKLFamily_t;

---------------

## HKL Specification  ##
 This section details the data to be imported from a .ctf file into the .h5ebsd file

### HKL (.ctf) Data Group Specification ###
1D Array of Values, where the number of elements in the array is equal to the total number of points per 2D Slice.

| **Name** | **HDF5 Type** | **Value** |
|----------|---------------|-----------|
| Phase | H5T\_NATIVE\_INT32 |  |
| X | H5T\_NATIVE\_FLOAT |  |
| Y | H5T\_NATIVE\_FLOAT |  |
| Z | H5T\_NATIVE\_FLOAT | Note that this ONLY appears in a "3D" .ctf data file |
| Bands | H5T\_NATIVE\_INT32 |  |
| Error | H5T\_NATIVE\_INT32 |  |
| Euler1 | H5T\_NATIVE\_FLOAT | In 2D files these are in Degrees. In 3D files these are in Radians |
| Euler2 | H5T\_NATIVE\_FLOAT | In 2D files these are in Degrees. In 3D files these are in Radians |
| Euler3 | H5T\_NATIVE\_FLOAT | In 2D files these are in Degrees. In 3D files these are in Radians |
| MAD | H5T\_NATIVE\_FLOAT |  |
| BD | H5T\_NATIVE\_INT32 |  |
| BS | H5T\_NATIVE\_INT32 |  |
| GrainIndex | H5T\_NATIVE\_INT32 |  |
| GrainRandomColourR | H5T\_NATIVE\_UINT8 |  |
| GrainRandomColourG | H5T\_NATIVE\_UINT8 |  |
| GrainRandomColourB | H5T\_NATIVE\_UINT8 |  |


### HKL (.ctf) Header Specification ###

| **Name** | **HDF5 Type** | **Value** |
|----------|---------------|-----------|
| OriginalFile | H5T\_STRING | Path to the .ctf file that was imported |
| OriginalHeader | H5T\_STRING | Contains the original header from the imported .ctf file |
| Prj | H5T\_STRING | Contains value for the header entry Prj |
| Author | H5T\_STRING | Contains value for the header entry Author |
| JobMode | H5T\_STRING | Contains value for the header entry JobMode |
| XCells | H5T\_NATIVE\_INT32 | Contains value for the header entry XCells |
| YCells | H5T\_NATIVE\_INT32 | Contains value for the header entry YCells |
| XStep | H5T\_NATIVE\_FLOAT | Contains value for the header entry XStep |
| YStep | H5T\_NATIVE\_FLOAT | Contains value for the header entry YStep |
| ZStep | H5T\_NATIVE\_FLOAT | Contains value for the header entry ZStep |
| ZCells | H5T\_NATIVE\_FLOAT | Contains value for the header entry ZCells |
| AcqE1 | H5T\_NATIVE\_FLOAT | Contains value for the header entry AcqE1 |
| AcqE2 | H5T\_NATIVE\_FLOAT | Contains value for the header entry AcqE2 |
| AcqE3 | H5T\_NATIVE\_FLOAT | Contains value for the header entry AcqE3 |
| Mag | H5T\_NATIVE\_INT32 | Contains value for the header entry Mag |
| Coverage | H5T\_NATIVE\_INT32 | Contains value for the header entry Coverage |
| Device | H5T\_NATIVE\_INT32 | Contains value for the header entry Device |
| KV | H5T\_NATIVE\_INT32 | Contains value for the header entry KV |
| TiltAngle | H5T\_NATIVE\_FLOAT | Contains value for the header entry TiltAngle |
| TiltAxis | H5T\_NATIVE\_FLOAT | Contains value for the header entry TiltAxis |
| Phases | H5G_GROUP | Group that contains a subgroup for each phase where the name of each subgroup is the index of the phase starting at **1.** |


### HKL (.ctf) Phase Group Specification ###

| **Name** | **HDF5 Type** | **Value** |
|----------|---------------|-----------|
| Comment | H5T\_STRING | Contains value for the header entry Comment |
| Internal1 | H5T\_STRING | Contains value for the header entry Internal1 |
| Internal2 | H5T\_STRING | Contains value for the header entry Internal2 |
| LatticeAngles | H5T\_NATIVE\_FLOAT | Contains value for the header entry Lattice Angles in a 1x3 array |
| LatticeDimensions | H5T\_NATIVE\_FLOAT | Contains value for the header entry Lattice Dimensions in a 1x3 array |
| LaueGroup | H5T\_NATIVE\_INT32 | Contains value for the header entry LaueGroup plus an H5T\_STRING Attribute which is the string name of the Laue Group, for example "Hexagonal-High 6/mmm" |
| SpaceGroup | H5T\_NATIVE\_INT32 | Contains value for the header entry SpaceGroup |
| PhaseName | H5T\_STRING | Contains value for the header entry PhaseName |



## Stacking Order Discussion ##

The _stacking order_ refers to the order in which the z slices are stacked together when they are read from the file. The enumerations are also in the EbsdLibConstants.h header file.

As a further explanation, if the ordering is **Low To High**, then the slice with the lowest number is positioned at Z=0 in 3D Cartesian space. For example, if your data set is numbered from 23 to 86 with file names of the form Slice\_023.ang and you select "Low To High", then the data inside of file Slice\_023.ang will be positioned at Z=0 during any method that has to deal with the data. The opposite of this is if the user were to select to have their data **High to Low**, in which case, the file with name Slice\_086.ang will be positioned at Z=0 and the file with name "Slice_023.ang" will be positioned at Z=64.

 
