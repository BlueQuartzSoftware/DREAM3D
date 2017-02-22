Establish Matrix Phase {#establishmatrixphase}
=============

## Group (Subgroup) ##
Synthetic Building (Packing)

## Description ##
This **Filter** inserts matrix phase(s) into a synthetic volume. A _matrix_ phase can be considered a contiguous region within a volume. The algorithm for inserting matrix phase(s) is as follows:

1. Identify all the matrix phases from the **Ensemble** *Phase Types* array
2. Determine the relative volume fractions of the matrix phases (i.e., if there are two matrix phases and they have volume fractions of *0.2* and *0.3*, respectively, then they would be scaled to *0.4* and *0.6* to total *1*) 
3. Identify all **Cells** that are not currently assigned to a **Feature** (i.e., belong to **Feature** *0*).
4. For each "unassigned" **Cell**, generate a random number to decide which matrix phase the **Cell** will be assigned to.  

The **Filter** requires the user to select a **Cell Attribute Matrix** in which to place the generated **Cell** level **Attribute Arrays**. This **Attribute Matrix** is most likely generated when [initializing the synthetic volume](@ref initializesyntheticvolume). The **Filter** will also create a new **Cell Feature Attribute Matrix** and **Cell Ensemble Attribute Matrix** to correspond to the instantiated **Features** and **Ensembles** in the **Image Geometry**.

*Note:* The **Filter** _does not_ actually try to match the volume fractions of the matrix phases, but rather just uses the relative volume fractions to decide what fraction of the "unassigned" **Cells** get assigned to each matrix phase.  If the fraction of unassigned  **Cells** is smaller or larger than the total volume fractions of the matrix phases, then the absolute volume fractions of the matrix phases will not match the goal defined in the _Statistics_ array.

For more information on synthetic building, visit the [tutorial](@ref tutorialsyntheticsingle).

## Parameters ##
None

## Required Geometry ##
Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Attribute Matrix** | CellData | Cell | N/A | **Cell Attribute Matrix** in which to establish the matrix phase |
| **Ensemble Attribute Array** | Statistics | Statistics Object | (1) | Statistics objects (depending on *Phase Type*) that store fits to descriptors such as size distribution, shape distribution, neighbor distribution, ODF, MDF, etc. |
| **Ensemble Attribute Array** | PhaseTypes | uint32_t | (1) | Enumeration specifying the phase type of each **Ensemble** |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | FeaturesIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |
| **Cell Attribute Array**  | Phases | int32_t | (1) |  Specifies to which **Ensemble** each **Cell** belongs |
| **Attribute Matrix** | CellFeatureData | Cell Feature | N/A | **Feature Attribute Matrix** for the created _Feature Ids_ |
| **Feature Attribute Array** | Phases | int32_t | (1) |  Specifies to which **Ensemble** each **Feature** belongs |
| **Attribute Matrix** | CellEnsembleData | Cell Ensemble | N/A | **Ensemble Attribute Matrix** for the created phases |
| **Ensemble Attribute Array** | NumFeatures | int32_t | (1) |  Specifies the number of **Features** in each **Ensemble** |


## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


