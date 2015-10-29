set(CMAKE_INCLUDE_CURRENT_DIR on)

add_library (tmfont_core STATIC tmfont.cc)

target_link_libraries(tmfont_core ${FREETYPE_LIBRARIES})
