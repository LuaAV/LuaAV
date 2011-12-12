# ----------------------------------------------------------------------------
#  CMake file for cvaux. See root CMakeLists.txt
#
# ----------------------------------------------------------------------------
project(cvaux)

file(GLOB aux_srcs *.cpp)
file(GLOB aux_srcs_vs vs/*.cpp)
set(lib_srcs ${aux_srcs_vs} ${aux_srcs})
source_group(Src FILES ${aux_srcs})
source_group(Src\\VideoSurveillance FILES ${aux_srcs_vs})

set(aux_hdr_names cvaux.h cvaux.hpp cvvidsurv.hpp)
set(aux_hdrs_ext)
foreach(h ${aux_hdr_names})
    list(APPEND aux_hdrs_ext "${CMAKE_CURRENT_SOURCE_DIR}/../../include/opencv/${h}")
endforeach()

file(GLOB_RECURSE aux_hdrs_int *.h)
set(lib_hdrs ${lib_hdrs} ${aux_hdrs_ext} ${aux_hdrs_int})

source_group("Include\\External" FILES ${aux_hdrs_ext})
source_group("Include\\Internal" FILES ${aux_hdrs_int})

add_definitions(-DCVAPI_EXPORTS)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../include/opencv"
                    "${CMAKE_CURRENT_SOURCE_DIR}"
                    "${CMAKE_CURRENT_BINARY_DIR}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/../cv" ".")

# ----------------------------------------------------------------------------------
# 				Define the library target:
# ----------------------------------------------------------------------------------
set(the_target "cvaux")

add_library(${the_target} ${lib_srcs} ${lib_hdrs})

if(PCHSupport_FOUND)
    set(pch_header ${CMAKE_CURRENT_SOURCE_DIR}/_cvaux.h)
    if(${CMAKE_GENERATOR} MATCHES "Visual*" OR ${CMAKE_GENERATOR} MATCHES "Xcode*")
        if(${CMAKE_GENERATOR} MATCHES "Visual*")
            set(${the_target}_pch "cvauxprecomp.cpp")
        endif()            
        add_native_precompiled_header(${the_target} ${pch_header})
    elseif(CMAKE_COMPILER_IS_GNUCXX AND ${CMAKE_GENERATOR} MATCHES ".*Makefiles")
        add_precompiled_header(${the_target} ${pch_header})
    endif()
endif()

# For dynamic link numbering convenions
set_target_properties(${the_target} PROPERTIES
	VERSION ${OPENCV_VERSION}
	SOVERSION ${OPENCV_SOVERSION}
	OUTPUT_NAME "${the_target}${OPENCV_DLLVERSION}"
	)

# Additional target properties
set_target_properties(${the_target} PROPERTIES
	DEBUG_POSTFIX "${OPENCV_DEBUG_POSTFIX}"
	ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/"
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/"
	)

# Add the required libraries for linking:
target_link_libraries(${the_target} ${OPENCV_LINKER_LIBS} cxcore cv highgui ml)

if(MSVC AND CMAKE_CROSSCOMPILING)
	set_target_properties(${the_target} PROPERTIES
		LINK_FLAGS "/NODEFAULTLIB:secchk"
		)
endif()

# Dependencies of this target:
add_dependencies(${the_target} cxcore cv highgui ml)

install(TARGETS ${the_target}
	RUNTIME DESTINATION bin COMPONENT main
	LIBRARY DESTINATION lib COMPONENT main
	ARCHIVE DESTINATION lib COMPONENT main)

install(FILES ${aux_hdrs_ext}
        DESTINATION include/opencv
        COMPONENT main)
