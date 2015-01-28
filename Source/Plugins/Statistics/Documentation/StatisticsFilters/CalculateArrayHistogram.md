Calculate Array Histogram {#calculatearrayhistogram}
==========

## Group (Subgroup) ##
Statistics Filters (Ensemble)

## Description ##
This filter will bin a specified scalar **Array**.  The user will select the number of bins (a value greater than zero). The user will enter a new attribute matrix name and a new histogram array name. If the user clicks the new Data Container check box, a new data container name must be entered. Otherwise the current data container will be used. To normalize the array, the user clicks the normalize check box.  The bins will be stored in the histogram array. The normalization is discrete probability distribution. See Normalization Quick Reference at https://plot.ly/histogram/. 

## Parameters ##

| Name | Type | Description |
|------|------| ----------- |
| Scalar Array To Bin | various | Create a histogram of this scalar array. |
| Number Of Bins | Integer greater than zero | |
| Normalize | Boolean | TRUE the array will be normalized |

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| various  | Array        | The array |


## Created Arrays ##
| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Ensemble | Statistics |  |  |

## Authors ##

**Copyright:** 2013 Joseph C. Tucker (UES), 2012 Michael A. Groeber (AFRL), 2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

