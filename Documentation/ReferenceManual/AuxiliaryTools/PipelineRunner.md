PipelineRunner {#pipelinerunner}
========
 
# Introduction #

PipelineRunner is a _Terminal_ or _Command Line_ application that is design to simply run a DREAM.3D pipeline without the use of a GUI. Because of this design PipelineRunner can be launched through a Terminal or Command Prompt or through other ways such as directly from a shell script (Linux), batch file (Windows), python script, IDL script or MATLAB script.

PipelineRunner only takes a single argument which is the path to the pipeline file (as a .json file). An example of its usage is:


### Unix Terminal ###

	[user@machine] $ ./PipelineRunner -p /Some/Path/to/Your/Pipeline.json
	

### Windows Command Prompt ###

	[ user ] > PipelineRunner -p C:/Some/Path/To/Your/Pipeline.json
	

Any output from the filters will be printed to the console. This includes progress information which can make the output very long for some pipelines. Using advanced shell or batch file techniques the user can "pipe" or redirect the output to a log file of their choosing.

# Use Cases #

There are several use cases for PipelineRunner. The first is running DREAM.3D pipelines from another environment such as Python or MATLAB. Other uses include having another program systematically generate a pipeline file and the have PipelineRunner execute that pipeline. This can be useful to perform a parameters study on specific filters or studying how inputs might affect the output of a filter.