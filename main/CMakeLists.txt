#the default component_dirs build var is "IDF_PATH/components"
#more on build system here https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html
#if having unclear issues building first just try by doing a fresh build.

idf_component_register(
    SRC_DIRS "."
    INCLUDE_DIRS "."
)
message(STATUS "Looking at CMakeLists.txt in main directory. Include_dirs= ${INCLUDE_DIRS} \nMain Component name = ${COMPONENT_ALIAS}")