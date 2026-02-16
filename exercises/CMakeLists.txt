# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.12)

# set target name
set(TARGET_NAME ProgAlg)
project (${TARGET_NAME})

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED True)

# specify C++ flags
set(DPCPP_FLAGS -Wno-linker-warnings)

# default C++ Flags for warnings and optimization
set(WARNING_FLAGS "-Wall -Wshadow -Wdouble-promotion -fno-common -Winit-self -Wuninitialized -Wmissing-declarations -Woverloaded-virtual")
set(EXTRA_W_FLAGS "-Wcast-align -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs  -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=5") #-Wnoexcept -Wold-style-cast -Wstrict-null-sentinel -switch-default -Wlogical-op
set(DISABLED_WARNINGS "-Wno-c++20-extensions -Wno-inline-namespace-reopened-noninline -Wno-undef -Wno-unused -Wno-unused-command-line-argument")
#set(OPT_FLAGS "-march=native -mtune=native -Ot -O3 -ffast-math -fomit-frame-pointer")
set(OPT_FLAGS "-march=native -mtune=native -O3 -ffast-math -fomit-frame-pointer")

# adding the flags to the targets
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${OPT_FLAGS} ${DISABLED_WARNINGS}")
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${WARNING_FLAGS} ${EXTRA_W_FLAGS} ${DISABLED_WARNINGS} -g -Og")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${WARNING_FLAGS} ${DISABLED_WARNINGS} -g -Og")

# specify source directories
include_directories(${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/include)

# specify output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# find packages
find_package(OpenMP)
find_package(MPI)
find_package(IntelSYCL CONFIG)

# set compiler
if(IntelSYCL_FOUND)
    set(CMAKE_CXX_COMPILER icpx)
endif()

# specify C++ flags
if(OpenMP_CXX_FOUND)
    if(IntelSYCL_FOUND)
        set(OpenMP_CXX_FLAGS -qopenmp)
    else()
        set(OpenMP_CXX_FLAGS -fopenmp)   
    endif()
endif()

# include sub-projects
add_subdirectory(01_C++)
add_subdirectory(06_TaskMapping)
if(OpenMP_CXX_FOUND)
    add_subdirectory(04_OpenMP)
    add_subdirectory(05_Sorting)
endif()
if(MPI_CXX_FOUND)
    add_subdirectory(08_MPI)
    add_subdirectory(09_MPI_Collective)
    add_subdirectory(10_MPI_Sorting)
    if(OpenMP_CXX_FOUND)
        add_subdirectory(11_MPI_Numeric)
    endif()
endif()
if(IntelSYCL_FOUND AND OpenMP_CXX_FOUND)
    add_subdirectory(12_SYCL)
    add_subdirectory(13_ImageProcessing)
endif()


