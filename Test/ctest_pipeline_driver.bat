@echo off

@echo "[@test@.json] Prebuilt Pipeline Test Starting"

cd @CMAKE_RUNTIME_OUTPUT_DIRECTORY@

PipelineRunner@EXE_EXT@ --pipeline "@DREAM3D_SUPPORT_DIR@/@test@.json"
