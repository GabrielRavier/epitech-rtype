cmake_minimum_required(VERSION 3.12)

project(rtype CXX)

function(rtype_set_common_target_properties target_name)
  set_target_properties(${target_name}
    PROPERTIES
      CXX_STANDARD 17
      CXX_STANDARD_REQUIRED ON
      CXX_EXTENSIONS OFF
  )

  target_include_directories(${target_name}
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
  )
endfunction()


add_executable(r-type_server
  "src/server/main.cpp"
)
rtype_set_common_target_properties(r-type_server)

add_executable(r-type_client
  "src/client/main.cpp"
)
rtype_set_common_target_properties(r-type_client)
