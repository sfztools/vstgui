##########################################################################################
# VSTGUI drawing Example Application
##########################################################################################
set(target drawing)

set(${target}_sources
  source/drawing.cpp
  source/drawing.h
  source/drawingarea.cpp
  source/drawingarea.h
)

set(drawing_resources
  resource/Window.uidesc
)

set(drawing_font
)

##########################################################################################
vstgui_add_executable(${target} "${${target}_sources}" )
vstgui_add_resources(${target} "${drawing_resources}")
vstgui_add_resources(${target} "${drawing_font}" "Fonts")
vstgui_set_target_infoplist(${target} "resource/Info.plist")
vstgui_set_target_rcfile(${target} "resource/drawing.rc")
vstgui_set_cxx_version(${target} 14)
target_include_directories(${target} PRIVATE ../../../../)
set_target_properties(${target} PROPERTIES ${APP_PROPERTIES} ${VSTGUI_STANDALONE_EXAMPLES_FOLDER})

