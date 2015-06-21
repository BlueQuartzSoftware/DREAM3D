Generate Ensemble Statistics {#generateensemblestatistics}
==========

## Group (Subgroup) ##
Statistics (Ensemble)

## Description ##
This filter will fit "common" statistical distributions (as chosen by the user) to specific **Feature** level attributes.  The user can choose which types of attributes to fit (morphological and/or crystallographic) and what distribution to use when fitting them.  The parameters that define the "best-fit" distributions will be stored in an **Ensemble** array.  The **Ensemble** array will be an array of *Statistics Objects* that are defined structures within *DREAM.3D*.  The *Statistics Objects* are used when building synthetic structures in DREAM.3D.  There are other filters for fitting a distribution to an individual array, but this filter is intended to be used to populate the *Statistics Objects* needed to build synthetic structures.  

Note: the choice of *Phase Type* for each **Ensemble** will change the arrays that are fit for that **Ensemble**.

## Parameters ##

| Name | Type | Description |
|------|------| ----------- |
| Phase Types | Choice | Informs the filter what "statistical type" each **Ensemble** is |
| Size Correlation Resolution | Float | Defines the width of *bins* used to discretize the size distribution when correlating other parameters to the size distribution |
| Include Radial Distribution Function | Boolean | Specifies whether to calculate the RDF |
| Calculate Morphological Stats | Boolean | Specifies whether to calculate the fits for each of the *morphological* statistics (size, shape, orientation, neighbors) |
| Calculate Crystallographic Stats | Boolean | Specifies whether to calculate the fits for each of the *crystallographic* statistics (orientation distribution function, misorientation distribution function) |
| Size Distribution Fit Type | Choice | Which distribution to fit the size distibution with (only necessary if *Calculate Morphological Stats* is *true*) |
| Aspect Ratio Distribution Fit Type | Choice | Which distribution to fit the aspect ratio distibution with (only necessary if *Calculate Morphological Stats* is *true*) |
| Omega3 Distribution Fit Type | Choice | Which distribution to fit the omega3 distibution with (only necessary if *Calculate Morphological Stats* is *true*) |
| Neighborhoods Distribution Fit Type | Choice | Which distribution to fit the neighborhood distibution with (only necessary if *Calculate Morphological Stats* is *true*) |

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | FeaturePhases | Int | (1) | Specifies the phase of the **Feature** - Values will begin at 1 as there is no phase 0, which is used temporarily in some filters for bad data|
| Feature | AvgQuats | Float | (4) | Defines the average orientation of the **Feature** in quaternion representation  (<x,y,z>, w) (Only required if *Calculate Crystallographic Stats* is *true*) |
| Feature | FeatureEulerAngles | Float | (3) | Defines the orientation of each **Feature** in Bunge convention (Z-X-Z) (Only required if *Calculate Crystallographic Stats* is *true*) |
| Feature | EquivalentDiameters | Float | (1) | Diameter of a sphere with the same volume as the **Feature**. (Only required if *Calculate Morphological Stats* is *true*)|
| Feature | Volumes |  Float | (1) | Volume (in units^3) of the **Feature**. (Only required if *Calculate Crystallographic Stats* is *true*) | 
| Feature | AspectRatios | Float | (2) | Ratio of axis lengths (b/a and c/a) for best-fit ellipsoid to **Feature** (Only required if *Calculate Morphological Stats* is *true*) |
| Feature | AxisEulerAngles | Float | (3) | Euler angles (in radians) necessary to rotate the sample reference frame to the reference frame of the **Feature**, where the prinicpal axes of the best-fit ellipsoid are (x,y,z). (Only required if *Calculate Morphological Stats* is *true*) |
| Feature | Omega3s | Float | (1) | 3rd invariant of the second-order moment matrix for the **Feature**, does not assume a shape type (ie ellipsoid) (Only required if *Calculate Morphological Stats* is *true*) |
| Feature | NeighborLists | List of Ints | (1) | List of the contiguous neighboring **Features** for a given **Feature** (Only required if *Calculate Morphological Stats* is *true*) |
| Feature | SharedSurfaceAreaLists | List of Floats | (1) | List of the shared surface area for each of the contiguous neighboring **Features** for a given **Feature** (Only required if *Calculate Crystallographic Stats* is *true*) |
| Feature | SurfaceFeatures | Boolean | (1) | Flag of 1 if **Feature** touches an outer surface or of 0 if it does not (Only required if *Calculate Crystallographic Stats* is *true*) |
| Feature | Neighborhoods | Int | (1) | Number of **Features** that have their centroid within the user specified multiple of equivalent sphere diameters from each **Feature** (Only required if *Calculate Morphological Stats* is *true*) |
| Feature | BiasedFeatures | Boolean | (1) | Flag of 1 if **Feature** is biased or of 0 if it is not (Only required if *Calculate Morphological Stats* is *true*) |
| Ensemble | CrystalStructures | Int | (1) | Specifies the crystal structure of each Ensemble using an enumeration defined by DREAM3D (Hexagonal_High=0, Cubic_High=1, Hexagonal_Low=2, Cubic_Low=3, Triclinic=4, Monoclinic=5, Orthorhombic=6, Tetragonal_Low=7, Tetragonal_High=8, Trigonal_Low=9, Trigonal_High=10, Unknown=999) (Only required if *Calculate Crystallographic Stats* is *true*) |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Ensemble | Statistics | Statistics Object | (1) | Statistics objects (depending on *Phase Type*) that store fits to descriptors like: size distribution, shape distribution, neighbor distribution, ODF, MDF, etc) |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


