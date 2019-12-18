cmake_minimum_required(VERSION 3.1)

project(KiCadPcbParser)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(THREADS_PREFER_PTHREAD_FLAG ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
find_library(SWIG_EXECUTABLE swig)
if(NOT SWIG_EXECUTABLE)
set(SWIG_EXECUTABLE "swig")
endif()

#find_package( PythonInterp 2.7 REQUIRED )
find_package( PythonInterp 3 REQUIRED )
#find_package( PythonLibs 2.7 REQUIRED )
find_package( PythonLibs 3 REQUIRED )
find_package(SWIG REQUIRED)
find_package(Boost REQUIRED)

set(KICADPCB_HOME ${PROJECT_SOURCE_DIR} )

set (KICADPCB_SRC 
  src/kicadPcbDataBase.cpp
  src/kicadParser.cpp
  src/shape.cpp
  src/util.cpp
  src/PcbDB_wrapper.cpp
  )

set (KICADPCB_HEADER
  src/kicadPcbDataBase.h
  src/point.h
  src/net.h
  src/kicadParser.h
  src/tree.h
  src/util.h
  src/shape.h
  src/pin.h
  src/pcbBoost.h
  src/rule.h
  src/module.h
  src/object.h
  )

set (DB_SWIG_FILES 
  src/PcbDB.i
)

############################################################
# include directories 
############################################################
include_directories(
    ${Boost_INCLUDE_DIR}
    ${PYTHON_INCLUDE_DIRS}
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

add_custom_command( OUTPUT ${KICADPCB_HOME}/src/PcbDB_wrapper.cpp
  COMMAND ${SWIG_EXECUTABLE} -python -py3 -c++ -o ${KICADPCB_HOME}/src/PcbDB_wrapper.cpp ${KICADPCB_HOME}/src/PcbDB.i   
  WORKING_DIRECTORY ${KICADPCB_HOME}
  DEPENDS ${DB_SWIG_FILES}
)

add_executable( kicadpcbparser src/main.cpp )
target_link_libraries( kicadpcbparser
  kicadpcbparserlib
  ${PYTHON_INCLUDE_DIRS}
  )

############################################################
# Install  
############################################################

#install(FILES ${PCBROUTER_HEADER} DESTINATION include)
#install(TARGETS pcbrouter DESTINATION bin)
#install(TARGETS pcbrouterlib DESTINATION lib)
