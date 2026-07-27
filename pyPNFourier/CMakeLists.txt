cmake_minimum_required(VERSION 3.15)

# project
project(pyPNFourier LANGUAGES C)

option(USE_OPENMP "Enable multi-threaded code via OpenMP" ON)

# Platform-specific architecture settings
if(APPLE)
    if(NOT CMAKE_OSX_ARCHITECTURES)
        set(CMAKE_OSX_ARCHITECTURES "${CMAKE_SYSTEM_PROCESSOR}" CACHE STRING "" FORCE)
    endif()
endif()

# Create interface library for compiler options
add_library(project_options INTERFACE)

# Cross-platform compiler optimization flags
if(MSVC)
    target_compile_options(project_options INTERFACE /O2 /W3)
    # Add MSVC-specific definitions if needed
    target_compile_definitions(project_options INTERFACE _CRT_SECURE_NO_WARNINGS)
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
    target_compile_options(project_options INTERFACE -O3)
    
    # Additional GCC/Clang warnings
    target_compile_options(project_options INTERFACE -Wall -Wextra)
endif()

# Cross-platform OpenMP detection
if(USE_OPENMP)
    find_package(OpenMP QUIET)
    
    # Platform-specific OpenMP handling
    if(NOT OpenMP_C_FOUND)
        if(APPLE)
            # macOS: Try Homebrew libomp
            message(STATUS "Trying Homebrew OpenMP libraries...")
            
            # Check multiple possible Homebrew prefixes
            set(HOMEBREW_PREFIXES
                "/opt/homebrew/opt/libomp"
                "/usr/local/opt/libomp"
                "/opt/homebrew"
                "/usr/local")
            
            foreach(PREFIX ${HOMEBREW_PREFIXES})
                if(NOT OpenMP_C_FOUND)
                    find_path(OPENMP_INCL_DIR omp.h PATHS "${PREFIX}/include" NO_DEFAULT_PATH)
                    find_library(OPENMP_LIB NAMES omp gomp PATHS "${PREFIX}/lib" NO_DEFAULT_PATH)
                    
                    if(OPENMP_INCL_DIR AND OPENMP_LIB)
                        set(OpenMP_C_FOUND TRUE)
                        set(OpenMP_C_LIBRARIES ${OPENMP_LIB})
                        target_include_directories(project_options INTERFACE ${OPENMP_INCL_DIR})
                        
                        if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
                            set(OpenMP_C_FLAGS "-fopenmp")
                        else()
                            set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp")
                        endif()
                        
                        message(STATUS "Found OpenMP at ${PREFIX}: ${OPENMP_LIB}")
                        break()
                    endif()
                endif()
            endforeach()
            
        elseif(WIN32)
            # Windows: Additional search paths for different compilers
            if(MSVC)
                # MSVC uses different OpenMP implementation
                find_package(OpenMP REQUIRED)
            elseif(MINGW)
                # MinGW might need additional paths
                find_library(OPENMP_LIB NAMES omp gomp pthread 
                           PATHS 
                           "${CMAKE_C_COMPILER}/../lib"
                           "${CMAKE_C_COMPILER}/../../lib")
                if(OPENMP_LIB)
                    set(OpenMP_C_FOUND TRUE)
                    set(OpenMP_C_FLAGS "-fopenmp")
                    set(OpenMP_C_LIBRARIES ${OPENMP_LIB})
                endif()
            endif()
            
        elseif(UNIX)
            # Linux: Try system package manager installations
            find_path(OPENMP_INCL_DIR omp.h PATHS 
                      "/usr/include"
                      "/usr/local/include")
            find_library(OPENMP_LIB NAMES omp gomp pthread PATHS 
                        "/usr/lib" 
                        "/usr/local/lib"
                        "/usr/lib/x86_64-linux-gnu"
                        "/usr/lib64")
            
            if(OPENMP_INCL_DIR AND OPENMP_LIB)
                set(OpenMP_C_FOUND TRUE)
                set(OpenMP_C_FLAGS "-fopenmp")
                set(OpenMP_C_LIBRARIES ${OPENMP_LIB})
                target_include_directories(project_options INTERFACE ${OPENMP_INCL_DIR})
            endif()
        endif()
    endif()
    
    # Apply OpenMP settings if found
    if(OpenMP_C_FOUND)
        if(TARGET OpenMP::OpenMP_C)
            target_link_libraries(project_options INTERFACE OpenMP::OpenMP_C)
        else()
            separate_arguments(
                OPENMP_C_FLAG_LIST NATIVE_COMMAND "${OpenMP_C_FLAGS}"
            )
            target_compile_options(
                project_options INTERFACE ${OPENMP_C_FLAG_LIST}
            )
            target_link_libraries(
                project_options INTERFACE ${OpenMP_C_LIBRARIES}
            )
        endif()
        target_compile_definitions(project_options INTERFACE USE_OPENMP=1)
        message(STATUS "OpenMP enabled → ${OpenMP_C_FLAGS}")
    else()
        message(WARNING "OpenMP NOT found – building serial version.")
        message(STATUS "To install OpenMP:")
        if(APPLE)
            message(STATUS "  macOS: brew install libomp")
        elseif(WIN32)
            message(STATUS "  Windows: Install with your compiler (MSVC/MinGW)")
        else()
            message(STATUS "  Linux: sudo apt-get install libomp-dev (Ubuntu/Debian)")
            message(STATUS "         sudo yum install libgomp-devel (CentOS/RHEL)")
        endif()
    endif()
endif()

# Print configuration summary
message(STATUS "Configuration Summary:")
message(STATUS "  System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "  Compiler ID: ${CMAKE_C_COMPILER_ID}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
if(USE_OPENMP)
    message(STATUS "  OpenMP: ${OpenMP_C_FOUND}")
endif()

# sub directories
add_subdirectory(libbasic)
add_subdirectory(libpnfourier)
