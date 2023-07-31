# the default component_dirs build var is "IDF_PATH/components"
# more on build system here https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html
# if having unclear issues building first just try by doing a fresh build.

file(GLOB_RECURSE MAIN_SOURCE_FILES "*.cpp")

idf_component_register(
    SRCS ${MAIN_SOURCE_FILES}
    INCLUDE_DIRS "."
    PRIV_REQUIRES arduino global_Defines globals FT6X36 TFT_eSPI Firebase-ESP-Client-main DHT-sensor-library-master Adafruit_BusIO-master Adafruit_Unified_Sensor
)
set(EXTRA_COMPONENT_DIRS "components")

# Common dependencies for multiple components
# set(COMMON_DEPENDENCIES arduino global_Defines)
# use by:
# PRIV_REQUIRES "." ${COMMON_DEPENDENCIES}
message(STATUS "Looking at CMakeLists.txt in main directory. Include_dirs= ${INCLUDE_DIRS} \nMain Component name = ${COMPONENT_ALIAS}")