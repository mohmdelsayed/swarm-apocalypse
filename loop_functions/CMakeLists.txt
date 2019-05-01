include_directories(${CMAKE_SOURCE_DIR})

# If Qt+OpenGL dependencies were found, descend into these directories
if(ARGOS_COMPILE_QTOPENGL)
  add_subdirectory(id_loop_functions_eyebot)
endif(ARGOS_COMPILE_QTOPENGL)
