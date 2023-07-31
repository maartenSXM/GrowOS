idf_component_register(
  SRCS "globals.cpp"
  INCLUDE_DIRS "."
  PRIV_REQUIRES arduino global_Defines FT6X36 TFT_eSPI Firebase-ESP-Client-main DHT-sensor-library-master Adafruit_BusIO-master Adafruit_Unified_Sensor
)
set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY LINK_INTERFACE_MULTIPLICITY 9)
