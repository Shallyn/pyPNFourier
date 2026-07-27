# files
set(DIR_LIB_SRCS 
	pnFourier_utils.c
	pnFourier_Jpqa0.c
	pnFourier_Jpqab.c
	pnFourier_Kpqab.c
	pnFourier.c)

# set dy-library
add_library (pnFourier SHARED ${DIR_LIB_SRCS})
add_library (proj::pnFourier ALIAS pnFourier)
target_include_directories(pnFourier PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")

find_package(GSL REQUIRED)
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(FFTW3 IMPORTED_TARGET fftw3)
endif()
if(NOT TARGET PkgConfig::FFTW3)
    find_path(FFTW3_INCLUDE_DIR fftw3.h)
    find_library(FFTW3_LIBRARY NAMES fftw3 REQUIRED)
    if(NOT FFTW3_INCLUDE_DIR)
        message(FATAL_ERROR "Could not find fftw3.h")
    endif()
endif()

target_link_libraries(
    pnFourier PRIVATE basic GSL::gsl GSL::gslcblas project_options
)
if(TARGET PkgConfig::FFTW3)
    target_link_libraries(pnFourier PRIVATE PkgConfig::FFTW3)
else()
    target_include_directories(pnFourier PRIVATE "${FFTW3_INCLUDE_DIR}")
    target_link_libraries(pnFourier PRIVATE "${FFTW3_LIBRARY}")
endif()

if(APPLE)
    set_target_properties(pnFourier PROPERTIES
        BUILD_RPATH "@loader_path/../libbasic"
        INSTALL_RPATH "@loader_path/../libbasic")
elseif(UNIX)
    set_target_properties(pnFourier PROPERTIES
        BUILD_RPATH "$ORIGIN/../libbasic"
        INSTALL_RPATH "$ORIGIN/../libbasic")
endif()

# profect info
# project(MAIN_BASIC)

# target
# add_executable(MAIN_BASIC main.c)
# target_link_libraries(MAIN_BASIC basic)
