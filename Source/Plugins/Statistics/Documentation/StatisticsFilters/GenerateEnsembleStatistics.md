Generate Ensemble Statistics {#generateensemblestatistics}
==========

## Group (Subgroup) ##
Statistics (Ensemble)

## Description ##
This **Filter** will fit "common" statistical distributions (as chosen by the user) to specific **Feature** level attributes.  The user can choose which types of attributes to fit (morphological and/or crystallographic) and what distribution to use when fitting them.  The parameters that define the "best-fit" distributions will be stored in an **Ensemble** array.  The **Ensemble** array will be an array of *Statistics Objects* that are defined structures within DREAM.3D.  The *Statistics Objects* are used when building synthetic structures in DREAM.3D.  There are other **Filters** for fitting a distribution to an individual array, but this **Filter** is intended to be used to populate the *Statistics Objects* needed to build synthetic structures. 

*Note:* The user must select a *Phase Type* for each **Ensemble** for which *Statistics Objects* are desired.  This selection occurs in the *Phase Types* selection, where the user must select the **Ensemble Attribute Matrix** that will be used to define the desired target **Ensembles**. The choice of *Phase Type* for each **Ensemble** will change the arrays that are fit for that **Ensemble**.

For more information on using this **Filter** to feed into synthetic building, visit the [tutorial](@ref tutorialsyntheticsingle).

## Parameters ##

| Name | Type | Description |
|------|------| ----------- |
| Phase Types | Enumeration | Specifies the phase type for each **Ensemble** in a chosen **Ensemble Attribute Matrix** |
| Size Correlation Resolution | float | Defines the width of *bins* used to discretize the size distribution when correlating other parameters to the size distribution |
| Calculate Morphological Stats | bool | Whether to calculate the fits for each of the *morphological* statistics (size, shape, orientation, neighbors) |
| Size Distribution Fit Type | Enumeration | Which distribution to fit the size distibution with. Only needed if *Calculate Morphological Stats* is checked |
| Aspect Ratio Distribution Fit Type | Enumeration | Which distribution to fit the aspect ratio distibution with. Only needed if *Calculate Morphological Stats* is checked |
| Omega3 Distribution Fit Type | Enumeration | Which distribution to fit the omega3 distibution with. Only needed if *Calculate Morphological Stats* is checked |
| Neighborhoods Distribution Fit Type | Enumeration | Which distribution to fit the neighborhood distibution with. Only needed if *Calculate Morphological Stats* is *checked |
| Calculate Crystallographic Stats | bool | Specifies whether to calculate the fits for each of the *crystallographic* statistics (orientation distribution function, misorientation distribution function) |

## Required Geometry ##
Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Feature Attribute Array** | Phases | int32_t | (1) | Specifies to which **Ensemble** each **Feature** belongs |
| **Feature Attribute Array** | NeighborList | List of int32_t | (1) | List of the contiguous neighboring **Features** for a given **Feature**|
| **Feature Attribute Array** | BiasedFeatures | bool | (1) | Flag of 1 if **Feature** is biased or of 0 if it is not. Only required if *Calculate Morphological Stats* is checked |
| **Feature Attribute Array** | EquivalentDiameters | float | (1) | Diameter of a sphere with the same volume as the **Feature**. Only required  if *Calculate Morphological Stats* is checked|
| **Feature Attribute Array** | AspectRatios | float | (2) | Ratio of axis lengths (b/a and c/a) for best-fit ellipsoid to **Feature**. Only required if *Calculate Morphological Stats* is checked |
| **Feature Attribute Array** | Omega3s | float | (1) | 3rd invariant of the second-order moment matrix for the **Feature**, does not assume a shape type (i.e., ellipsoid). Only required  if *Calculate Morphological Stats* is checked |
| **Feature Attribute Array** | Neighborhoods | int32_t | (1) | Number of **Features** that have their centroid within the user specified multiple of equivalent sphere diameters from each **Feature**. Only required  if *Calculate Morphological Stats* is checked |
| **Feature Attribute Array** | AxisEulerAngles | float | (3) | Euler angles (in radians) necessary to rotate the sample reference frame to the reference frame of the **Feature**, where the prinicpal axes of the best-fit ellipsoid are (x,y,z). Only required if *Calculate Morphological Stats* is checked |
| **Feature Attribute Array** | SurfaceFeatures | bool | (1) | Flag of 1 if **Feature** touches an outer surface or of 0 if it does not. Only required if *Calculate Crystallographic Stats* is checked |
| **Feature Attribute Array** | Volumes |  float | (1) | Volume (in units^3) of the **Feature**. Only required if *Calculate Crystallographic Stats* is checked | 
| **Feature Attribute Array** | AvgEulerAngles | float | (3) | Defines the orientation of each **Feature** in Bunge convention (Z-X-Z). Only required if *Calculate Crystallographic Stats* is checked |
| **Feature Attribute Array** | AvgQuats | float | (4) | Defines the average orientation of the **Feature** in quaternion representation. Only required if *Calculate Crystallographic Stats* is checked|
| **Feature Attribute Array** | SharedSurfaceAreaLists | List of float | (1) | List of the shared surface area for each of the contiguous neighboring **Features** for a given **Feature**. Only required if *Calculate Crystallographic Stats* is checked |
| **Ensemble Attribute Array** | CrystalStructures | uint32_t | (1) | Enumeration representing the crystal structure for each **Ensemble** |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Ensemble Attribute Array** | PhaseTypes | uint32_t | (1) | Enumeration specifying the phase type of each **Ensemble** |
| **Ensemble Attribute Array** | Statistics | Statistics Object | (1) | Statistics objects (depending on *phase type*) that store fits to descriptors like: size distribution, shape distribution, neighbor distribution, ODF, MDF, etc) |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


