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

find_package(Boost REQUIRED COMPONENTS thread)

# I do not comprehend why vcpkg is setup like it is but it makes it so we have to do this cursed stuff depending on whether this is a Windows build
if (WIN32)
  find_path(LOGURU_INCLUDE_DIRS "loguru/loguru.hpp")
else()
  find_package(loguru CONFIG REQUIRED)
endif()

# Go complain to whoever set up vcpkg like it is if you think this is bad
set(r_type_server_SOURCES
  "src/server/main.cpp"
  "src/server/utils.cpp"
  "src/server/core/NetworkClientManager.cpp"
  "src/server/core/NetworkServerManager.cpp"
  "src/server/systems/MovementSystem.cpp"
  "src/server/systems/ProjectileSystem.cpp"
  "src/server/systems/WaveSystem.cpp"
  "src/server/systems/WeaponSystem.cpp"
  "src/server/packets/connection_state.cpp"
  "src/server/packets/packet_client_input.cpp"
  "src/server/packets/packet_client_keep_alive.cpp"
  "src/server/packets/packet_client_login.cpp"
  "src/server/packets/packet_client_logout.cpp"
  "src/server/packets/packet_client_pos.cpp"
  "src/server/packets/packet_server_entity_create.cpp"
  "src/server/packets/packet_server_entity_destroy.cpp"
  "src/server/packets/packet_server_keep_alive.cpp"
  "src/server/packets/packet_server_keep_alive.cpp"
  "src/server/packets/packet_server_login.cpp"
  "src/server/packets/packet_server_update_health.cpp"
  "src/server/packets/packet_server_update_pos.cpp"
  "src/server/packets/packet_server_update_score.cpp"
)

add_executable(r-type_server "${r_type_server_SOURCES}")
rtype_set_common_target_properties(r-type_server ON)
target_link_libraries(r-type_server PRIVATE Boost::boost Boost::thread)

# Go complain to whoever set up vcpkg like it is if you think this is bad
set(r_type_client_SOURCES
  "src/client/main.cpp"
  "src/client/core/NetworkManager.cpp"
  "src/client/core/WindowManager.cpp"
  "src/client/systems/MovementSystem.cpp"
  "src/client/systems/BackgroundSystem.cpp"
  "src/client/systems/PhysicsSystem.cpp"
  "src/client/systems/PlayerSystem.cpp"
  "src/client/systems/RenderSystem.cpp"
  "src/client/systems/ObjectsSystem.cpp"
  "src/client/utils/Vec2.cpp"
  "src/server/utils.cpp"
  "src/server/packets/connection_state.cpp"
  "src/server/packets/packet_client_input.cpp"
  "src/server/packets/packet_client_keep_alive.cpp"
  "src/server/packets/packet_client_login.cpp"
  "src/server/packets/packet_client_logout.cpp"
  "src/server/packets/packet_client_pos.cpp"
  "src/server/packets/packet_server_entity_create.cpp"
  "src/server/packets/packet_server_entity_destroy.cpp"
  "src/server/packets/packet_server_keep_alive.cpp"
  "src/server/packets/packet_server_keep_alive.cpp"
  "src/server/packets/packet_server_login.cpp"
  "src/server/packets/packet_server_update_health.cpp"
  "src/server/packets/packet_server_update_pos.cpp"
  "src/server/packets/packet_server_update_score.cpp"
)
if (WIN32)
  list(APPEND r_type_client_SOURCES "${LOGURU_INCLUDE_DIRS}/loguru/loguru.cpp")
endif()

add_executable(r-type_client "${r_type_client_SOURCES}")
rtype_set_common_target_properties(r-type_client ON)
target_link_libraries(r-type_client PRIVATE Boost::boost Boost::thread)
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

# This RUNTIME_DEPENDENCIES+EXCLUDE_REGEXES mess is done so that DLL dependencies from Windows are copied
# We filter out .so so that we don't uselessly try to copy dependencies on Linux
list(APPEND RTYPE_INSTALL_PRE_EXCLUDE_REGEXES "api-ms-win-.*")
list(APPEND RTYPE_INSTALL_PRE_EXCLUDE_REGEXES "ext-ms-win.*")
list(APPEND RTYPE_INSTALL_PRE_EXCLUDE_REGEXES "ext-ms-onecore-.*")
list(APPEND RTYPE_INSTALL_PRE_EXCLUDE_REGEXES ".*.so")
list(APPEND RTYPE_INSTALL_POST_EXCLUDE_REGEXES ".*WINDOWS[\\/]system32.*")

install(
  TARGETS r-type_client r-type_server
  RUNTIME_DEPENDENCIES
    PRE_EXCLUDE_REGEXES ${RTYPE_INSTALL_PRE_EXCLUDE_REGEXES}
    POST_EXCLUDE_REGEXES ${RTYPE_INSTALL_POST_EXCLUDE_REGEXES}
  RUNTIME FRAMEWORK
  DESTINATION .)
install(DIRECTORY ./assets/ DESTINATION ./assets/)

# Ignore a bunch of files w.r.t. CPack source packaging - a mess to do properly, this probably doesn't work on Windows... but at least it works on my setup on Linux (probably a better way to package the source code itself would be to use git archive...)
set(CPACK_SOURCE_IGNORE_FILES "/CVS/;/\.svn/;\.swp$;\.#;/#;/\.git/;/build/;/vcpkg/downloads/;/vcpkg/packages/;/\.cache;/compile_commands.json$;/.*~$;/r-type_client$;/r-type_server$;/r-type_client.log$")
include(CPack)
