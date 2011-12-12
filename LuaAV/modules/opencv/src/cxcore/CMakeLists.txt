# ----------------------------------------------------------------------------
#  CMake file for cxcore. See root CMakeLists.txt
# ----------------------------------------------------------------------------
project(cxcore)

file(GLOB lib_srcs "*.cpp")
source_group("Src" FILES ${lib_srcs})

add_definitions(-DCVAPI_EXPORTS)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../3rdparty/include"
                    "${CMAKE_CURRENT_SOURCE_DIR}/../../include/opencv"
                    "${CMAKE_CURRENT_BINARY_DIR}")

set(lib_hdr_names cxcore.h cxcore.hpp cxerror.h cxmat.hpp cxmisc.h cxoperations.hpp cxtypes.h cvver.h cvwimage.h cxflann.h cvinternal.h)
set(lib_hdrs)
foreach(h ${lib_hdr_names})
    list(APPEND lib_hdrs "${CMAKE_CURRENT_SOURCE_DIR}/../../include/opencv/${h}")
endforeach()
source_group("Include\\External" FILES ${lib_hdrs})

file(GLOB lib_int_hdrs "*.h*")
source_group("Include\\Internal" FILES ${lib_int_hdrs})

set(the_target "cxcore")

add_library(${the_target} ${lib_srcs} ${lib_hdrs} ${lib_int_hdrs})

if(PCHSupport_FOUND)
    set(pch_header ${CMAKE_CURRENT_SOURCE_DIR}/_cxcore.h)
    if(${CMAKE_GENERATOR} MATCHES "Visual*" OR ${CMAKE_GENERATOR} MATCHES "Xcode*")
        if(${CMAKE_GENERATOR} MATCHES "Visual*")
            set(${the_target}_pch "cxprecomp.cpp")
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
target_link_libraries(${the_target} ${OPENCV_LINKER_LIBS} ${IPP_LIBS} opencv_lapack zlib flann)

# Linker flag needed for Windows Mobile 5 and 6 SDKs
if(MSVC)
    if (CMAKE_CROSSCOMPILING)
        set_target_properties(${the_target} PROPERTIES
            LINK_FLAGS "/NODEFAULTLIB:secchk"
            )
    endif()
    set_target_properties(${the_target} PROPERTIES
        LINK_FLAGS "/NODEFAULTLIB:libc"
        )
endif()

add_dependencies(${the_target} opencv_lapack zlib)

install(TARGETS ${the_target}
    RUNTIME DESTINATION bin COMPONENT main
    LIBRARY DESTINATION lib COMPONENT main
    ARCHIVE DESTINATION lib COMPONENT main)

# install each module headers separately
install(FILES ${lib_hdrs}
        DESTINATION include/opencv
        COMPONENT main)
