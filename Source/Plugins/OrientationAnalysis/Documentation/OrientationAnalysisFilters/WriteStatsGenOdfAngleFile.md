Write StatsGenerator ODF Angle File {#writestatsgenodfanglefile}
=====

## Group (Subgroup) ##

IO (Output)

## Description ##

This **Filter** is used in a workflow where the user would like to generate a synthetic microstructure with an ODF that matches (as closely as possible) an existing experimental data set or other data set that is being mimicked. The basic workflow is the following:

1. Import Euler angle data (e.g., ANG or CTF files)
2. Optionally threshold the data so each cell is marked as allowable or not-allowable
3. Insert the "Write StatsGenerator ODF Angle File" **Filter** in the **Pipeline**
4. Run the **Pipeline** to produce the file
5. Launch **StatsGenerator**
6. Generate the data
7. Click on the ODF Tab
8. Click on the *Bulk Load* sub tab
9. Select the file that was just written
10. Load the data and inspect the ODF that was generated

**StatsGenerator can not load data from standard .ang or .ctf files. So if you want to get the ODF from an existing experimental data set and you have one of those files then you can use that functionality in StatsGenerator**

## Notes on Implementation ##

+ A separate file is written for each phase
+ Spaces are used as the delimiters between values
+ Default values of 1.0 are used for both the weight and sigma
+ The user has the option to convert the supplied Euler angles to degrees. **StatsGenerator** is able to import Euler angles as either degrees or radians based on user input, so the output type from this **Filter** could remain as radians or be converted to degrees. The user should remain cognizant of what representation their angles are in so that the correct option is chosen during the import process in **StatsGenerator**

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Output File | File Path | Output angles file path |  
| Convert to Degrees | bool | Whether to convert the Euler angles from radians to degrees. If the Euler angles are already in degrees, this option will "convert" the data again, resulting in garbage orientations! |  
| Only Write Good Elements | bool | Whether to only write the Euler angles for those elements denoted as true in the supplied mask array |  


## Required Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| **Element Attribute Array** | EulerAngles | float | (3) | Three angles defining the orientation of the **Element** in Bunge convention (Z-X-Z) |
| **Element Attribute Array** | Phases | int32_t | (1) |  Specifies to which **Ensemble** each **Element** belongs |
| **Element Attribute Array** | Mask | bool | (1) | Used to define **Elements** as *good* or *bad*. Only required if _Only Write Good Elements_ is checked |

## Created Objects ##

None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)

