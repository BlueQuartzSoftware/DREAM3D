
@echo off
:: Check to see if we were built using the Visual Studio generators, if so
:: then we need an additional argument to tell this script which configuration
:: directory to use as the source for the compiled files.
:: set CONFIG_DIR=%CMAKE_CONFIG_TYPE%
IF "@MSVC_IDE@" == "1" (
    set CONFIG_DIR=%CMAKE_CONFIG_TYPE%
) else (
    set CONFIG_DIR=
) 

::-----------------------------------------------------------------------------
IF "%CMAKE_CONFIG_TYPE%" == "Debug" (
    set EXE_SFX="_debug"
)

::-----------------------------------------------------------------------------
IF "%CMAKE_CONFIG_TYPE%" == "Release" (
    set EXE_SFX=""
)

::-----------------------------------------------------------------------------
IF "@CMAKE_BUILD_TYPE@" == "Debug" (
    set CONFIG_DIR=
    set EXE_SFX="_debug"
)

::-----------------------------------------------------------------------------
IF "@CMAKE_BUILD_TYPE@" == "Release" (
    set CONFIG_DIR=
    set EXE_SFX=""
)

echo on
@echo "Prebuilt Pipeline Test Starting"
@echo "    @test@.json"

@echo "Running Executable at @CMAKE_RUNTIME_OUTPUT_DIRECTORY@\%CONFIG_DIR%\PipelineRunner%EXE_SFX%@EXE_EXT@"

cd @CMAKE_RUNTIME_OUTPUT_DIRECTORY@\%CONFIG_DIR%

PipelineRunner%EXE_SFX%@EXE_EXT@ --pipeline "@Example_PIPELINE_FILE@"
