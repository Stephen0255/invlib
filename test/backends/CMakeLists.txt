cmake_minimum_required (VERSION 2.6)

include_directories(SYSTEM "${Boost_INCLUDE_DIRS}")
include_directories("${PROJECT_SOURCE_DIR}/src/"
                    "${PROJECT_SOURCE_DIR}/test/"
                    /opt/intel/mkl/include)

link_directories(${Boost_LIBRARIES})

include(FindCUDA)
include(FindMKL)

if (CUDA_FOUND)
  add_definitions(-DCUDA)
endif(CUDA_FOUND)

if (MKL_FOUND)
  add_definitions(-DMKL)
endif(MKL_FOUND)

# Executables
if (CUDA_FOUND AND Boost_FOUND)
  cuda_add_executable(backends backends.cpp)
  target_link_libraries(backends cuda_kernels
                        ${MKL_LIBRARIES}
                        ${CUDA_CUBLAS_LIBRARIES}
                        ${CUDA_cusparse_LIBRARY})
endif (CUDA_FOUND AND Boost_FOUND)
