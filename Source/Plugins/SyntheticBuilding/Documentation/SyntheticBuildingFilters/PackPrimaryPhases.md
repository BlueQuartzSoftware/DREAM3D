# Pack Primary Phases #

## Group (Subgroup) ##

Synthetic Building (Packing)

## Description ##

If there are primary *Phases Types* in the volume, then this **Filter** will place primary **Features** with the sizes, shapes, physical orientations and locations corresponding to the goal statistics.   All primary phase types will placed at once.

First, the **Filter** will determine the available volume for placing primary **Features**.  This is accomplished by querying the *Feature Ids* array for the number of **Cells** not currently assigned to a valid **Feature** (*Feature Id* > 0).  Then, the available volume is divided amongst the primary phase types according to their relative volume fractions.  The size distribution of each primary phase type is sampled until the necessary volume of **Features** is generated.  After each primary phase type has a list of **Feature** sizes from sampling the size distribution, the shapes, number of neighoring **Features** and physical orientations are sampled from distributions that are correlated to the size distribution for that primary phase type.  At this point, the **Features** are fixed in their definition and are placed randomly in the volume.  Once all **Features**, from all primary phase types, are placed, the packing is assessed on two criteria: 1. How well do the **Features** fill space (i.e .minimal overlaps and gaps) and 2. How well do the neighborhoods of **Features** match the neighbor statistics distributions.  For a fixed number of iterations (100 \* number of **Features**), the **Features** are moved and swapped while trying to optimize against the two criteria mentioned previously.  If a move or swap improves the packing, it is accepted and if it does not it is rejected.  During this process, the **Features** are not actually placed and are not filling space, but rather being represented analytically.  Once the itrative process is finished, the **Features** are locked at their current location and they begin to *grow* from their centroid location according to their size, shape and orientation.  The growth rates are defined such that the **Features** grow as the *Shape Type* they are (i.e. ellipsoid, superellipsoid, cube-octaheron, cylinder, etc), in the orientation they were placed and at a speed relative to their size.  This growth continues until **Features** impinge and until all available **Cells** from the initial check are consumed.

The user can specify if they want *periodic boundary conditions*.  If they choose *periodic boundary conditions*, when the **Features** are being placed and when they are growing, if a **Feature** attempts to extend past the boundary of the volume, it wraps to the opposing face and is placed on the opposite side of the volume.

The user can also specify if they want to write out the goal attributes of the generated **Features**.  The **Features**, once packed, will not necessarily have the exact statistics (size, shape, orientation, number of neighbors) as sampled from the distributions.  This is due to the use of non-space-filling objects in the packing process.  The overlaps and gaps that occur after packing, must be assigned and will cause the **Features** to deviate from the intended goal (albeit hopefully in a minor way).  Writing out the goal attributes allows the user to then calculate the actual attributes and compare to determine how well the packing algorithm is working for their **Features**.

The user can specify if they want to use a *mask* when building the volume.  If the user chooses to use a *mask*, the they will have specify a boolean array that defines the volume that **Features** can be placed in (*=true*) and the volume that **Features** cannot grown past (*=false*).

The user can also choose to read in a list of **Features** with their locations and size and shape descriptions already determined.  If this option is choosen, the **Filter** will skip the steps of generating the **Features** and iteratively placing them and will begin *growing* the **Features** defined in list.  The format of the *Feature Input File* is:

    Number of Features
    Phase X Y Z A B C O3 Phi1 PHI Phi2
    Phase X Y Z A B C O3 Phi1 PHI Phi2
    Phase X Y Z A B C O3 Phi1 PHI Phi2
    .
    .
    .
    Phase X Y Z A B C O3 Phi1 PHI Phi2

+ The file is a _SPACE_ delimited file
+ The first line of the file is an integer that is the number of features in the file.
+ All other lines contain 11 columns of data. *ALL* columns are required.

| Column | Type | Description |
| ------ | ---- | ----------- |
|  X     | Float | X Coordinate |
|  Y     | Float | Y Coordinate |
|  Z     | Float | Z Coordinate |
|  A     | Float | Major Pricipal Semi Axis Length |
|  B     | Float | Mid Pricipal Semi Axis Length |
|  C     | Float | Minor Pricipal Semi Axis Length |
| O3 | Float | Omega 3 Value of the feature |
| phi1 | Float | First Euler Angle |
| PHI  | Float | Second Euler Angle |
| phi2 | Float | Third Euler Angle |

where (X,Y,Z) are the coordinates of the Feature's centroid, (A,B,C) are the major, mid and minor principal semiaxis lengths of the Feature, O3 is the Omega 3 value of the Feature and (Phi1,PHI,Phi2) are the Euler angles that describe the Features's crystal orientation in the sample reference frame. Note that the coordinates (for the centroid) must be given in the same units as was specified in Initialize Synthetic Volume.  For example, if you choose a volume 128x128x128 then the coordinates should correspond to that domain size in order to fill the volume as you expect.  Users are recommended to check the minimum and maximum values of their feature coordinates in order to make sure that the generated volume contains all the features as expected.

For more information on synthetic building, visit the [tutorial](@ref tutorialsyntheticsingle).

### How to write out the Goal Attributes ##

In previous versions of DREAM.3D for this filter there was an option to "Write Goal Attributes". This has been replaced with the feature that allows the user to save the Shape Parameters to another Attribute Matrix. To regain this functionality the user can enable this option from a drop down combo box in the Filter Parameter input widget, then add the "Export Feature Data as CSV File" later in the pipeline. In that added filter, the user can select the Feature Attribute Matrix where they saved the Shape Parameters.

## Parameters ##

| Name | Type | Description |
|------|------| ----------- |
| Periodic Boundaries | bool | Whether to *wrap* **Features** to create *periodic boundary conditions* |
| Use Mask | Boolean | Whether there is an array that defines where the **Features** can be placed and where they cannot *grow* past |
| Feature Generation | Int | Whether the user already has the final location and the size and shape definition of the **Features** and can skip the **Feature** generation and iterative placement process. 0=Generate Features, 1=Skip Generation |
| Feature Input File | File Path | Path to the file that contains the description and location of the **Features** the user wishes to use (only necessary if **Feature Generation = 1**) |
| Save Shape Description Arrays | Int | 0=Do not Save, 1=Save to New Attribute Matrix, 2=Append to existing AttributeMatrix |
| New AttributeMatrix | DataArrayPath | AttributeMatrix to save the Shape DescriptionArrays into |

## Required Geometry ##

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Attribute Matrix** | CellData | Cell | N/A | **Cell Attribute Matrix** in which to pack **Features** |
| **Ensemble Attribute Array** | Statistics | Statistics Object | (1) | Statistics objects (depending on *Phase Type*) that store fits to descriptors such as size distribution, shape distribution, neighbor distribution, ODF, MDF, etc. |
| **Ensemble Attribute Array** | PhaseTypes | uint32_t | (1) | Enumeration specifying the phase type of each **Ensemble** |
| **Ensemble Attribute Array** | ShapeTypes | uint32_t | (1) | Enumeration specifying the type of shape to place for each **Ensemble** |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |
| **Cell Attribute Array**     | Phases            | int32_t | (1) | Specifies to which **Ensemble** each **Cell** belongs |
| **Attribute Matrix** | CellFeatureData | Cell Feature | N/A | **Feature Attribute Matrix** for the created _Feature Ids_ |
| **Feature Attribute Array** | Phases | int32_t | (1) |  Specifies to which **Ensemble** each **Feature** belongs |
| **Attribute Matrix** | CellEnsembleData | Cell Ensemble | N/A | **Ensemble Attribute Matrix** for the created phases |
| **Ensemble Attribute Array** | NumFeatures | int32_t | (1) |  Specifies the number of **Features** in each **Ensemble** |

## Shape Description Arrays ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | AxisEulerAngles | float | (3) | Morphological Orientation in Bunge Convention (ZX'Z'') |
| **Cell Attribute Array** | AxisLengths | float | (3) | The Ellipsoid Aspect Ratios[a, a/b, c/a] |
| **Cell Attribute Array** | Centroids | float | (3) | The XYZ value for each ellipsoid |
| **Cell Attribute Array** | EquivalentDiameters | float | (1) | Equivalent Spherical Diameter |
| **Cell Attribute Array** | Neighborhoods | int32_t | (1) |  |
| **Cell Attribute Array** | Omega3s | float | (1) | The Shape Parameter |
| **Cell Attribute Array** | Volumes | float | (1) | The Volume of the ellipsoid |

## Example Pipelines ##

+ (02) Single Hexagonal Phase Equiaxed
+ (03) Single Cubic Phase Rolled
+ InsertTransformationPhase
+ (01) Single Cubic Phase Equiaxed
+ (04) Two Phase Cubic Hexagonal Particles Equiaxed
+ (06) SmallIN100 Synthetic

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)