# Copyright (C) 2005-2009 MaNGOS project <http://getmangos.com/>

file(GLOB_RECURSE sources *.cpp *.h)

if( UNIX )
  include_directories (
    ${CMAKE_SOURCE_DIR}/src/server/shared
    ${CMAKE_SOURCE_DIR}/dep/libmpq
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/loadlib
  )
elseif( WIN32 )
  include_directories (
    ${CMAKE_SOURCE_DIR}/src/server/shared
    ${CMAKE_SOURCE_DIR}/dep/libmpq
    ${CMAKE_SOURCE_DIR}/dep/libmpq/win
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/loadlib
  )
endif()

add_executable(mapextractor
  ${sources}
)

target_link_libraries(mapextractor
  ${BZIP2_LIBRARIES}
  ${ZLIB_LIBRARIES}
  mpq
)

add_dependencies(mapextractor mpq)

if( UNIX )
  install(TARGETS mapextractor DESTINATION bin)
elseif( WIN32 )
  install(TARGETS mapextractor DESTINATION "${CMAKE_INSTALL_PREFIX}") 
endif()
