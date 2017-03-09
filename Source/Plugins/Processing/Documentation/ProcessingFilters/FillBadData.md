Fill Bad Data {#fillbaddata}
=============

## Group (Subgroup) ##
Processing (Cleanup)

## Description ##
This **Filter** removes small *noise* in the data, but keeps larger regions that are possibly **Features**, e.g., pores or defects. This **Filter** collects the *bad* **Cells** (*Feature Id = 0*) and _erodes_ them until none remain. However, contiguous groups of *bad* **Cells** that have at least as many **Cells** as the minimum allowed defect size enter by the user will not be _eroded_.

## Parameters ##
| Name | Type | Decision |
|------|------|------|
| Minimum Allowed Defect Size | int32_t | The size at which a group of *bad* **Cells** are left unfilled as a "defect" |
| Replace Bad Data | bool | Whether to replace all data or just *Feature Ids* |
| Store Defects as New Phase | bool | Whether to change the phase of "defects" larger than the minimum allowed size above |

## Required Geometry ##
Image 

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Cell Attribute Array** | FeatureIds | int32_t | (1) | Specifies to which **Feature** each **Cell** belongs |
| **Cell Attribute Array** | Phases | int32_t | (1) | Specifies to which **Ensemble** each **Cell** belongs. Only required if _Store Defects as New Phase_ is checked |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


