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
MACRO( GENERATE_PROTOC_RULES proto_source_dir protoc_output_dir generated_pb_cc_list generated_pb_h_list )

    SET( proto_pb_cc_suffixes    .pb.cc )
    SET( proto_pb_h_suffixes     .pb.h  )
    SET( proto_suffixes            ${proto_pb_cc_suffixes} ${proto_pb_h_suffixes} )
    SET( protoc_command         ${PROTOBUF_PROTOC_EXECUTABLE}${EXE_EXTENSION} )
    
    SET( proto_args -I${proto_source_dir} --cpp_out=${protoc_output_dir} )


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
  SET( proto_source_counter 0 )
  FOREACH( proto_source_basename ${ARGN} )
    SET( proto_source "${proto_source_dir}/${proto_source_basename}" )
    #MESSAGE( STATUS "DEBUG: Dealing with ${slice_source_basename}")

    #
    # Add a custom cmake command to generate each type of output file: headers and source
    #
    FOREACH ( suffix ${proto_suffixes} )

      # OUTPUT is the target we're generating rules for.
      STRING( REGEX REPLACE "\\.proto" ${suffix} output_basename "${proto_source_basename}" )
      SET( output_fullname "${protoc_output_dir}/${output_basename}" )
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
      IF( ${suffix} STREQUAL ".pb.cc" )
        APPEND( ${generated_pb_cc_list} ${protoc_output_dir}/${output_basename} )
        SET( generated_file_type "source" )
      ELSE( ${suffix} STREQUAL ".pb.cc" )
        APPEND( ${generated_pb_h_list} ${protoc_output_dir}/${output_basename} )
        SET( generated_file_type "header" )
      ENDIF( ${suffix} STREQUAL ".pb.cc" )

      #
      # Add the command to generate file.xxx from file.ice
      # Note: when the 'output' is needed, the 'command' will be called with the 'args'
      #
	  #MESSAGE( STATUS "DEBUG: Adding rule for generating ${output_basename} from ${proto_source} ${proto_source_basename} depends ${proto_source}" )
      ADD_CUSTOM_COMMAND(
        OUTPUT  ${protoc_output_dir}/${output_basename}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${protoc_output_dir}
        COMMAND ${protoc_command} ${proto_source} ${proto_args} 
        #DEPENDS ${all_depends}
		DEPENDS ${proto_source}
        COMMENT "-- Generating ${generated_file_type} file from ${proto_source}"
        VERBATIM
        )

    ENDFOREACH ( suffix ${proto_suffixes} )

    MATH( EXPR proto_source_counter "${proto_source_counter} + 1" )

  ENDFOREACH( proto_source_basename )

  # alexm:
  # as of cmake v.2.2, we don't need to explicitly set the generated files to be 'generated'.
  # see cmake FAQ entry:
  # http://www.cmake.org/Wiki/CMake_FAQ#Running_.22make_clean.22_does_not_remove_custom_command_outputs._Why.3F
  # one small problem: the last header file is not deleted on 'make clean' for some reason. don't know why?

#   MESSAGE( STATUS "DEBUG: generated_cpp_list: ${${generated_cpp_list}}")
#   MESSAGE( STATUS "DEBUG: generated_header_list: ${${generated_header_list}}")

  MESSAGE( STATUS "Will generate pb.cc pb.h and source files from ${proto_source_counter} Proto definitions using this command:" )
  MESSAGE( STATUS "${protoc_command} <source.proto> ${proto_args}" )

ENDMACRO( GENERATE_PROTOC_RULES generated_pb_cc_list generated_pb_h_list )
