Find Volume Fractions of Phases {#findvolfractions}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This filter determines the volume fraction of each **Ensemble**.  Technically, the filter counts the number of **Elements** belonging to each **Ensemble** and stores the number fraction.  In the case of an *Image Geometry* (or any geometry with **Elements** of a single size), this will also be the volume fraction

## Parameters ##
None 

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | CellPhases | Int | (1) | Id specifying the **Ensemble** of the **Element** |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Ensemble | VolFractions | Float | (1) | Fraction of volume that belongs to each **Ensemble** |

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

