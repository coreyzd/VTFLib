# KDE (Dolphin etc.) thumbnail plugin for .vtf files.

find_package(Qt6 REQUIRED COMPONENTS Core Gui)

add_library(vtfthumbnail MODULE
	vtfthumbnail.cpp
	VtfThumbnail.cpp
	VtfThumbnail.h
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
	vtflib
	KF6::KIOGui
	Qt6::Gui
)

# KIO only scans Qt's own plugin directory, not one under /usr/local,
# so install there by default (e.g. /usr/lib64/qt6/plugins/kf6/thumbcreator).
set(VTFLIB_KDE_THUMBNAILER_DIR "${QT6_INSTALL_PREFIX}/${QT6_INSTALL_PLUGINS}/kf6/thumbcreator"
	CACHE PATH "Install directory for the KDE .vtf thumbnail plugin")

install(TARGETS vtfthumbnail LIBRARY DESTINATION "${VTFLIB_KDE_THUMBNAILER_DIR}")
