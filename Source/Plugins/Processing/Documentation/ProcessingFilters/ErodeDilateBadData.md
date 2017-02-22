Erode/Dilate Bad Data {#erodedilatebaddata}
=============

## Group (Subgroup) ##
Processing (Cleanup)

## Description ##
Bad data refers to a **Cell** that has a _Feature Id_ of *0*, which means the **Cell** has failed some sort of test and been marked as a *bad* **Cell**. If the *bad* data is _dilated_, the **Filter** grows the *bad* data by one **Cell** in an iterative sequence for a user defined number of iterations.  During the *dilate* process the _Feature Id_ of any **Cell** neighboring a *bad* **Cell** will be changed to *0*. If the *bad* data is _eroded_, the **Filter** shrinks the bad data by one **Cell** in an iterative sequence for a user defined number of iterations.  During the *erode* process the _Feature Id_ of the *bad* **Cell** is changed from *0* to the _Feature Id_ of the majority of its neighbors. If there is a tie between two _Feature Ids_, then one of the *Feature Ids*, chosen randomly, will be assigned to the *bad* **Cell**. If _Replace Bad Data_ is selected, all **Attribute Arrays** will be replaced with their neighbor's value during erosion/dilation (instead of only _Feature Id_). The **Filter** also offers the option(s) to turn on/off the erosion or dilation in specific directions (X, Y or Z).

Goals a user might be trying to accomplish with this **Filter** include:

- Remove small or thin regions of bad data by running a single (or two) iteration _erode_ operation. 
- Increase the size of a *bad* data region by running an _dilate_ operation. This might be useful if the experimental technique tends to underestimates the size of certain objects. For example, when running EBSD, the pores (which show up as *bad* data) are generally smaller in the scans than in the specimen, because the beam, when it is just inside the pore, still can pick up signal from the material just beneath the pore.  

Running the _erode-dilate_ operations in pairs can often change the size of some objects without affecting others. For example, if there were a number of big pores and a number of single *bad* **Cells**, running a single _erode_ operation would remove the single **Cells** and reduce the pores by one **Cell**. If this is followed immediately by  a _dilate_ operation, then the pores would grow by one **Cell** and return to near their original size, while the single **Cells** would remain removed and not "grow back".

## Parameters ##

| Name | Type | Description |
|------|------|-------------|
| Operation | Enumeration | Whether to dilate or erode |
| Number of Iterations | int32_t | The number of iterations to use for erosion/dilation |
| X Direction | bool | Whether to erode/dilate in the X direction |
| Y Direction | bool | Whether to erode/dilate in the Y direction |
| Z Direction | bool | Whether to erode/dilate in the Z direction |
| Replace Bad Data | bool | Whether to replace all data or just _Feature Ids_ |

## Required Geometry ##
Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


