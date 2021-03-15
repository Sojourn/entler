add_subdirectory(behavior_tree)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

set(IMGUI_HEADER_FILES
    ${IMGUI_DIR}/imconfig.h
    ${IMGUI_DIR}/imgui.h
    ${IMGUI_DIR}/imgui_internal.h
    ${IMGUI_DIR}/imstb_rectpack.h
    ${IMGUI_DIR}/imstb_textedit.h
    ${IMGUI_DIR}/imstb_truetype.h
    ${IMGUI_DIR}/backends/imgui_impl_dx12.h
)

set(IMGUI_SOURCE_FILES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_dx12.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCE_FILES} ${IMGUI_HEADER_FILES})

target_include_directories(imgui PUBLIC $<BUILD_INTERFACE:${imgui_DIR}>)
