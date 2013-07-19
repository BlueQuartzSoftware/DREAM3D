Find Field Axis ODF {#findaxisodf}
======


## Group (Subgroup) ##

Statistics Filters (Morphological)



## Description ##
The ODF is defined as the volume fraction of **Fields** with a certain orientation vector.
The orientation vector is normally identified using three _Euler_ angles which are used to describe the transition from the _sample_’s reference frame into the _crystallographic_ reference frame of each individual **Field** of the polycrystal. One thus ends up with a large set of different _Euler_ angles, the distribution of which is described by the ODF.
This Filter searches and identifies the ODF axis of the volume fraction of **Fields**.


## Parameters ##

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Field | AxisEulerAngles |  |
| Field | FieldPhases |  |
| Field | SurfaceFields |  |

## Created Arrays ##
None

## Authors ##


**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

