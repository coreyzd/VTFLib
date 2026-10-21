# .vtf thumbnailers for Linux file managers. Both share one decoder.
#   vtf-thumbnailer   freedesktop thumbnailer (Nautilus, Nemo, Caja, Thunar, ...)
#   vtfthumbnail.so   KDE KIO plugin (Dolphin, ...)

find_package(Qt6 QUIET COMPONENTS Core Gui)
if(NOT Qt6Gui_FOUND)
	message(STATUS "Qt6 Gui not found: skipping .vtf thumbnailers")
	return()
endif()

add_library(vtfthumbnail_core STATIC
	VtfThumbnail.cpp
	VtfThumbnail.h
)
set_target_properties(vtfthumbnail_core PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(vtfthumbnail_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(vtfthumbnail_core PUBLIC vtflib Qt6::Gui)

# ---------------------------------------------------------------------------
# freedesktop thumbnailer
# ---------------------------------------------------------------------------

if(VTFLIB_BUILD_THUMBNAILER)
	message(STATUS "Building vtf-thumbnailer (Nautilus, Nemo, Thunar, ...)")

	add_executable(vtf-thumbnailer main.cpp)
	target_link_libraries(vtf-thumbnailer PRIVATE vtfthumbnail_core)

	install(TARGETS vtf-thumbnailer RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

	# Absolute path: file managers don't always run thumbnailers with the
	# user's PATH (GNOME runs them in a sandbox).
	set(VTF_THUMBNAILER_PATH "${CMAKE_INSTALL_FULL_BINDIR}/vtf-thumbnailer")
	configure_file(vtf.thumbnailer.in "${CMAKE_CURRENT_BINARY_DIR}/vtf.thumbnailer" @ONLY)
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/vtf.thumbnailer"
		DESTINATION "${CMAKE_INSTALL_DATADIR}/thumbnailers"
	)
endif()

# ---------------------------------------------------------------------------
# KDE KIO plugin
# ---------------------------------------------------------------------------

if(VTFLIB_BUILD_KDE_THUMBNAILER)
	find_package(KF6KIO QUIET)
	if(NOT KF6KIO_FOUND)
		message(STATUS "KF6 KIO not found: skipping KDE .vtf thumbnail plugin")
		return()
	endif()

	message(STATUS "KF6 KIO found: building KDE .vtf thumbnail plugin")

	add_library(vtfthumbnail MODULE
		vtfthumbnail.cpp
		vtfthumbnail.json
	)

	set_target_properties(vtfthumbnail PROPERTIES
		PREFIX ""
		AUTOMOC ON
		# Installed into Qt's plugin dir, outside our prefix, so it needs an
		# absolute path to find libVTFLib++.so.
		INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}"
	)

	target_link_libraries(vtfthumbnail PRIVATE
		vtfthumbnail_core
		KF6::KIOGui
	)

	# KIO only scans Qt's own plugin directory, not one under /usr/local,
	# so install there by default (e.g. /usr/lib64/qt6/plugins/kf6/thumbcreator).
	set(VTFLIB_KDE_THUMBNAILER_DIR "${QT6_INSTALL_PREFIX}/${QT6_INSTALL_PLUGINS}/kf6/thumbcreator"
		CACHE PATH "Install directory for the KDE .vtf thumbnail plugin")

	install(TARGETS vtfthumbnail LIBRARY DESTINATION "${VTFLIB_KDE_THUMBNAILER_DIR}")
endif()
