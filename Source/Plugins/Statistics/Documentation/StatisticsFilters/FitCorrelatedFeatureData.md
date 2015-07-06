Name of Filter {#fitcorrelatedfeaturedata}
=====

## Group (Subgroup) ##
Filter Group (Filter Subgroup)


## Description ##
This filter will correlate one **Feature** with another **Feature** and bin the result.

## Parameters ##
| Name             | Type |
|------------------|------|
| Feature Array To Fit | String | Feature to be fit to a distribution. |
| Distribution Type | Choice | Choose from a lognormal, power and beta distribution. |
| Array To Correlate With | String | Feature to correlate with the Selected Feature Array To Fit. |
| Number Of Bins For Correlated Array | Integer | |
| Remove Biased Features | Boolean | TRUE if biased features are to be omitted from the binning counts. |

## Required Objects ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Feature | BiasedFeatures | Boolean flag of 1 if **Feature** is biased or of 0 if it is not. | | Find Biased Features (Generic) |
| Feature | SelectedFeature | | | |
| Feature | Correlated Feature | | | |

## Created Objects ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Ensemble | Statistics |  |  |



## Authors ##

**Copyright** 2014 Joseph C. Tucker (UES), 2012 Michael A. Groeber (AFRL), 2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 5.0.0

**License**  See the License.txt file that came with DREAM3D.



## License & Copyright ##

Please see the description file distributed with this **Plugin**

## DREAM.3D Mailing Lists ##

If you need more help with a **Filter**, please consider asking your question on the [DREAM.3D Users Google group!](https://groups.google.com/forum/?hl=en#!forum/dream3d-users)

