idf_component_register(
    SRC_DIRS "."
    INCLUDE_DIRS "."
    REQUIRES Adafruit-GFX-Library-master
)
set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY LINK_INTERFACE_MULTIPLICITY 4)
