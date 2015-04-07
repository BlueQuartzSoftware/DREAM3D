Find Feature Largest Cross-Sections {#findlargestcrosssections}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This filter calculates the largest cross-sectional area on a user-defined plane for all **Features**.  The filter simply iterates through all **Cells** (on each section) asking for **Feature** that owns them.  
On each section, the count of **Cells** for each **Feature** is then converted to an area and stored as the *LargestCrossSection* if the area for the current section is larger than the existing *LargestCrossSection* for that **Feature**.

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Plane of Interest | Choice | Specifies which plane to consider when determining the maximum cross-section for each **Feature** |

## Required Geometry ##
Image/Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs. |

## Created Arrays ##

| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | LargestCrossSection | Float | (1) | Area of largest cross-section for **Feature** perpendicular to the user specified direction. |

## Authors ##

**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

