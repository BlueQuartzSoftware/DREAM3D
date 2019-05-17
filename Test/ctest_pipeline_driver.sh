#!/bin/bash

#set -e

echo "[@test@.json] Prebuilt Pipeline Test Starting"

DEBUG_EXT=""
if [ "@BUILD_TYPE@" = "Debug" ]; then
    DEBUG_EXT="@EXE_DEBUG_EXTENSION@"
fi

cd "@CMAKE_RUNTIME_OUTPUT_DIRECTORY@"
"./PipelineRunner${DEBUG_EXT}" --pipeline "@DREAM3D_SUPPORT_DIR@/@test@.json"

if [ $? -eq 0 ]
then
  echo "[@test@] Success"
  exit 0
else
  echo "[@test@] Failure" >&2
  exit 1
fi

