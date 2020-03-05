set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "light_manager.c" "led.c" "main.c" "mqtt_client.c" "nvs_config.c" "web_interface.c" "wlan.c" ${COMPONENT_PATH}/git_revision.h)
set(COMPONENT_ADD_INCLUDEDIRS "")

set(COMPONENT_EMBED_TXTFILES "html/info_page.html")

register_component()

add_custom_command(OUTPUT ${COMPONENT_PATH}/git_revision.h
    COMMAND ${COMPONENT_PATH}/../scripts/make-git-revision.sh ${COMPONENT_PATH}/git_revision.h
    VERBATIM)
