Export StatsGenerator ODF Angle File {#writestatsgenodfanglefile}
=====

## Group (Subgroup) ##

IO (Output)

## Description ##

This **Filter** is used in a workflow where the user would like to generate a synthetic microstructure with an ODF that matches (as closely as possible) an existing experimental data set or other data set that is being mimicked. The basic workflow is the following:

1. Import Euler angle data (e.g., ANG or CTF files)
2. Optionally threshold the data so each cell is marked as allowable or not-allowable
3. Insert the "Export StatsGenerator ODF Angle File" **Filter** in the **Pipeline**
4. Run the **Pipeline** to produce the file
5. Launch **StatsGenerator**
6. Generate the data
7. Click on the ODF Tab
8. Click on the *Bulk Load* sub tab
9. Select the file that was just written
10. Load the data and inspect the ODF that was generated


## Important Change from Earlier Versions of StatsGenerator ##

**StatsGenerator can not load data from standard .ang or .ctf files. If you want to get the ODF from an existing experimental data set and you have one of those files then you must use the functionality of this filter**

## Notes on Implementation ##

+ A separate file is written for each phase
+ Spaces are the default as the delimiters between values. The user can select another value
+ Default values of 1.0 are used for both the _weight_ and _sigma_. **If the user needs a stronger texture due to a low number of angles then larger values should be used such as 10, 100 or even 1000.**
+ The user has the option to convert the supplied Euler angles to degrees. **StatsGenerator** is able to import Euler angles as either degrees or radians based on user input, so the output type from this **Filter** could remain as radians or be converted to degrees. The user should remain cognizant of what representation their angles are in so that the correct option is chosen during the import process in **StatsGenerator**

## Example File ##

The file written is a simple text file that contains a short comment section and a single _Header_ line of data. All comment lines should come **BEFORE** the actual header line. There is a single header line in the form of "Key:Value" and then the lines of data.

	# All lines starting with '#' are comments and should come before the header.
	# DREAM.3D StatsGenerator Angles Input File
	# DREAM.3D Version 6.1.107.0d8bad9
	# Angle Data is space delimited.
	# Euler0 Euler1 Euler2 Weight Sigma
	Angle Count:100
	0 0 0 1 1
	3.6 1.8 3.6 1 1
	7.2 3.6 7.2 1 1
	10.8 5.4 10.8 1 1
	14.4 7.2 14.4 1 1
	
The **only** required header line is:

	Angle Count:100

There are 5 columns of data which are the 3 Euler Angles, the Weight Value and the Sigma Value.


### Delimiter ###
Choice of delimiter is as follows:

    0 = , (comma)
    1 = ; (semicolon)
    2 = (space)   <==== DEFAULT VALUE
    3 = : (colon)
    4 = \t (tab)


## Parameters ##

| Name | Type | Description |
|---------|---------| --------------- |
| Output File | File Path | Output angles file path |
| Default Weight | float | This value will be used for the Weight column |
| Default Sigma | int | This value will be used for the Sigma column |
| Delimiter | Enumeration | The delimeter separating the data |
| Convert to Degrees | bool | Whether to convert the Euler angles from radians to degrees. If the Euler angles are already in degrees, this option will "convert" the data again, resulting in garbage orientations! |
| Only Write Good Elements | bool | Whether to only write the Euler angles for those elements denoted as true in the supplied mask array |


## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|-------|---------------------|--------|---------------------------------|-----------------|
| **Element Attribute Array** | EulerAngles | float | (3) | Three angles defining the orientation of the **Element** in Bunge convention (Z-X-Z) |
| **Element Attribute Array** | Phases | int32_t | (1) |  Specifies to which **Ensemble** each **Element** belongs |
| **Element Attribute Array** | Mask | bool | (1) | Used to define **Elements** as *good* or *bad*. Only required if _Only Write Good Elements_ is checked |

## Created Objects ##

None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)

