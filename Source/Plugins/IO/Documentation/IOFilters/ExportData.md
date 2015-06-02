Export Data {#exportdata}
=====

## Group (Subgroup) ##

IO Filters (Output)


## Description ##


This filter writes an array to a file as ASCII representations. The file extension and the
maximum tuples per line are entered. The file delimiter is selected from an enumerated
list of values. The maximum number of tuples are selected by the user. For example,
if an array has only 1 component (like a simple scalar array) and the user selects
"1" for the Maximum Tuples/Line then only a single vale will appear on each line.
If the user selects an array that has 3 components (an array of 3D coordinates representing
X, Y, Z locations in space) and the user selected 1 Tuple Per Line, then the file
will actually contain 3 values per line (the X, Y, Z Value). If that same user
selected 3 tuples per line then 9 values would be printed per line. More than one array to export may be selected at a time. All arrays may be selected with the Select/Deselect All checkbox.


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

| Name             | Type |
|------------------|------|
| Output Path | Output File Path |
| File Extension | String |
| Maximum Tuples Per Line | Int > 0 |
| Delimiter | Int |
| Select/Deselect All | Boolean |



## Required Arrays ##

| Type | Default Array Name | Description |
|------|--------------------|-------------|
| Any numeric array | None | One or more arrays can be selected  |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


