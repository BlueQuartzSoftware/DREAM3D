Write ASCII Data {#writeasciidata}
=============

## Group (Subgroup) ##
IO (Output)

## Description ##
This **Filter** writes an array to a file as ASCII representations. The user may select the file extension and the maximum number of tuples printed per line. The user may also select the file delimiter from an enumerated list of values.  For example, if an array has only 1 component (a simple scalar array) and the user selects "1" for the _Maximum Tuples Per Line_ parameter then only a single vale will appear on each line. If the user selects an array that has 3 components (an array of 3D coordinates representing X, Y, Z locations in space) and the user selected 1 tuple per line, then the file will actually contain 3 values per line (the X, Y, Z values). If that same user selected 3 tuples per line then 9 values would be printed per line, and so on. More than one array to export may be selected at a time. All arrays may be selected or deselected at once with the _Select/Deselect All_ checkbox.  Each exported array is written as a separate file.  All file names will match the array name.


### Example Output ###
Phases.txt (one component) with 3 tuples/line, comma delimited:     

	1,1,1
	1,1,1
	1,1,1  
	1,1,1
	….. 

EulerAngles.txt (three components) with 3 tuples/line, comma delimited:     

	0.785398,0,0.785398,0.785398,0,0.785398,0.785398,0,0.785398
	0.785398,0,0.785398,0.785398,0,0.785398,0.785398,0,0.785398
	0.785398,0,0.785398,0.785398,0,0.785398,0.785398,0,0.785398  
	0.785398,0,0.785398,0.785398,0,0.785398,0.785398,0,0.785398
	….. 

EulerAngles.txt (three components) with 1 tuple/line, space delimited:     

	0.785398 0 0.785398
	0.785398 0 0.785398
	0.785398 0 0.785398  
	0.785398 0 0.785398
	….. 

### Delimiter ###
Choice of delimiter is as follows:

    , (comma)
    ; (semicolon)
      (space)
    : (colon)
    \t (tab)

## Parameters ##
| Name             | Type | Description |
|------------------|------|------------|
| Output Path | File Path | The output file path |
| File Extension | String | File extension for output file(s) |
| Maximum Tuples Per Line | int32_t | Number of tuples to print on each line |
| Delimiter | Enumeration | The delimeter separating the data |

## Required Geometry ##
Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Any **Attribute Array** | None | Any | Any | Output arrays to be written as ASCII representations |

## Created Objects ##
None

## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


