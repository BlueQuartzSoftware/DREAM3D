Find Feature Histogram {#findfeaturehistogram}
==========

## Group (Subgroup) ##
Statistics Filters (Ensemble)

## Description ##
This filter will bin a specified **Feature** level attribute.  The user can chose both which attributes to bin and the number of bins.  The bins will be stored in an **Ensemble** array.

## Parameters ##

| Name | Type | Description |
|------|------| ----------- |
| Feature Array To Bin | String | Feature to be binned. |
| Number Of Bins | Integer | |
| Remove Biased Features | Boolean | TRUE if biased features are to be omitted from the binning counts. |

## Required DataContainers ##
Volume

## Required Objects ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Feature | BiasedFeatures | Boolean flag of 1 if **Feature** is biased or of 0 if it is not. | | Find Biased Features (Generic) |
| Feature | SelectedFeature | Selected feature to be binned. | | |


## Created Objects ##
| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Ensemble | Statistics |  |  |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)


