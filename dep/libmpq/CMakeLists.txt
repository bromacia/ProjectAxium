
file(GLOB sources_mpq libmpq/*.c libmpq/*.h)

set(mpq_STAT_SRCS
  ${sources_mpq}
)

if( UNIX )
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/dep/zlib
    ${CMAKE_SOURCE_DIR}/dep/bzip2
  )
elseif( WIN32 )
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/win
    ${CMAKE_SOURCE_DIR}/dep/zlib
    ${CMAKE_SOURCE_DIR}/dep/bzip2
  )
endif()

add_library(mpq STATIC ${mpq_STAT_SRCS})

set_target_properties(mpq PROPERTIES LINKER_LANGUAGE CXX)
