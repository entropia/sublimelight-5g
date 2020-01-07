set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "main.c" "web_interface.c" "wlan.c" "nvs_config.c")
set(COMPONENT_ADD_INCLUDEDIRS "")

set(COMPONENT_EMBED_TXTFILES "html/info_page.html")

register_component()

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/git-revision.h
    COMMAND ${COMPONENT_PATH}/../scripts/make-git-revision.sh ${CMAKE_CURRENT_BINARY_DIR}/git-revision.h
    VERBATIM)
