cmake_minimum_required(VERSION 3.12)

project(rtype CXX)

include(CTest)

function(rtype_set_common_target_properties target_name is_top_directory)
  if (${is_top_directory})
    set_target_properties(${target_name}
      PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF

        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/"
    )
  else()
    set_target_properties(${target_name}
      PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
    )
  endif()

  target_include_directories(${target_name}
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
  )
endfunction()

find_package(SFML 2.5
  COMPONENTS
    system window graphics network audio REQUIRED
)

# I do not comprehend why vcpkg is setup like it is but it makes it so we have to do this cursed stuff depending on whether this is a Windows build
if (WIN32)
  find_path(LOGURU_INCLUDE_DIRS "loguru/loguru.hpp")
else()
  find_package(loguru CONFIG REQUIRED)
endif()

add_executable(r-type_server
  "src/server/main.cpp"
)
rtype_set_common_target_properties(r-type_server ON)

# Go complain to whoever set up vcpkg like it is if you think this is bad
set(r_type_client_SOURCES
  "src/client/main.cpp"
  "src/client/core/WindowManager.cpp"
  "src/client/systems/MovementSystem.cpp"
  "src/client/systems/BackgroundSystem.cpp"
  "src/client/systems/PhysicsSystem.cpp"
  "src/client/systems/PlayerSystem.cpp"
  "src/client/systems/RenderSystem.cpp"
  "src/client/utils/Vec2.cpp"
)
if (WIN32)
  list(APPEND r_type_client_SOURCES "${LOGURU_INCLUDE_DIRS}/loguru/loguru.cpp")
endif()

add_executable(r-type_client "${r_type_client_SOURCES}")
rtype_set_common_target_properties(r-type_client ON)
target_link_libraries(r-type_client PRIVATE sfml-graphics)
target_link_libraries(r-type_client PRIVATE sfml-system)
target_link_libraries(r-type_client PRIVATE sfml-window)
if (WIN32)
  target_include_directories(r-type_client PRIVATE "${LOGURU_INCLUDE_DIRS}/loguru")
else()
  target_link_libraries(r-type_client PRIVATE loguru)
endif()

add_executable(basic-ecs-test
  "src/tests/basic-ecs.cpp"
)
rtype_set_common_target_properties(basic-ecs-test OFF)

add_test(NAME basic-ecs-test COMMAND basic-ecs-test)
