cmake_minimum_required(VERSION 3.1)

project(KiCadPcbParser)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


set(KICADPCB_HOME ${PROJECT_SOURCE_DIR} )

set (KICADPCB_SRC 
  src/kicadPcbDataBase.cpp
  )

set (KICADPCB_HEADER
  src/kicadPcbDataBase.h
  src/mymath.h
  src/router.h
  )

############################################################
# include directories 
############################################################
include_directories(
  )

############################################################
# Create a library 
############################################################
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${KICADPCB_HOME}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${KICADPCB_HOME}/lib)

add_library( kicadpcbparserlib STATIC
  ${KICADPCB_SRC}
)

set_target_properties( kicadpcbparserlib 
  PROPERTIES OUTPUT_NAME "kicadpcbparser"
)

############################################################
# Create an executable
############################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${KICADPCB_HOME}/bin)

add_executable( kicadpcbparser src/main.cpp )
target_link_libraries( kicadpcbparser
  kicadpcbparserlib
  )

############################################################
# Install  
############################################################

#install(FILES ${PCBROUTER_HEADER} DESTINATION include)
#install(TARGETS pcbrouter DESTINATION bin)
#install(TARGETS pcbrouterlib DESTINATION lib)

