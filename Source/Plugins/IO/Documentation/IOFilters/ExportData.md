Export Data {#exportdata}
=====

## Group (Subgroup) ##
IO Filters (Output)


## Description ##
This filter writes a scalar array to a file. The file extension and the maximum values per line are entered. The file delimeter is selected.

**Delimeter**

Choice of delimeter is as follows:
-  , (comma)
-  ; (semicolon)
-    (space)
-  : (colon) 

## Parameters ##
| Name             | Type |
|------------------|------|
| Output Path | Output File Path |
| File Extension | String |
| Maximum Values Per Line | Int > 0 |
| Delimeter | Int |

# Required DataContainers ##
Any

## Required Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Scalar  | SomeName        


## Created Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Text  | ExportData          



## Authors ##

**Copyright** 2014 Michael A. Groeber (AFRL), 2014 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

