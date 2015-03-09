Find Feature Phases {#findfeaturephases}
==========
## Group (Subgroup) ##

Generic Filters (Misc)

## Description ##

This filter determines the phase of each **Feature** by querying the phase of the **Elements** that belong to the **Feature**.
Note that it is assumed that all **Elements** belonging to a **Feature** are of the same phase, and thus any **Element** can be used to determine the phase of the **Feature** that owns that **Element**.

## Parameters ##

None

## Required Geometry ##

Not Applicable

## Required Arrays ##

| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Element | CellPhases | Int | (1,1) | Specifies the phase of the **Element** |
| Cell | FeatureIds | Int | (1,1) | Specifies to which **Feature** each **Cell** belongs. |

## Created Arrays ##

| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Feature | FeaturePhases | Int | (1,1) | Specifies the phase of the **Feature** - Values will begin at 1 as there is no phase 0, which is used temporarily in some filters for bad data|


## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

