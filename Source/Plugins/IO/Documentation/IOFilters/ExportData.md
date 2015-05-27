Export Data {#exportdata}
=====

## Group (Subgroup) ##

IO Filters (Output)


## Description ##


This filter writes an array to a file as ASCII representations. The file extension and the
maximum tuples per line are entered. The file delimiter is selected from an enumerated
list of values. The maximum number of Tuples are selected by the user. For example
if an array has only 1 Component (Like a simple scalar array) and the user selects
"1" for the Maximum Tuples/Line then only a single vale will appear on each line.
If the user selects an array that has 3 components (An array of 3D coordinates representing
X, Y, Z locations in space) and the user selected 1 Tuple Per line, then the file
will actually contain 3 values per line (The X, Y, Z Value). If that same user
selected 3 tuples per line then 9 values would be printed per line.

### Delimiter ###

Choice of delimiter is as follows:

    , (comma)
    ; (semicolon)
      (space)
    : (colon)
    TAB

## Parameters ##

| Name             | Type |
|------------------|------|
| Output Path | Output File Path |
| File Extension | String |
| Maximum tuples Per Line | Int > 0 |
| Delimiter | Int |



## Required Arrays ##

| Type | Default Array Name | Description |
|------|--------------------|-------------|
| Any numeric array | NONE | Any array can be selected  |


## Authors ##

**Copyright** 2015 BlueQuartz Software, LLC

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


