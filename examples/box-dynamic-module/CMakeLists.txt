STRING( REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} )
add_library(${CURRENT_FOLDER} SHARED box.c )
target_link_libraries(${CURRENT_FOLDER} guile)
set_target_properties(${CURRENT_FOLDER} PROPERTIES OUTPUT_NAME box-module)
