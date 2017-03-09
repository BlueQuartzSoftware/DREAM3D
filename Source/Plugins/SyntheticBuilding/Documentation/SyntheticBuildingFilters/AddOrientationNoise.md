Add Orientation Noise {#addorientationnoise}
=============

## Group (Subgroup) ##
Synthetic Building (Crystallography)

## Description ##
This **Filter** adds "noise" to the orientations on each **Element** of a **Geometry**.  This **Filter**  is intended to add "realism" to synthetic structures that have single orientations assigned to **Features**.  The user can specify a magnitude of the noise, which refers to the maximum rotation angle that is applied to the existing **Element** orientations.  The algorithm for applying the orientation noise is as follows:

1. Obtain the orientation of the **Element**
2. Generate a random number between *0* and the user defined magnitude. This value is the rotation angle
3. Generate a random unit vector.  This is the rotation axis
4. Calculate the rotation corresponding to the axis-angle pair generated in steps 2-3 and apply it to the orientation of the **Element** to obtain a new orientation
5. Repeat for all **Elements**

For more information on synthetic building, visit the [tutorial](@ref tutorialsyntheticsingle).

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Magnitude of Orientation Noise (Degrees) | Float | Maximum rotation angle in degrees to apply to **Element** orientations |

## Required Geometry ##
Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| **Element Attribute Array** | EulerAngles | float | (3) | Three angles defining the orientation of the **Element** in Bunge convention (Z-X-Z) |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


