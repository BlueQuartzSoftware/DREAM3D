Find Feature Boundary Element Fractions {#findboundaryelementfractions}
=============

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This Filter calculates the fraction of **Elements** of each **Feature** that are on the "surface" of that **Feature**.  The Filter simply iterates through all **Elements** asking for the **Feature** that owns them and if the **Element** is a "surface" **Element**.  Each **Feature** counts the total number of **Elements** it owns and the number of those **Elements** that are "surface" **Elements**.  The fraction is then stored for each **Feature**.

## Parameters ##
None

## Required Geometry ##
Not Applicable

## Required Arrays ##

| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | FeatureIds | Int | (1) | Specifies to which **Feature** each **Element** belongs. |
| Element | BoundaryCells | Int | (1) | The number of neighboring **Elements** of a given **Element** that belong to a different **Feature** than itself. Values will range from *0* to *6* |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | SurfaceElementFractions | Float | (1) | Fraction of **Elements** belonging to the **Feature** that are "surface" **Elements**. |

## Authors ##
**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


