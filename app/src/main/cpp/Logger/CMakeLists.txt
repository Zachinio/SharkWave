project(Logger)

file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/src/*.c*)

add_library(${PROJECT_NAME} ${sources})
add_library(sub::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)



