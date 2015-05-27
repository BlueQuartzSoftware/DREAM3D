Convert Orientation Representations {#convertorientations}
=======

## Group (Subgroup) ##
Orientation Analysis Filters (Conversion)

## Description ##

This filter generates a new orientation representation for each **Element**, given the _Input Orientation Representation_ for the **Element**. The following table lists the various orientation representations that are supported. DREAM3D is capable of converting between any representation with some caveats.

The valid range for Euler angles is:
    + phi-1: 0 to 360
    + Phi : 0 to 180
    + phi-2: 0 to 360

If the angles fall outside of this range the **original** Euler Input data **WILL BE CHANGED** to ensure they are within this range.


| Orientation Representation | No. of Components | Orientation Type Index |
|----------------------------|------------------|-------------------------|
| EulerAngles               | 3      | 0 |
| OrientationMatrix | 9 | 1 |
| Quaternions                | 4            | 2 |
| AxisAngle           | 4       | 3 |
| Rodrigues Vectors          | 4 | 4 |
| Homochoric                 | 3 | 5 |


## Parameters ##

| Name             | Type | Description |
|------------------|------|-------------|
| Input Orientation Representation | Int | Use the proper index from the table above. The GUI will do this for you. |
| Output Orientation Representation | Int | Use the proper index from the table above. The GUI will do this for you. |
| Input DataArray Path | DataArray Path | The path to the Input Orientation Representation Data |
| Output Array Name | String | The name of the output array. The array will be created in the same AttributeMatrix as the input array |

## Required Geometry ##

Not Applicable

## Required Arrays ##

| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Element | Varies    | Float | see table above. |  |


## Created Arrays ##

| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-------------------|
| Element     | Varies            | Float | Varies |           |


## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users

