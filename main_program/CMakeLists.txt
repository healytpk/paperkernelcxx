set(SRCS
	ai.cpp
	AUTO_GENERATED_wxformbuilder.cpp
	embedded_archive.cpp
	GUI_Dialog_Main.cpp
	paper.cpp
	paperman.cpp
	papertree.cpp
	semantic.cpp
    view_portal.cpp)
set(HEADERS
	ai.hpp
	AUTO_GENERATED_wxformbuilder.h
	Auto.h
	common.hpp
	embedded_archive.hpp
	GUI_Dialog_Main.hpp
	GUI_Dialog_Waiting.hpp
	incbin.h
	paper.hpp
	paperman.hpp
	paper_tree_contents.hpp
	papertree.hpp
	semantic.hpp
    view_portal.hpp)

if(WIN32)
	set(SRCS ${SRCS} main_program.rc)
	set(PREPROCESSOR_DEFINITIONS ${PREPROCESSOR_DEFINITIONS};
		/D_USRDLL;
		/DwxUSE_NO_MANIFEST=1;
		/D__STDC_CONSTANT_MACROS)
endif(WIN32)

set(LIBS ${wxWidgets_LIBRARIES})

set(EXECUTABLE_NAME main_program)

add_definitions(${PREPROCESSOR_DEFINITIONS})
include_directories(${INCLUDE_DIRECTORIES} ${BASE_INCLUDE_DIRECTORIES})
link_directories(${LINK_DIRECTORIES})

if(WIN32)
	set(EXECUTABLE_TYPE WIN32)
endif(WIN32)
if(APPLE)
	set(MACOSX_BUNDLE YES)
	set(EXECUTABLE_TYPE MACOSX_BUNDLE)
	set(CMAKE_INSTALL_PATH "@loader_path")
endif(APPLE)
if(LINUX)
	set(EXECUTABLE_TYPE "")
endif(LINUX)

set(SRCS ${SRCS} ${HEADERS})

set(PROJECT_FILES ${SRCS})
add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_TYPE} ${PROJECT_FILES})

set(EXE_DIR bin)
set(TARGET_LOCATION ${PROJECT_SOURCE_DIR}/${EXE_DIR}${LIB_SUFFIX})
set_target_properties(${EXECUTABLE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TARGET_LOCATION})
if(APPLE)
	set(CMAKE_MACOSX_RPATH 1)
	set(MACOSX_RPATH TRUE)
        set_target_properties(
                ${EXECUTABLE_NAME} PROPERTIES 
                INSTALL_RPATH "@loader_path/../Frameworks")
endif(APPLE)

target_link_libraries(${EXECUTABLE_NAME} ${LIBS})

if(APPLE)
	FOREACH(DEP_LIB ${DEMO_LIBS_SHARED})
		get_filename_component(ABS_ROOT_DIR ${PROJECT_ROOT_DIR} ABSOLUTE)
		set(LIBNAME_FULL "${ABS_ROOT_DIR}/${DEP_LIB}/${OS_BASE_NAME}${LIB_SUFFIX}/$(CONFIGURATION)/lib${DEP_LIB}.dylib")
                add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD
                	COMMAND install_name_tool -change "${LIBNAME_FULL}" "@executable_path/../Frameworks/lib${DEP_LIB}.dylib" $<TARGET_FILE:${EXECUTABLE_NAME}>)
        ENDFOREACH(DEP_LIB)
endif(APPLE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_include_directories(main_program PRIVATE ../build/xapian/build_debug/include/)
	target_link_directories(main_program PRIVATE ../build/xapian/build_debug/.libs/)
	target_link_libraries(main_program libxapian.a)

	find_package(ZLIB REQUIRED)
	target_link_libraries(main_program ZLIB::ZLIB)

	find_package(uuid REQUIRED)
	target_link_libraries(main_program uuid)
else()
	find_package(xapian REQUIRED)
	target_include_directories(main_program PRIVATE ${xapian_INCLUDE_DIRS})
	target_link_libraries(main_program xapian)
	if(APPLE)
		execute_process(COMMAND brew --prefix xapian OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE XAPIAN_PREFIX)
		target_include_directories(main_program PRIVATE ${XAPIAN_PREFIX}/include)
		target_link_directories(main_program PRIVATE ${XAPIAN_PREFIX}/lib)
	endif(APPLE)
endif()

find_package(archive REQUIRED)
target_include_directories(main_program PRIVATE ${archive_INCLUDE_DIRS})
target_link_libraries(main_program archive)

if(APPLE)
execute_process(COMMAND brew --prefix libarchive OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE LIBARCHIVE_PREFIX)
target_include_directories(main_program PRIVATE ${LIBARCHIVE_PREFIX}/include)
target_link_directories(main_program PRIVATE ${LIBARCHIVE_PREFIX}/lib)
endif(APPLE)

if(LINUX)
target_link_libraries(main_program wx_gtk3u_webview-3.2)
endif(LINUX)
if(APPLE)
target_link_libraries(main_program wx_osx_cocoau_webview-3.2)
endif(APPLE)
if(WIN32)
target_link_libraries(main_program wxmsw32u_webview)
endif(WIN32)
