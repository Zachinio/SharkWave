project(SharkWave)

file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/src/*.c* ${PROJECT_SOURCE_DIR}/src/*.h*)

set(PROTOBUF_ARGUMENTS protoc -I=${PROJECT_SOURCE_DIR} --cpp_out=${PROJECT_SOURCE_DIR}/src/models/ ${PROJECT_SOURCE_DIR}/main_messages.proto)
execute_process(COMMAND ${PROTOBUF_ARGUMENTS})

add_executable(${PROJECT_NAME} ${sources})

target_link_libraries(${PROJECT_NAME}  sub::Logger )
