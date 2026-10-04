idf_component_register(
    SRC_DIRS
        "."
        "managers"
        "drivers"

    INCLUDE_DIRS
        "."
        "inc"

    REQUIRES
        freertos
        esp_driver_pcnt
        esp_driver_gpio
        esp_driver_spi
)