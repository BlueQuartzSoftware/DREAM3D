Combine Feature/Ensemble Attribute Matrices {#combineattributematrices}
=============

## Group (Subgroup) ##
Core (Memory/Management)

## Description ##
This **Filter** allows the user to specify two **Feature or Ensemble Attribute Matrices**, within the same **Data Container**, to combine.  The **Filter** appends the **Features or Ensembles** from the *second Attribute Matrix* onto the **Features or Ensembles** of the *first Attribute Matrix*, renumbering the **Features or Ensembles** of the *second Attribute Matrix* starting from one plus the number of **Features or Ensembles** in the *first Attribute Matrix*.  The **Filter** will create a new **Feature or Ensemble Attribute Matrix** with a number of tuples equal to the combined size of the two **Attribute Matrices**.  The unique set of **Attribute Arrays** from the combination of the two **Feature or Ensemble Attribute Matrices** will be added to the new **Attribute Matrix**.  The values of each **Attribute Arrays** for each **Feature or Ensemble Attribute Matrix** are then copied into the appropriate tuple in the appropriate **Attribute Array** in the new **Attribute Matrix**.


*Note:* The original **Feature or Ensemble Attribute Matrices** are retained

*Note:* The default values of each **Attribute Array** in the new **Attribute Matrix** are retained in any tuple that was did not contain a value in one of the **Feature or Ensemble Attribute Matrices**. 

## Parameters ##
None

## Required Geometry ##
Not Applicable

## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Attribute Matrix**  | None | **Feature or Ensemble** | N/A | **Feature Attribute Matrix** to append to |
| **Attribute Matrix**  | None | **Feature or Ensemble** | N/A | **Feature Attribute Matrix** to append from |
| **Element or Feature Attribute Array**  | None | int32_t | (1) | **Attribute Array** to use as the index map into the **Feature or Ensemble Attribute Matrix** being appended to |
| **Element or Feature Attribute Array**  | None | int32_t | (1) | **Attribute Array** to use as the index map into the **Feature or Ensemble Attribute Matrix** being appended from |

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Attribute Matrix**  | None | **Feature or Ensemble** | N/A | Combined **Feature or Ensemble Attribute Matrix** |
| **Attribute Arrays**  | None | varies | varies | Set of unique **Attribute Arrays** from the two **Attribute Matrices** being combined. |

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


