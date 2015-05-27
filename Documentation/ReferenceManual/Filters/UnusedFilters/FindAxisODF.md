Find Feature Axis ODF {#findaxisodf}
======


## Group (Subgroup) ##

Statistics Filters (Morphological)



## Description ##
The ODF is defined as the volume fraction of **Features** with a certain orientation vector.
The orientation vector is normally identified using three _Euler_ angles which are used to describe the transition from the _sample_’s reference frame into the _crystallographic_ reference frame of each individual **Feature** of the polycrystal. One thus ends up with a large set of different _Euler_ angles, the distribution of which is described by the ODF.
This Filter searches and identifies the ODF axis of the volume fraction of **Features**.


## Parameters ##

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Feature | AxisEulerAngles |  |
| Feature | FeaturePhases |  |
| Feature | SurfaceFeatures |  |

## Created Arrays ##
None

## Authors ##


**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


