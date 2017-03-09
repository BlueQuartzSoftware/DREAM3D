Find Volume Fractions of Ensembles {#findvolfractions}
=============

## Group (Subgroup) ##
Statistics (Morphological)

## Description ##
This **Filter** determines the volume fraction of each **Ensemble**. The **Filter** counts the number of **Cells** belonging to each **Ensemble** and stores the number fraction.

## Parameters ##
None 

## Required Geometry ##
Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | Phases | int32_t | (1) | Specifies to which **Ensemble** each **Cell** belongs |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Ensemble Attribute Array** | VolFractions | float | (1) | Fraction of volume that belongs to each **Ensemble** |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


