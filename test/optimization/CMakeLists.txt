cmake_minimum_required (VERSION 2.6)
project (maplib)

include_directories(SYSTEM "${Boost_INCLUDE_DIRS}")
include_directories("${PROJECT_SOURCE_DIR}/src/"
                    "${PROJECT_SOURCE_DIR}/test/")
link_directories(${Boost_LIBRARIES})

# Executables
if (Boost_FOUND)
  add_executable(exact exact.cpp)
  target_link_libraries(exact ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES})
  add_executable(test_functions test_functions.cpp)
  target_link_libraries(test_functions ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES})
endif (Boost_FOUND)
