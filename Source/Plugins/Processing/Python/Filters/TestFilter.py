from typing import List, Tuple, Union

from dream3d.Filter import Filter, FilterDelegatePy
from dream3d.simpl import DataContainerArray, FilterDelegateCpp, FilterParameter, IntFilterParameter, VectorInt32

class TestPythonFilter(Filter):
  def __init__(self) -> None:
    self.x_min: int = 0

  def _set_x_min(self, value: int) -> None:
    self.x_min = value

  def _get_x_min(self) -> int:
    return self.x_min

  @staticmethod
  def name() -> str:
    return 'TestPythonFilter'

  @staticmethod
  def uuid() -> str:
    return '{f1111318-de28-421b-8aab-bb4a391a3034}'

  @staticmethod
  def group_name() -> str:
    return 'Example'

  @staticmethod
  def sub_group_name() -> str:
    return 'Sub Example'

  @staticmethod
  def human_label() -> str:
    return 'Test Filter (Python)'

  @staticmethod
  def version() -> str:
    return '1.0.0'

  @staticmethod
  def compiled_lib_name() -> str:
    return 'Processing'

  def setup_parameters(self) -> List[FilterParameter]:
    return [
      IntFilterParameter('X Min (Column)', 'x_min', self.x_min, FilterParameter.Category.Parameter, self._set_x_min, self._get_x_min, VectorInt32([-1])),
    ]

  def data_check(self, dca: DataContainerArray, delegate: Union[FilterDelegateCpp, FilterDelegatePy] = FilterDelegatePy()) -> Tuple[int, str]:
    delegate.notifyStatusMessage(f'x_min = {self.x_min}')
    return (0, 'Success')

  def _execute_impl(self, dca: DataContainerArray, delegate: Union[FilterDelegateCpp, FilterDelegatePy] = FilterDelegatePy()) -> Tuple[int, str]:
    return (0, 'Success')
