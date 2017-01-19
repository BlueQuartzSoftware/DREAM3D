# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#                                                                                               #
# Copyright (c) 2015 The Regents of the University of California, Author: William Lenthe        #
#                                                                                               #
# This program is free software: you can redistribute it and/or modify it under the terms of    #
# the GNU Lesser General Public License as published by the Free Software Foundation, either    #
# version 2 of the License, or (at your option) any later version.                              #
#                                                                                               #
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;     #
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     #
# See the GNU Lesser General Public License for more details.                                   #
#                                                                                               #
# You should have received a copy of the GNU Lesser General Public License                      #
# along with this program.  If not, see <http://www.gnu.org/licenses/>.                         #
#                                                                                               #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

set(_filterGroupName LGPLToolboxFilters)
set(${_filterGroupName}_FILTERS_HDRS "")

#--------
# This macro must come first before we start adding any filters
START_FILTER_GROUP(${LGPLToolbox_BINARY_DIR} "${_filterGroupName}" "LGPLToolbox")


#---------
# List your public filters here
set(_PublicFilters

  ArrayCalculator
)


#--------------
# Loop on all the filters adding each one. In this loop we default to making each filter exposed in the user
# interface in DREAM3D. If you want to have the filter compiled but NOT exposed to the user then use the next loop
foreach(f ${_PublicFilters} )
  ADD_DREAM3D_FILTER(  "LGPLToolbox" "LGPLToolbox"
                        ${_filterGroupName} ${f}
                        ${LGPLToolbox_SOURCE_DIR}/Documentation/${_filterGroupName}/${f}.md TRUE)
endforeach()


#---------------
# This is the list of Private Filters. These filters are available from other filters but the user will not
# be able to use them from the DREAM3D user interface.
set(_PrivateFilters

)

#-----------------
# Loop on the Private Filters adding each one to the DREAM3DLib project so that it gets compiled.
foreach(f ${_PrivateFilters} )
  ADD_DREAM3D_FILTER(  "LGPLToolbox" "LGPLToolbox"
                        ${_filterGroupName} ${f}
                        ${SIMPLib_FILTER_DOC_DIR}/${_filterGroupName}/${f}.md FALSE)
endforeach()



#---------------------
# This macro must come last after we are done adding all the filters and support files.
END_FILTER_GROUP(${LGPLToolbox_BINARY_DIR} "${_filterGroupName}" "LGPLToolbox")

