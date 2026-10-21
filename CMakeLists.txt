cmake_minimum_required(VERSION 3.21)

project(VTFLib
	VERSION 2.0.0
	LANGUAGES C CXX
)

# zstd ships hand-written x86-64 assembly for its decompressor
if(NOT MSVC)
	enable_language(ASM)
endif()

include(CMakeDependentOption)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(VTFLIB_BUILD_VTFCMD "Build VTFCmd" ON)
option(VTFLIB_BUILD_FUZZER "Build VTFFuzzer" OFF)
option(VTFLIB_BUILD_VTFEDIT "Build VTFEdit" ON)
if(NOT WIN32)
	option(VTFLIB_BUILD_THUMBNAILER "Build vtf-thumbnailer for Nautilus/Nemo/Thunar etc. (needs Qt 6)" ON)
	option(VTFLIB_BUILD_KDE_THUMBNAILER "Build the KDE/Dolphin .vtf thumbnail plugin (if KF6 KIO is found)" ON)
endif()

foreach(config "" _DEBUG _RELEASE _RELWITHDEBINFO _MINSIZEREL)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY${config} "${CMAKE_BINARY_DIR}/bin")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY${config} "${CMAKE_BINARY_DIR}/bin")
endforeach()

set(VTFLIB_THIRDPARTY "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")

if(NOT WIN32)
	include(GNUInstallDirs)
	# Find libVTFLib++.so / libDevIL.so next to the binaries in build/bin,
	# and in ../lib (or lib64) once installed, without needing LD_LIBRARY_PATH.
	set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
	set(CMAKE_BUILD_RPATH "$ORIGIN")
endif()

add_subdirectory(thirdparty)

add_subdirectory(VTFLib)

if(VTFLIB_BUILD_VTFCMD)
	add_subdirectory(VTFCmd)
endif()

if(VTFLIB_BUILD_VTFEDIT)
	add_subdirectory(VTFEdit)
endif()

if(NOT WIN32 AND (VTFLIB_BUILD_THUMBNAILER OR VTFLIB_BUILD_KDE_THUMBNAILER))
	add_subdirectory(thumbnailer)
endif()

if(NOT WIN32)
	# Set here, after all subprojects, so it matches the final CMAKE_INSTALL_LIBDIR
	# (some bundled deps re-include GNUInstallDirs and can change it).
	set(VTFLIB_INSTALL_RPATH "$ORIGIN;$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
	foreach(target vtflib DevIL vtfcmd vtfedit vtf-thumbnailer)
		if(TARGET ${target})
			set_target_properties(${target} PROPERTIES INSTALL_RPATH "${VTFLIB_INSTALL_RPATH}")
		endif()
	endforeach()

	install(TARGETS vtflib DevIL
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	)
	install(FILES lib/VTFLib.h DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/VTFLib++")
	if(VTFLIB_BUILD_VTFCMD)
		install(TARGETS vtfcmd RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
	endif()
endif()

if(VTFLIB_BUILD_FUZZER)
	add_subdirectory(fuzz)
endif()
