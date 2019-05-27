### Setup required CMake version
cmake_minimum_required(VERSION 2.6)

### Enable needed languages
ENABLE_LANGUAGE( CXX )

### Project name:
PROJECT(Lights_and_textures)

SET (Lights_and_textures_cpp
Light_and_textures/laba4.cpp
Light_and_textures/readBMP.cpp
)

SET (Lights_and_textures_h
Light_and_textures/GL_movement.h
Light_and_textures/readBMP.h
)

SET (Lights_and_textures_other
CMakeLists.txt
)

SET (Lights_and_textures_images
bar_wall.bmp
common_wall.bmp
song_list.bmp
piano.bmp
key.bmp
)

### Make directory available for include files.

include_directories(BEFORE "Light_and_textures")

### Find the OpenGL libraries.
include(FindOpenGL)
if (NOT OPENGL_FOUND)
    message(SEND_ERROR, "Cannot find OpenGL!!!")
endif()

if(OPENGL_INCLUDE_DIR)
    include_directories(BEFORE "${OPENGL_INCLUDE_DIR}")
endif()

### Find the GLUT library.
include(FindGLUT)
if (NOT GLUT_FOUND)
    message(WARNING, "Cannot find GLUT. Please specify GLUT_INCLUDE_DIR and GLUT_LIBRARIES manually.")
	set(GLUT_INCLUDE_DIR CACHE PATH "Include path for GLUT library")
	set(GLUT_LIBRARIES CACHE FILEPATH "GLUT library file")
endif()

if(GLUT_INCLUDE_DIR)
    include_directories(BEFORE "${GLUT_INCLUDE_DIR}")
endif()

# Targets.

### PARTITION: Customize file groups / subgroups for CMake generators:
source_group("Source Files" FILES ${Lights_and_textures_cpp})
source_group("Header Files" FILES ${Lights_and_textures_h})
source_group("Images for textures" FILES ${Lights_and_textures_images})
source_group("Other Files" FILES ${Lights_and_textures_other})

### Copy texture image files into the binary directory.
FOREACH( SRCFILE IN LISTS Lights_and_textures_images)
	ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${SRCFILE}
	                    COMMAND ${CMAKE_COMMAND}
					    ARGS -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${SRCFILE} ${CMAKE_CURRENT_BINARY_DIR}/${SRCFILE}
					    MAIN_DEPENDENCY ${SRCFILE}
					    COMMENT "Copying the texture file ${SRCFILE}"
					    VERBATIM
	                  )
ENDFOREACH( SRCFILE )

ADD_EXECUTABLE(Lights_and_textures
${Lights_and_textures_cpp}
${Lights_and_textures_h}
${Lights_and_textures_other}
${Lights_and_textures_images}
)

target_link_libraries(Lights_and_textures ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
