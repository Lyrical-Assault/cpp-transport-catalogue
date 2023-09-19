cmake_minimum_required(VERSION 3.10)
project(Sprint15 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_PREFIX_PATH "/Users/sergey/Protobuf")

find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS transport_catalogue.proto svg.proto map_renderer.proto graph.proto transport_router.proto)

add_executable(Sprint15 ${PROTO_SRCS} ${PROTO_HDRS} main.cpp domain.h geo.cpp geo.h json.cpp json.h json_builder.cpp json_builder.h graph.h json_reader.cpp json_reader.h map_renderer.cpp map_renderer.h ranges.h request_handler.cpp request_handler.h router.h serialization.cpp serialization.h svg.cpp svg.h transport_catalogue.cpp transport_catalogue.h transport_router.cpp transport_router.h transport_catalogue.proto)

target_include_directories(Sprint15 PUBLIC ${Protobuf_INCLUDE_DIRS})
target_include_directories(Sprint15 PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

string(REPLACE "protobuf.lib" "protobufd.lib" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")
string(REPLACE "protobuf.a" "protobufd.a" "Protobuf_LIBRARY_DEBUG" "${Protobuf_LIBRARY_DEBUG}")

target_link_libraries(Sprint15 "$<IF:$<CONFIG:Debug>,${Protobuf_LIBRARY_DEBUG},${Protobuf_LIBRARY}>" Threads::Threads)