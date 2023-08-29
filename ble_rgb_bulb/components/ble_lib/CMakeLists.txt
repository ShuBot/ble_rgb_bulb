set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/bluetooth/nimble/common/nimble_peripheral_utils)

idf_component_register(SRCS "ble_lib.c" "gatt_svr.c" "misc.c" "scli.c"
                    INCLUDE_DIRS "include"
                    REQUIRES bt console driver nvs_flash)
