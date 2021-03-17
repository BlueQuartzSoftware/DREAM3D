from setuptools import setup, find_packages

setup(
  name='dream3d',
  version='1.0.0',
  packages=find_packages(),
  package_data={'dream3d': ['*.pyd']},
  include_package_data=True,
  zip_safe=False
)
