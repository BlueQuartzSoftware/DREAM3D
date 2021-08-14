import os
import sys
import subprocess
from os.path import join, pathsep

# based on cpw.py

env = os.environ.copy()

prefix = env['CONDA_PREFIX']

new_paths = pathsep.join([join(prefix, "bin")])

scripts_dir = join(prefix, "Scripts")

# insert %PREFIX%\bin after %PREFIX\Scripts which cpw.py doesn't do
# even though it is there when actually activating the environment
# required for tbb which installs there instead of %LIBRARY_PREFIX%\bin

index = env['PATH'].find(scripts_dir)
if index < 0:
  raise RuntimeError('Invalid environment: Missing conda paths')

left_side = env['PATH'][:index - 1]
right_side = env['PATH'][index + len(scripts_dir) + 1:]
env['PATH'] = left_side + pathsep + new_paths + pathsep + right_side

sys.exit(subprocess.call([f'{prefix}\\Library\\bin\\DREAM3D.exe'], env=env))
