idf_component_register(SRCS "WiFi.c"
                    REQUIRES nvs_flash esp_wifi FFT protocol_examples_common
                    INCLUDE_DIRS "include") # ${EXTRA_COMPONENT_DIRS}