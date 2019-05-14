#!/bin/bash

#set -e

echo "[@test@.json] Prebuilt Pipeline Test Starting"

cd "@CMAKE_RUNTIME_OUTPUT_DIRECTORY@"
"./PipelineRunner@EXE_EXT@" --pipeline "@DREAM3D_SUPPORT_DIR@/@test@.json"

if [ $? -eq 0 ]
then
  echo "[@test@] Success"
  exit 0
else
  echo "[@test@] Failure" >&2
  exit 1
fi

