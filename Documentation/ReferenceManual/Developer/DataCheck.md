The DataCheck Function {#dataCheckPage}
========

Intro goes here...

## Major Functions ##
<a name="dataCheck">
### dataCheck() ###
</a>

The **dataCheck()** function is the section where the filter's developer can write all the sanity checks for the filter to make sure that the filter will run smoothly and without errors.  These checks will be executed when preflight runs on the filter, and errors and warnings will appear if any of the checks fail.

Some examples of common sanity checks:
- Created arrays have been given unique names.
- Arrays needed by the filter currently exist when this filter is running.
- Created arrays' tuple dimensions match their attribute matrix's tuple dimensions.
- Values that cannot be negative should be positive, and vice versa.
- The value that was input into the filter is in range. (i.e. An unsigned 8-bit integer array should be initialized only with values between 0-255).