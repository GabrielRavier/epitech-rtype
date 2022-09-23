cmake_minimum_required(VERSION 3.12)

project(rtype CXX)

include(FetchContent)

FetchContent_Declare(
  sfml
  URL "https://github.com/SFML/SFML/archive/2.5.1.zip"
)

FetchContent_GetProperties(sfml)
if(NOT sfml_POPULATED)
  FetchContent_Populate(sfml)
  add_subdirectory(${sfml_SOURCE_DIR} ${sfml_BINARY_DIR})
endif()

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
  "src/client/core/WindowManager.cpp"
  "src/client/systems/BackgroundSystem.cpp"
  "src/client/systems/PhysicsSystem.cpp"
  "src/client/systems/RenderSystem.cpp"
  "src/client/utils/Vec2.cpp"
)
target_link_libraries(r-type_client PRIVATE sfml-graphics)
target_link_libraries(r-type_client PRIVATE sfml-system)
target_link_libraries(r-type_client PRIVATE sfml-window)
rtype_set_common_target_properties(r-type_client)
