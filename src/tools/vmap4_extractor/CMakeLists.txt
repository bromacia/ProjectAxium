# Copyright (C) 2005-2009 MaNGOS project <http://getmangos.com/>

file(GLOB_RECURSE sources *.cpp *.h)

# uncomment next line to disable debug mode
add_definitions("-DIOMAP_DEBUG")

# build setup currently only supports libmpq 0.4.x
add_definitions("-DUSE_LIBMPQ04")
add_definitions("-Wall")
add_definitions("-ggdb")
add_definitions("-O3")

if( UNIX )
  include_directories(
    ${CMAKE_SOURCE_DIR}/dep/libmpq
  )
elseif( WIN32 )
  include_directories(
    ${CMAKE_SOURCE_DIR}/dep/libmpq
    ${CMAKE_SOURCE_DIR}/dep/libmpq/win
  )
endif()

add_executable(vmap4extractor ${sources})

target_link_libraries(vmap4extractor
  ${BZIP2_LIBRARIES}
  ${ZLIB_LIBRARIES}
  mpq
)

add_dependencies(vmap4extractor mpq)

if( UNIX )
  install(TARGETS vmap4extractor DESTINATION bin)
elseif( WIN32 )
  install(TARGETS vmap4extractor DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()
