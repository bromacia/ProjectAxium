# Copyright (C) 2005-2009 MaNGOS project <http://getmangos.com/>

include_directories(
  ${CMAKE_SOURCE_DIR}/dep/g3dlite/include
  ${CMAKE_SOURCE_DIR}/src/server/shared
  ${CMAKE_SOURCE_DIR}/src/server/shared/Debugging
  ${CMAKE_SOURCE_DIR}/src/server/collision
  ${CMAKE_SOURCE_DIR}/src/server/collision/Maps
  ${CMAKE_SOURCE_DIR}/src/server/collision/Models
  ${ACE_INCLUDE_DIR}
  ${ZLIB_INCLUDE_DIR}
)

add_definitions(-DNO_CORE_FUNCS)
add_executable(vmap4assembler VMapAssembler.cpp)
add_dependencies(vmap4assembler mpq)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  set_target_properties(vmap4assembler PROPERTIES LINK_FLAGS "-framework Carbon")
endif()

target_link_libraries(vmap4assembler
  collision
  g3dlib
  ${ZLIB_LIBRARIES}
)

if( UNIX )
  install(TARGETS vmap4assembler DESTINATION bin)
elseif( WIN32 )
  install(TARGETS vmap4assembler DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()
