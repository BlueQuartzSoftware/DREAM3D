#!/bin/bash

#set -e

echo "[D3D_Prebuilt_@test_index@] Prebuilt Pipeline Test Starting"
echo "    @test@.json"

DEBUG_EXT=""
if [ "@CMAKE_BUILD_TYPE@" = "Debug" ]; then
    DEBUG_EXT="@EXE_DEBUG_EXTENSION@"
fi

cd "@CMAKE_RUNTIME_OUTPUT_DIRECTORY@"
"./PipelineRunner${DEBUG_EXT}" --pipeline "@Example_PIPELINE_FILE@"

if [ $? -eq 0 ]
then
  echo "[@test@] Success"
  exit 0
else
  echo "[@test@] Failure" >&2
  exit 1
fi

