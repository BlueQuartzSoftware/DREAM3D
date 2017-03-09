Minimum Size {#minsize}
=============

## Group (Subgroup) ##
Processing (Cleanup)

## Description ##
This **Filter** removes **Features** that have a total number of **Cells** below the minimum threshold defined by the user. Entering a number larger than the largest **Feature** generates an _error_ (since all **Features** would be removed). Hence, a choice of threshold should be carefully be chosen if it is not known how many **Cells** are in the largest **Features**. After removing all the small **Features**, the remaining **Features** are isotropically coarsened to fill the gaps left by the small **Features**.

The **Filter** can be run in a mode where the minimum number of neighbors is applied to a single **Ensemble**.  The user can select to apply the minimum to one specific **Ensemble**.

## Parameters ##
| Name | Type | Description |
|------|------|-------------|
| Minimum Allowed Feature Size | int32_t | Number of **Cells** that must be present in the **Feature** for it to remain in the sample |
| Apply to Single Phase Only | bool | Tells the Filter whether to apply minimum to single ensemble or all ensembles |
| Phase Index | int32_t | Which **Ensemble** to apply minimum to. Only needed if _Apply to Single Phase Only_ is checked |

## Required Geometry ##
Image

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |
| **Feature Attribute Array** | Phases | int32_t | (1) | Specifies to which **Ensemble** each **Feature** belongs. Only required if _Apply to Single Phase Only_ is checked  |
| **Feature Attribute Array** | NumCells | int32_t | (1) | Specifies the number of **Cells** belonging to each **Feature** |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


