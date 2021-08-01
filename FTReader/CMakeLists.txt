# CMakeList.txt: progetto CMake per SensorStreamer. Includere l'origine e definire
# qui la logica specifica del progetto.
#
cmake_minimum_required (VERSION 3.8)

set(CMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD 1)

if (NOT DAQmx_DIR)
message("****")
set(DAQmx_DIR "" CACHE STRING "")
endif()

project("FTReader"
        DESCRIPTION "Reader for F/T ATI Sensor")

# Adding the 
find_library(aticonvert
			NAMES aticonvert
			PATHS ${DAQmx_DIR}
			PATH_SUFFIXES lib
			REQUIRED)

find_library(NIDAQmx 
			NAMES NIDAQmx
			PATHS ${DAQmx_DIR}
			PATH_SUFFIXES lib
			REQUIRED)

message( "Found aticonvert in:  ${aticonvert}")
message( "Found NIDAQmx in:  ${NIDAQmx}")

file(GLOB_RECURSE  PR_SOURCES "include/*.h" "include/*.hpp" "src/*.cpp")

add_executable(${PROJECT_NAME} ${PR_SOURCES})


target_include_directories(${PROJECT_NAME} PRIVATE
  include
  ${DAQmx_DIR}
  )


target_link_libraries(${PROJECT_NAME} ${aticonvert} ${NIDAQmx})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(FILES "config.json" DESTINATION bin)
install(FILES "FT27374.cal" DESTINATION bin)


#set(YOUR_LIBRARIES "")
#find_path(DAQmx_INCLUDE_DIR "")

# searching for library file
#find_library(DAQmx_LIBRARY "")

#if (DAQmx_INCLUDE_DIR AND DAQmx_LIBRARY)
    # you may need that if further action in your CMakeLists.txt depends
    # on detecting your library
#    set(DAQmx_FOUND TRUE)

    # you may need that if you want to conditionally compile some parts
    # of your code depending on library availability
#    add_definitions(-DHAVE_DAQmx=1)

    # those two, you really need
#    include_directories(${DAQmx_INCLUDE_DIR})
#    set(YOUR_LIBRARIES ${YOUR_LIBRARIES} ${SIFTGPU_LIBRARY})
#endif ()

#message( "Trace for YOUR_LIBRARIES variable: ${YOUR_LIBRARIES}")
