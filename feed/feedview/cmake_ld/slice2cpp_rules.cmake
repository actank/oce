#
# This file contains a set of macros used for generating 
# C++ source from Slice files
#
# Author: Alex Brooks
#

#
# Generate rules for SLICE->C++ files generation, for each
# of the named slice source files.
#
# GENERATE_SLICE2CPP_RULES( generated_cpp_list generated_header_list SRC1 [SRC2 [SRC3...]] )
# 
# Returns lists of all the .cpp and .h files that will be generated.
#   (ie modifies the first two arguments: generated_cpp_list and generated_header_list)
#
# Sets up dependencies: 
#  - Each .cpp and .h file depends on all the .ice files.
#  - Each .cpp file depends on all .h files associated with all the previously-listed slice sources.
#
MACRO( GENERATE_SLICE2CPP_RULES slice_source_dir slice2cpp_output_dir generated_cpp_list generated_header_list )

    SET( slice_cpp_suffixes        .cpp )
    SET( slice_header_suffixes     .h  )
    SET( slice_suffixes            ${slice_cpp_suffixes} ${slice_header_suffixes} )
    SET( slice2cpp_command         ${ICE_HOME}/bin/slice2cpp${EXE_EXTENSION} )
    
    SET( slice_args ${SLICE_PROJECT_ARGS} -I${XNS_ROOT_DIR}/UtilSlice/slice -I${slice_source_dir} -I${ICE_SLICE_HOME}/slice --output-dir ${slice2cpp_output_dir} )


  # debug
  # MESSAGE( STATUS "   slice sources    : " ${slice_source_dir} )
  # MESSAGE( STATUS "   cpp destination  : " ${slice2cpp_output_dir} )
  # MESSAGE( STATUS "GENERATE_SLICE2CPP_RULES: ARGN: ${ARGN}" )

  #
  # First pass: Loop through the SLICE sources we were given, add the full path for dependencies
  #
  #FOREACH( slice_source_basename ${ARGN} )
  #  APPEND( all_slice_depends "${slice_source_dir}/${slice_source_basename}" )
  #ENDFOREACH( slice_source_basename ${ARGN} )

  #
  # Second pass: Loop through the SLICE sources we were given, add the CMake rules
  #
  SET( slice_source_counter 0 )
  FOREACH( slice_source_basename ${ARGN} )
    SET( slice_source "${slice_source_dir}/${slice_source_basename}" )
    #MESSAGE( STATUS "DEBUG: Dealing with ${slice_source_basename}")

    #
    # Add a custom cmake command to generate each type of output file: headers and source
    #
    FOREACH ( suffix ${slice_suffixes} )

      # OUTPUT is the target we're generating rules for.
      STRING( REGEX REPLACE "\\.ice" ${suffix} output_basename "${slice_source_basename}" )
      SET( output_fullname "${slice2cpp_output_dir}/${output_basename}" )
      APPEND( ADDITIONAL_MAKE_CLEAN_FILES ${output_fullname} )

      #
      # Make each .h and .cpp file depend on _every_ slice source.  This means that if you 
      # change any .ice file everything will be recompiled. We want this because slice files
      # can include each other and if the included file has changed the downstream file needs
      # to processed again. Unfortunately CMake can't track dependencies between slice files
      # so it's safer to make them all depend on each other.
      #
      #SET( all_depends ${all_slice_depends} )

      #
      # For .cpp files (not .h files) add extra dependencies.
      # Let CMake know that it has to generate all the .h/.cpp files in the order listed.
      # To enforce this, make each .cpp file depend on _all_ the previously-listed .h/.cpp files.
      #
      #IF( ${suffix} STREQUAL ".cpp" )
      #  SET( all_depends ${all_depends} ${headers_so_far} ${bodies_so_far} )
      #  STRING( REGEX REPLACE "\\.cpp" ".h" associated_header "${output_fullname}" )
      #  APPEND( headers_so_far ${associated_header} )
      #  APPEND( bodies_so_far  ${output_fullname} )
      #ENDIF( ${suffix} STREQUAL ".cpp" )

      #
      # Add this output to the list of generated files
      #
      IF( ${suffix} STREQUAL ".cpp" )
        APPEND( ${generated_cpp_list} ${slice2cpp_output_dir}/${output_basename} )
        SET( generated_file_type "source" )
      ELSE( ${suffix} STREQUAL ".cpp" )
        APPEND( ${generated_header_list} ${slice2cpp_output_dir}/${output_basename} )
        SET( generated_file_type "header" )
      ENDIF( ${suffix} STREQUAL ".cpp" )

      #
      # Add the command to generate file.xxx from file.ice
      # Note: when the 'output' is needed, the 'command' will be called with the 'args'
      #
      #MESSAGE( STATUS "DEBUG: Adding rule for generating ${output_basename} from ${slice_source} ${slice_source_basename} depends ${slice_source}" )
      ADD_CUSTOM_COMMAND(
        OUTPUT  ${slice2cpp_output_dir}/${output_basename}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${slice2cpp_output_dir}
        COMMAND ${slice2cpp_command} ${slice_args} ${slice_source}
        #DEPENDS ${all_depends}
		DEPENDS ${slice_source}
        COMMENT "-- Generating ${generated_file_type} file from ${slice_source}"
        VERBATIM
        )

    ENDFOREACH ( suffix ${slice_suffixes} )

    MATH( EXPR slice_source_counter "${slice_source_counter} + 1" )

  ENDFOREACH( slice_source_basename )

  # alexm:
  # as of cmake v.2.2, we don't need to explicitly set the generated files to be 'generated'.
  # see cmake FAQ entry:
  # http://www.cmake.org/Wiki/CMake_FAQ#Running_.22make_clean.22_does_not_remove_custom_command_outputs._Why.3F
  # one small problem: the last header file is not deleted on 'make clean' for some reason. don't know why?

#   MESSAGE( STATUS "DEBUG: generated_cpp_list: ${${generated_cpp_list}}")
#   MESSAGE( STATUS "DEBUG: generated_header_list: ${${generated_header_list}}")

  MESSAGE( STATUS "Will generate cpp header and source files from ${slice_source_counter} Slice definitions using this command:" )
  MESSAGE( STATUS "${slice2cpp_command} <source.ice> ${slice_args}" )

ENDMACRO( GENERATE_SLICE2CPP_RULES generated_cpp_list generated_header_list )
