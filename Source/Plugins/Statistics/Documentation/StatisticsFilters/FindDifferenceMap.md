FindDifferenceMap {#finddifferencemap}
=============

## Group (Subgroup) ##
Statistics (Misc)

## Description ##
This **Filter** calculates the difference between two **Attribute Arrays**. The arrays must have the same _primitive type_ (e.g., float), _component dimensions_ and _number of tuples_. The **Filter** uses the following pseudocode to calculate the difference map:

	for i to all tuples in the arrays
		for j to number of components of the arrays
			differenceMap[number of components * i + j] = firstSelectedArray[number of components * i + j] - secondSelectedArray[number of components * i + j]
		end loop over number of components
	end loop over all tuples

Note that in the above algorithm, the difference is taken as the _first selected_ **Attribute Array** minus the _second selected_ **Attribute Array**. The differences are also taken _component by component_. Therefore, two selected scalar arrays will result in a scalar difference map, whereas two selected 3 component vector arrays would result in a 3 component vector difference map, where component values are the scalar differences between the components of the input arrays.

**WARNING: The resulting difference values _may be negative_ if the values of the first array are smaller than those in the second array. Therefore, if the two arrays are _unsigned integers_, it may be possible to underflow the resulting difference map values if the first array is sufficiently smaller than the second array. "Underflow" means that what should be a negative value will end up being a potentially very large value!**

## Parameters ##
None

## Required Geometry ##
Not Applicable

## Required Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Any **Attribute Array** | None | Any | Any | The first selected **Attribute Array**. This array's values will be the first argument for the subtraction operation |
| Any **Attribute Array** | None | Any | Any | The second selected **Attribute Array**. This array's values will be subtracted from the first selected **Attribute Array** values |

## Created Objects ##

| Kind | Default Name | Type | Component Dimensions | Description |
|------|--------------|------|----------------------|-------------|
| Any **Attribute Array** | DifferenceMap | Same as input arrays | Same as input arrays | The difference map **Attribute Array** |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users