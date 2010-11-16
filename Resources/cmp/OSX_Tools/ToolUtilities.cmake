
include(GetPrerequisites)


# get_tool_executable
#
# Takes either a ".app" directory name or the name of an executable
# nested inside a ".app" directory and returns the path to the ".app"
# directory in ${tool_var} and the path to its main executable in
# ${executable_var}
#
function(get_tool_executable app tool_var executable_var valid_var)
  set(valid 0)

  if(EXISTS "${app}")
      # Is it an executable file?
      is_file_executable("${app}" is_executable)
      if(is_executable)
          set(${tool_var} "${dotapp_dir}" PARENT_SCOPE)
          set(${executable_var} "${app}" PARENT_SCOPE)
          set(valid 1)
          #message(STATUS "info: handled executable file case...")
      else(is_executable)
        message(STATUS "warning: *NOT* handled - not .app dir, not executable file...")
      endif(is_executable)

  else(EXISTS "${app}")
    message(STATUS "warning: *NOT* handled - directory/file does not exist...")
  endif(EXISTS "${app}")

  if(NOT valid)
    set(${tool_var} "error: not a tool" PARENT_SCOPE)
    set(${executable_var} "error: not a tool" PARENT_SCOPE)
  endif(NOT valid)

  set(${valid_var} ${valid} PARENT_SCOPE)
endfunction(get_tool_executable)

# get_tool_and_executable
#
# Takes either a ".app" directory name or the name of an executable
# nested inside a ".app" directory and returns the path to the ".app"
# directory in ${tool_var} and the path to its main executable in
# ${executable_var}
#
function(get_tool_and_executable app tool_var executable_var valid_var)
  set(valid 0)

  if(EXISTS "${app}")
      # Is it an executable file?
      is_file_executable("${app}" is_executable)
      if(is_executable)
          set(${tool_var} "${app}" PARENT_SCOPE)
          set(${executable_var} "${app}" PARENT_SCOPE)
          set(valid 1)
      else(is_executable)
        message(STATUS "warning: *NOT* handled - not .app dir, not executable file...")
      endif(is_executable)
   
  else(EXISTS "${app}")
    message(STATUS "warning: *NOT* handled - directory/file does not exist...")
  endif(EXISTS "${app}")

  if(NOT valid)
    set(${tool_var} "error: not a tool" PARENT_SCOPE)
    set(${executable_var} "error: not a tool" PARENT_SCOPE)
  endif(NOT valid)

  set(${valid_var} ${valid} PARENT_SCOPE)
endfunction(get_tool_and_executable)


# get_tool_all_executables
#
# Scans the given tool recursively for all executable files and accumulates
# them into a variable.
#
function(get_tool_all_executables tool exes_var)
  set(exes "")

  file(GLOB_RECURSE file_list "${tool}/*")
  foreach(f ${file_list})
    is_file_executable("${f}" is_executable)
    if(is_executable)
      set(exes ${exes} "${f}")
    endif(is_executable)
  endforeach(f)

  set(${exes_var} "${exes}" PARENT_SCOPE)
endfunction(get_tool_all_executables)


# get_item_key
#
# Given a file (item) name, generate a key that should be unique considering the set of
# libraries that need copying or fixing up to make a tool standalone. This is
# essentially the file name including extension with "." replaced by "_"
#
# This key is used as a prefix for CMake variables so that we can associate a set
# of variables with a given item based on its key.
#
function(get_item_key item key_var)
  get_filename_component(item_name "${item}" NAME)
  string(REGEX REPLACE "\\." "_" ${key_var} "${item_name}")
  set(${key_var} ${${key_var}} PARENT_SCOPE)
endfunction(get_item_key)


# clear_tool_keys
#
# Loop over the list of keys, clearing all the variables associated with each
# key. After the loop, clear the list of keys itself.
#
# Caller of get_tool_keys should call clear_tool_keys when done with list
# of keys.
#
function(clear_tool_keys keys_var)
  foreach(key ${${keys_var}})
    set(${key}_ITEM PARENT_SCOPE)
    set(${key}_RESOLVED_ITEM PARENT_SCOPE)
    set(${key}_DEFAULT_EMBEDDED_PATH PARENT_SCOPE)
    set(${key}_EMBEDDED_ITEM PARENT_SCOPE)
    set(${key}_RESOLVED_EMBEDDED_ITEM PARENT_SCOPE)
    set(${key}_COPYFLAG PARENT_SCOPE)
  endforeach(key)
  set(${keys_var} PARENT_SCOPE)
endfunction(clear_tool_keys)

# set_tool_key_values
#
# Add a key to the list (if necessary) for the given item. If added,
# also set all the variables associated with that key.
#
function(set_tool_key_values keys_var context item exepath dirs copyflag)
  get_filename_component(item_name "${item}" NAME)

  get_item_key("${item}" key)

  list(LENGTH ${keys_var} length_before)
  gp_append_unique(${keys_var} "${key}")
  list(LENGTH ${keys_var} length_after)

  if(NOT length_before EQUAL length_after)
    gp_resolve_item("${context}" "${item}" "${exepath}" "${dirs}" resolved_item)

    gp_item_default_embedded_path("${item}" default_embedded_path)

    if(item MATCHES "[^/]+\\.framework/")
      # For frameworks, construct the name under the embedded path from the
      # opening "${item_name}.framework/" to the closing "/${item_name}":
      #
      string(REGEX REPLACE "^.*(${item_name}.framework/.*/${item_name}).*$" "${default_embedded_path}/\\1" embedded_item "${item}")
    else(item MATCHES "[^/]+\\.framework/")
      # For other items, just use the same name as the original, but in the
      # embedded path:
      #
      set(embedded_item "${default_embedded_path}/${item_name}")
    endif(item MATCHES "[^/]+\\.framework/")

    # Replace @executable_path and resolve ".." references:
    #
    string(REPLACE "@executable_path" "${exepath}" resolved_embedded_item "${embedded_item}")
    get_filename_component(resolved_embedded_item "${resolved_embedded_item}" ABSOLUTE)

    # *But* -- if we are not copying, then force resolved_embedded_item to be
    # the same as resolved_item. In the case of multiple executables in the
    # original tool, using the default_embedded_path results in looking for
    # the resolved executable next to the main tool executable. This is here
    # so that exes in the other sibling directories (like "bin") get fixed up
    # properly...
    #
    if(NOT copyflag)
      set(resolved_embedded_item "${resolved_item}")
    endif(NOT copyflag)

    set(${keys_var} ${${keys_var}} PARENT_SCOPE)
    set(${key}_ITEM "${item}" PARENT_SCOPE)
    set(${key}_RESOLVED_ITEM "${resolved_item}" PARENT_SCOPE)
    set(${key}_DEFAULT_EMBEDDED_PATH "${default_embedded_path}" PARENT_SCOPE)
    set(${key}_EMBEDDED_ITEM "${embedded_item}" PARENT_SCOPE)
    set(${key}_RESOLVED_EMBEDDED_ITEM "${resolved_embedded_item}" PARENT_SCOPE)
    set(${key}_COPYFLAG "${copyflag}" PARENT_SCOPE)
  else(NOT length_before EQUAL length_after)
    #message("warning: item key '${key}' already in the list, subsequent references assumed identical to first")
  endif(NOT length_before EQUAL length_after)
endfunction(set_tool_key_values)

# get_tool_keys
#
# Loop over all the executable and library files within the tool (and given as
# extra "${libs}") and accumulate a list of keys representing them. Set values
# associated with each key such that we can loop over all of them and copy
# prerequisite libs into the tool and then do appropriate install_name_tool
# fixups.
#
function(get_tool_keys app libs dirs keys_var)
  set(${keys_var} PARENT_SCOPE)

  get_tool_and_executable("${app}" tool executable valid)
  message(STATUS "valid: ${valid}")
  if(valid)
    # Always use the exepath of the main tool executable for @executable_path
    # replacements:
    #
    get_filename_component(exepath "${executable}" PATH)
    set(exes ${executable})

    # But do fixups on all executables in the tool:
    #
    #get_tool_all_executables("${tool}" exes)

    # For each extra lib, accumulate a key as well and then also accumulate
    # any of its prerequisites. (Extra libs are typically dynamically loaded
    # plugins: libraries that are prerequisites for full runtime functionality
    # but that do not show up in otool -L output...)
    #
    foreach(lib ${libs})
      set_tool_key_values(${keys_var} "${lib}" "${lib}" "${exepath}" "${dirs}" 1)

      set(prereqs "")
      get_prerequisites("${lib}" prereqs 1 1 "${exepath}" "${dirs}")
      foreach(pr ${prereqs})
        set_tool_key_values(${keys_var} "${lib}" "${pr}" "${exepath}" "${dirs}" 1)
      endforeach(pr)
    endforeach(lib)

    # For each executable found in the tool, accumulate keys as we go.
    # The list of keys should be complete when all prerequisites of all
    # binaries in the tool have been analyzed.
    #
    message(STATUS "exes: ${exes}")
    foreach(exe ${exes})
      # Add the exe itself to the keys:
      #
      set_tool_key_values(${keys_var} "${exe}" "${exe}" "${exepath}" "${dirs}" 0)
        
      # Add each prerequisite to the keys:
      #
      set(prereqs "")
      get_prerequisites("${exe}" prereqs 1 1 "${exepath}" "${dirs}")
      foreach(pr ${prereqs})
        set_tool_key_values(${keys_var} "${exe}" "${pr}" "${exepath}" "${dirs}" 1)
      endforeach(pr)
    endforeach(exe)

    # Propagate values to caller's scope:
    #
    set(${keys_var} ${${keys_var}} PARENT_SCOPE)
    foreach(key ${${keys_var}})
      set(${key}_ITEM "${${key}_ITEM}" PARENT_SCOPE)
      set(${key}_RESOLVED_ITEM "${${key}_RESOLVED_ITEM}" PARENT_SCOPE)
      set(${key}_DEFAULT_EMBEDDED_PATH "${${key}_DEFAULT_EMBEDDED_PATH}" PARENT_SCOPE)
      set(${key}_EMBEDDED_ITEM "${${key}_EMBEDDED_ITEM}" PARENT_SCOPE)
      set(${key}_RESOLVED_EMBEDDED_ITEM "${${key}_RESOLVED_EMBEDDED_ITEM}" PARENT_SCOPE)
      set(${key}_COPYFLAG "${${key}_COPYFLAG}" PARENT_SCOPE)
    endforeach(key)
  endif(valid)
endfunction(get_tool_keys)

# copy_resolved_item_into_tool
#
# Copy a resolved item into the tool if necessary. Copy is not necessary if the resolved_item
# is the same as the resolved_embedded_item.
#
function(copy_resolved_item_into_tool resolved_item resolved_embedded_item)
  if("${resolved_item}" STREQUAL "${resolved_embedded_item}")
    message(STATUS "warning: resolved_item == resolved_embedded_item - not copying...")
  else("${resolved_item}" STREQUAL "${resolved_embedded_item}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy "${resolved_item}" "${resolved_embedded_item}")
  endif("${resolved_item}" STREQUAL "${resolved_embedded_item}")
endfunction(copy_resolved_item_into_tool)


# deploy_tool_item
#
# Get the direct/non-system prerequisites of the resolved embedded item. For each
# prerequisite, change the way it is referenced to the value of the _EMBEDDED_ITEM
# keyed variable for that prerequisite. (Most likely changing to an "@executable_path"
# style reference.)
#
# Also, change the id of the item being fixed up to its own _EMBEDDED_ITEM value.
#
# Accumulate changes in a local variable and make *one* call to install_name_tool
# at the end of the function with all the changes at once.
#
function(deploy_tool_item resolved_embedded_item exepath dirs)
  # This item's key is "ikey":
  #
  get_item_key("${resolved_embedded_item}" ikey)

  set(prereqs "")
  get_prerequisites("${resolved_embedded_item}" prereqs 1 0 "${exepath}" "${dirs}")

  set(changes "")

  foreach(pr ${prereqs})
    # Each referenced item's key is "rkey" in the loop:
    #
    get_item_key("${pr}" rkey)

    if(NOT "${${rkey}_EMBEDDED_ITEM}" STREQUAL "")
      set(changes ${changes} "-change" "${pr}" "${${rkey}_EMBEDDED_ITEM}")
    else(NOT "${${rkey}_EMBEDDED_ITEM}" STREQUAL "")
      message("warning: unexpected reference to '${pr}'")
    endif(NOT "${${rkey}_EMBEDDED_ITEM}" STREQUAL "")
  endforeach(pr)

  # Change this item's id and all of its references in one call
  # to install_name_tool:
  #
  execute_process(COMMAND install_name_tool
    ${changes} -id "${${ikey}_EMBEDDED_ITEM}" "${resolved_embedded_item}"
  )
endfunction(deploy_tool_item)


# --------------------------------------------------------------------
function(deploy_tool app libs dirs)
  message(STATUS "deploy_tool")
  message(STATUS "  app='${app}'")
  message(STATUS "  libs='${libs}'")
  message(STATUS "  dirs='${dirs}'")

  get_tool_executable("${app}" tool executable valid)
  if(valid)
    get_filename_component(exepath "${executable}" PATH)

    message(STATUS "deploy_tool: preparing...")
    get_tool_keys("${app}" "${libs}" "${dirs}" keys)

    message(STATUS "deploy_tool: copying libraries...")
    list(LENGTH keys n)
    math(EXPR n ${n}*2)

    set(i 0)
    foreach(key ${keys})
      math(EXPR i ${i}+1)
      if(${${key}_COPYFLAG})
        message(STATUS "${i}/${n}: copying '${${key}_RESOLVED_ITEM}'")
      else(${${key}_COPYFLAG})
        #message(STATUS "${i}/${n}: *NOT* copying '${${key}_RESOLVED_ITEM}'")
      endif(${${key}_COPYFLAG})

      set(show_status 0)
      if(show_status)
        message(STATUS "key='${key}'")
        message(STATUS "item='${${key}_ITEM}'")
        message(STATUS "resolved_item='${${key}_RESOLVED_ITEM}'")
        message(STATUS "default_embedded_path='${${key}_DEFAULT_EMBEDDED_PATH}'")
        message(STATUS "embedded_item='${${key}_EMBEDDED_ITEM}'")
        message(STATUS "resolved_embedded_item='${${key}_RESOLVED_EMBEDDED_ITEM}'")
        message(STATUS "copyflag='${${key}_COPYFLAG}'")
        message(STATUS "")
      endif(show_status)

      if(${${key}_COPYFLAG})
        copy_resolved_item_into_tool("${${key}_RESOLVED_ITEM}"
          "${${key}_RESOLVED_EMBEDDED_ITEM}")
      endif(${${key}_COPYFLAG})
    endforeach(key)

    message(STATUS "deploy_tool: adjusting 'install_name' on copied libraries...")
    foreach(key ${keys})
      math(EXPR i ${i}+1)
      message(STATUS "${i}/${n}: adjusting '${${key}_RESOLVED_EMBEDDED_ITEM}'")
      deploy_tool_item("${${key}_RESOLVED_EMBEDDED_ITEM}" "${exepath}" "${dirs}")
    endforeach(key)

    message(STATUS "deploy_tool: cleaning up...")
    clear_tool_keys(keys)

    message(STATUS "deploy_tool: verifying executable has proper library paths...")
    verify_app("${app}")
  else(valid)
    message(STATUS "error: deploy_tool: not a valid tool")
  endif(valid)

  message(STATUS "deploy_tool: done")
endfunction(deploy_tool)


# copy_and_deploy_tool
#
# Makes a copy of the tool "src" at location "dst" and then fixes up the
# new copied tool in-place at "dst"...
#
function(copy_and_deploy_tool src dst libs dirs)
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory "${src}" "${dst}")
  deploy_tool("${dst}" "${libs}" "${dirs}")
endfunction(copy_and_deploy_tool)


# verify_tool_prerequisites
#
# Verifies that the sum of all prerequisites of all files inside the tool
# are contained within the tool or are "system" libraries, presumed to exist
# everywhere.
#
function(verify_tool_prerequisites tool result_var info_var)
  set(result 1)
  set(info "")
  set(count 0)

  set(main_tool_exe "${tool}")
  #message(STATUS "main_tool_exe: ${main_tool_exe}")
  set(file_list "${tool}")
  foreach(f ${file_list})
    is_file_executable("${f}" is_executable)
    if(is_executable)
      get_filename_component(exepath "${f}" PATH)
      #message(STATUS "executable file: ${f}")

      math(EXPR count "${count} + 1")

      set(prereqs "")
      get_prerequisites("${f}" prereqs 1 1 "${exepath}" "")

      # "embedded" and "system" prerequisites are fine... anything else means
      # the tool's prerequisites are not verified (i.e., the tool is not
      # really "standalone")
      #
      set(external_prereqs "")
      foreach(p ${prereqs})
        set(p_type "")
        gp_file_type("${f}" "${p}" p_type)
        if (NOT "${p_type}" STREQUAL "embedded" AND NOT "${p_type}" STREQUAL "system")
          set(external_prereqs ${external_prereqs} "${p}")
        endif (NOT "${p_type}" STREQUAL "embedded" AND NOT "${p_type}" STREQUAL "system")
      endforeach(p)

      if(external_prereqs)
        # Found non-system/non-embedded prerequisites:
        set(result 0)
        set(info ${info} "non-system/non-embedded prerequisites found:\nf='${f}'\nexternal_prereqs='${external_prereqs}'\n")
      endif(external_prereqs)
    endif(is_executable)
  endforeach(f)

  if(result)
    set(info "Verified ${count} executable files in '${tool}'")
  endif(result)

  set(${result_var} "${result}" PARENT_SCOPE)
  set(${info_var} "${info}" PARENT_SCOPE)
endfunction(verify_tool_prerequisites)


# verify_tool_symlinks
#
# Verifies that any symlinks found in the tool point to other files that are
# already also in the tool... Anything that points to an external file causes
# this function to fail the verification.
#
function(verify_tool_symlinks tool result_var info_var)
  set(result 1)
  set(info "")
  set(count 0)

  # TODO: implement this function for real...
  # Right now, it is just a stub that verifies unconditionally...

  set(${result_var} "${result}" PARENT_SCOPE)
  set(${info_var} "${info}" PARENT_SCOPE)
endfunction(verify_tool_symlinks)


# verify_app
#
# Verifies that an application appears valid based on running analysis tools on it.
# Calls message/FATAL_ERROR if the application is not verified.
#
function(verify_app app)
  set(verified 0)
  set(info "")

  get_tool_and_executable("${app}" tool executable valid)

  message(STATUS "===========================================================================")
  message(STATUS "Analyzing app='${app}'")
  message(STATUS "tool='${tool}'")
  message(STATUS "executable='${executable}'")
  message(STATUS "valid='${valid}'")

  # Verify that the tool does not have any "external" prerequisites:
  #
  verify_tool_prerequisites("${tool}" verified info)
  message(STATUS "verified='${verified}'")
  message(STATUS "info='${info}'")
  message(STATUS "")

#  if(verified)
    # Verify that the tool does not have any symlinks to external files:
    #
#    verify_tool_symlinks("${tool}" verified info)
#    message(STATUS "verified='${verified}'")
#    message(STATUS "info='${info}'")
#    message(STATUS "")
#  endif(verified)

  if(NOT verified)
    message(FATAL_ERROR "error: verify_app failed")
  endif(NOT verified)
endfunction(verify_app)

