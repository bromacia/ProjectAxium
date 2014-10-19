set(Recast_STAT_SRCS
    Recast.cpp 
    RecastAlloc.cpp 
    RecastArea.cpp 
    RecastContour.cpp 
    RecastFilter.cpp 
    RecastMesh.cpp 
    RecastMeshDetail.cpp 
    RecastRasterization.cpp 
    RecastRegion.cpp 
)

if(WIN32)
  include_directories(
    ${CMAKE_SOURCE_DIR}/dep/zlib
  )
endif()

add_library(Recast STATIC ${Recast_STAT_SRCS})

target_link_libraries(Recast ${ZLIB_LIBRARIES})