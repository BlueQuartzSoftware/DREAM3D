Convert Orientation Representations {#convertorientations}
=======

## Group (Subgroup) ##
Orientation Analysis (Conversion)

## Description ##

This **Filter** generates a new orientation representation (see Data Layout Table below) for each **Element**, given the _Input Orientation Representation_ for the **Element**. The following table lists the various orientation representations that are supported. DREAM3D is capable of converting between any representation with some caveats.

#### Data Layout ####

| Orientation Representation | No. of Components | Representation Type | Data Layout |
|----------------------------|-------------- ----|---------------------|-------------|
| EulerAngles                | 3 | 0 | phi1, Phi, phi2 |
| Orientation Matrix         | 9 | 1 | Row Major Format |
| Quaternions                | 4 | 2 | ( \[x, y, z\], w ) |
| Axis Angle                 | 4 | 3 | ( \[x, y, z\], Angle) |
| Rodrigues Vectors          | 4 | 4 | *Internally DREAM.3D keeps a 4th component* |
| Homochoric                 | 3 | 5 |  |
| Cubochoric                 | 3 | 5 |  |



#### Data Range ####

The valid range for Euler angles is (Degrees):

    + phi-1: 0 to 360
    + Phi : 0 to 180
    + phi-2: 0 to 360

#### Data Conversion Notes ####

If the angles fall outside of this range the **original** Euler Input data **WILL BE CHANGED** to ensure they are within this range.

## Precision Notes ##

While every effort has been made to ensure the correctness of each transformation algorithm, certain situations may arise where the initial precision of the input data is not large enough for the algorithm to calculate an answer that is intuitive. The user should be acutely aware of their input data and if their data may cause these situations to occur. Combinations of Euler angles close to 0, 180 and 360 can cause these issues to be hit. For instance an Euler angle of [180, 56, 360] is symmetrically the same as [180, 56, 0] and due to calculation errors and round off errors converting that Euler angle between representations may not give the numerical answer the user was anticipating but will give a symmetrically equivalent angle.

## Parameters ##

| Name             | Type | Description |
|------------------|------|-------------|
| Input Orientation Type | Enumeration | Specifies the incoming orientation representation |
| Output Orientation Type | Enumeration | Specifies to which orientation representation to convert the incoming data  |

## Required Geometry ##

Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Any **Attribute Array** | None    | float | See above table | Incoming orientation representation |


## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-------------------|
| Any **Attribute Array**     | None            | float | See above table | Converted orientation representation          |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)

