
file(GLOB sources *.c *.h)

set(mongoose_STAT_SRCS
   ${sources}
)

include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(mongoose STATIC ${mongoose_STAT_SRCS})
