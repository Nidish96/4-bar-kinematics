cmake_minimum_required(VERSION 3.5)

include_directories("./" ${GLUT_INCLUDE_DIRS})

find_package( GLUT REQUIRED )

add_executable(4bar Main.c Header.c Graphics.c)
target_link_libraries(4bar m ${GLUT_LIBRARIES} GL GLU)
