# ----------------------------------------------------------------------------
#  CMake file for ml. See root CMakeLists.txt
# ----------------------------------------------------------------------------
project(ml)

file(GLOB lib_srcs "*.cpp")
source_group("Src" FILES ${lib_srcs})
set(lib_hdrs ${CMAKE_CURRENT_SOURCE_DIR}/../../include/opencv/ml.h)
source_group("Include\\External" FILES ${lib_hdrs})
file(GLOB lib_int_hdrs "*.h*")
source_group("Include\\Internal" FILES ${lib_int_hdrs})

add_definitions(-DCVAPI_EXPORTS)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../include/opencv"
                    "${CMAKE_CURRENT_SOURCE_DIR}"
                    "${CMAKE_CURRENT_BINARY_DIR}")

set(the_target "ml")

add_library(${the_target} ${lib_srcs} ${lib_hdrs} ${lib_int_hdrs})

if(PCHSupport_FOUND)
    set(pch_header ${CMAKE_CURRENT_SOURCE_DIR}/_ml.h)
    if(${CMAKE_GENERATOR} MATCHES "Visual*" OR ${CMAKE_GENERATOR} MATCHES "Xcode*")
        if(${CMAKE_GENERATOR} MATCHES "Visual*")
            set(${the_target}_pch "ml.cpp")
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

# Linker flag needed for Windows Mobile 5 and 6 SDKs
if(MSVC AND CMAKE_CROSSCOMPILING)
	set_target_properties(${the_target} PROPERTIES
		LINK_FLAGS "/NODEFAULTLIB:secchk"
		)
endif()

# Add the required libraries for linking:
target_link_libraries(${the_target} ${OPENCV_LINKER_LIBS} cxcore)

# Dependencies of this target:
add_dependencies(${the_target} cxcore)

install(TARGETS ${the_target}
	RUNTIME DESTINATION bin COMPONENT main
	LIBRARY DESTINATION lib COMPONENT main
	ARCHIVE DESTINATION lib COMPONENT main)

install(FILES ${lib_hdrs}
        DESTINATION include/opencv
        COMPONENT main)
