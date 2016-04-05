@echo off
if "%1" == "" goto x86
if not "%2" == "" goto usage

if /i %1 == x86       goto x86
if /i %1 == amd64     goto amd64
if /i %1 == x64       goto amd64
if /i %1 == arm       goto arm
if /i %1 == x86_arm   goto x86_arm
if /i %1 == x86_amd64 goto x86_amd64
if /i %1 == amd64_x86 goto amd64_x86
if /i %1 == amd64_arm goto amd64_arm
goto usage

:x86
if not exist "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\vcvars32.bat" goto missing
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\vcvars32.bat"
@set ARCH_TYPE=i386
goto :SetVisualStudioVersion

:amd64
if not exist "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\amd64\vcvars64.bat" goto missing
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\amd64\vcvars64.bat"
@set ARCH_TYPE=x64
goto :SetVisualStudioVersion

:arm
if not exist "%~dp0bin\arm\vcvarsarm.bat" goto missing
call "%~dp0bin\arm\vcvarsarm.bat"
goto :SetVisualStudioVersion

:x86_amd64
if not exist "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\x86_amd64\vcvarsx86_amd64.bat" goto missing
call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\bin\x86_amd64\vcvarsx86_amd64.bat"
@set ARCH_TYPE=x64
goto :SetVisualStudioVersion

:x86_arm
if not exist "%~dp0bin\x86_arm\vcvarsx86_arm.bat" goto missing
call "%~dp0bin\x86_arm\vcvarsx86_arm.bat"
goto :SetVisualStudioVersion

:amd64_x86
if not exist "%~dp0bin\amd64_x86\vcvarsamd64_x86.bat" goto missing
call "%~dp0bin\amd64_x86\vcvarsamd64_x86.bat"
goto :SetVisualStudioVersion

:amd64_arm
if not exist "%~dp0bin\amd64_arm\vcvarsamd64_arm.bat" goto missing
call "%~dp0bin\amd64_arm\vcvarsamd64_arm.bat"
goto :SetVisualStudioVersion

:SetVisualStudioVersion
set VisualStudioVersion=12.0
goto :env_setup

:usage
echo Error in script usage. The correct usage is:
echo     %0 [option]
echo where [option] is: x86 ^| amd64 ^| arm ^| x86_amd64 ^| x86_arm ^| amd64_x86 ^| amd64_arm
echo:
echo For example:
echo     %0 x86_amd64
goto :eof

:missing
echo The specified configuration type is missing.  The tools for the
echo configuration might not be installed.
goto :eof


:env_setup

@rem Setup some environment variables in order to collect those into a single location
@set DREAM3D_SDK=C:/DREAM3D_SDK
@set PARALLEL_BUILD=8
@set HDF_VERSION=1.8.16
@set ITK_VERSION=4.9.0

@set CMAKE_INSTALL=%DREAM3D_SDK%\cmake-3.5.1-win32-x86
@set PATH=%PATH%;%CMAKE_INSTALL%\bin

@set CMAKE_MODULE_PATH=%DREAM3D_SDK%\hdf5-%HDF_VERSION%\cmake

cmake -P Build_ITK.cmake
