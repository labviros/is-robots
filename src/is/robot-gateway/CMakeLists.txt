include(GNUInstallDirs)

set(target "service.bin") 

find_package(Protobuf REQUIRED)
find_package(is-msgs REQUIRED)
find_package(is-wire REQUIRED)
find_package(zipkin-cpp-opentracing REQUIRED)
find_package(zlib REQUIRED)

get_target_property(Protobuf_IMPORT_DIRS is-msgs::is-msgs INTERFACE_INCLUDE_DIRECTORIES)
set(PROTOBUF_GENERATE_CPP_APPEND_PATH OFF)
PROTOBUF_GENERATE_CPP(options_src options_hdr conf/options.proto)

#######
####
#######

add_executable(${target} 
  "service.cpp"
  "robot-gateway.cpp"
  "robot-gateway.hpp"
  ${options_src}
  ${options_hdr}
)

# compile options
set_property(TARGET ${target} PROPERTY CXX_STANDARD 14)

# link dependencies
target_link_libraries(
  ${target}
 PUBLIC
  is-msgs::is-msgs
  is-wire::is-wire
  is-robot-drivers::is-robot-drivers-aria
  zipkin-cpp-opentracing::zipkin-cpp-opentracing
  zlib::zlib
)

# header dependencies
target_include_directories(
  ${target}
 PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../..> # for headers when building
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> # for generated files in build mode
  $<INSTALL_INTERFACE:include/${include_dir}> # for clients in install mode
)
