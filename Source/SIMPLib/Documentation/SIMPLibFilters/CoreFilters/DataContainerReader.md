Read DREAM.3D Data File {#datacontainerreader}
=============

## Group (Subgroup) ##
IO (Input)

## Description ##
This **Filter** reads in a .dream3d data file into the current data structure. The user selects the .dream3d file to be read from using the _Select File_ button. Only the objects that are selected by the user are read into memory. The _Overwrite Existing Data Containers_ check box allows the user to import **Data Containers** into the data structure that have the same name as existing **Data Containers** by overwriting those currently in the data structure. This functionality allows the **Filter** to be placed in the middle of a **Pipeline**. Note that by default, the **Filter** will not allow existing **Data Containers** to be overwritten. Also note that if **Data Containers** that have _different_ names than those in the existing data structure will simply be _merged_ into the current **Data Container Array**.


## Parameters ##

| Name | Type | Description |
|------|------|--------------|
| Select File | File Path | The .dream3d file to read |
| Overwrite Existing Data Containers | bool | Whether to overwrite **Data Containers** in the current data structure that have the same name as **Data Containers** in the incoming .dream3d file |

## Required Geometry ##
Not Applicable

## Required Objects ##
None

## Created Objects ##
| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Any | None | Any | Any | Object(s) read in from the .dream3d will all be inserted into the current data structure |



## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


