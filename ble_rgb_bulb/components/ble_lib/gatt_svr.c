/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "bleprph.h"
#include "services/ans/ble_svc_ans.h"
#include "ble_lib.h"
#include "global_functions.h"
//#include "led_control.h"
/*** Maximum number of characteristics with the notify flag ***/
#define MAX_NOTIFY 5

uint8_t wrt_data = 0;
volatile bool bleinterruptTriggered = false;
//int brightness_in = 0; brightness_in
int ble_get_write_data(void)
{
    uint8_t wdata = wrt_data;

    if(bleinterruptTriggered == true)
    {
        printf("bleinterruptTriggered!!! \n");
        bleinterruptTriggered = false;
        return wdata;
    }
    else 
    {
        //printf("NO Trigger. \n");
        return -1;
    }
    //return wrt_data;
}

uint8_t ble_send_write_data(uint8_t x)
{   //const uint8_t *data, uint16_t len
    // if (len <= sizeof(custom_char_value)) {
    //     memcpy(custom_char_value, data, len);
    //     custom_char_len = len;

    //     // Notify subscribers about the updated characteristic value
    //     ble_gatts_chr_updated(ble_conn_handle, your_custom_char_handle);
    // }
    uint8_t data = x;

    //gdata_test = 50;
    
    return data*10;
    // int rc = ble_gattc_write_no_rsp(conn_handle, custom_value_handle_1, &data, sizeof(data), NULL, NULL);
    
    // if (rc != 0) {
    //     printf("Error writing to characteristic: %d", rc);
    //     return rc;
    // }
    
    // printf("Characteristic updated: %d", data);
}

static const ble_uuid128_t gatt_svr_svc_uuid =
    BLE_UUID128_INIT(0x2d, 0x71, 0xa2, 0x59, 0xb4, 0x58, 0xc8, 0x12,
                     0x99, 0x99, 0x43, 0x95, 0x12, 0x2f, 0x46, 0x59);

/* A characteristic that can be subscribed to */
static uint8_t gatt_svr_chr_val;
static uint16_t gatt_svr_chr_val_handle;
static const ble_uuid128_t gatt_svr_chr_uuid =
    BLE_UUID128_INIT(0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11,
                     0x22, 0x22, 0x22, 0x22, 0x33, 0x33, 0x33, 0x33);

/* A custom descriptor */
// static uint8_t gatt_svr_dsc_val;
// static const ble_uuid128_t gatt_svr_dsc_uuid =
//     BLE_UUID128_INIT(0x01, 0x01, 0x01, 0x01, 0x12, 0x12, 0x12, 0x12,
//                      0x23, 0x23, 0x23, 0x23, 0x34, 0x34, 0x34, 0x34);

static int
gatt_svc_access(uint16_t conn_handle, uint16_t attr_handle,
                struct ble_gatt_access_ctxt *ctxt,
                void *arg);
/****
    CUSTOM BLE CODE
****/

static uint8_t custom_value = 0;

static const ble_uuid128_t time_svc_uuid =
    BLE_UUID128_INIT(0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                     0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00);

static const ble_uuid128_t time_value_uuid =
    BLE_UUID128_INIT(0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11,
                     0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99);

static int time_gatt_svc_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg);

static uint16_t custom_value_handle_1;

//BLE LED Brightness Input Service
static uint8_t led_brightness = 0;
//02484e53-e7ba-44bd-94ba-534333dc2a93
static const ble_uuid128_t led_brt_svc_uuid = 
    BLE_UUID128_INIT(0x93, 0x2a, 0xdc, 0x33, 0x43, 0x53, 0xba, 0x94,
                     0xbd, 0x44, 0xba, 0xe7, 0x53, 0x4e, 0x48, 0x02);
//bff2df76-56d6-4194-ab02-d8979d00ced0
static const ble_uuid128_t led_brt_value_uuid = 
    BLE_UUID128_INIT(0xd0, 0xce, 0x00, 0x9d, 0x97, 0xd8, 0x02, 0xab,
                     0x94, 0x41, 0xd6, 0x56, 0x76, 0xdf, 0xf2, 0xbf);

static int led_brt_gatt_svc_access(uint16_t conn_handle, uint16_t attr_handle,
                            struct ble_gatt_access_ctxt *ctxt, void *arg);

static uint16_t custom_value_handle_2;

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        /*** Service ***/
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[])
        { {
                /*** This characteristic can be subscribed to by writing 0x00 and 0x01 to the CCCD ***/
                .uuid = &gatt_svr_chr_uuid.u,
                .access_cb = gatt_svc_access,
#if CONFIG_EXAMPLE_ENCRYPTION
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE |
                BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_WRITE_ENC |
                BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_INDICATE,
#else
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_INDICATE,
#endif
                .val_handle = &gatt_svr_chr_val_handle,
//                 .descriptors = (struct ble_gatt_dsc_def[])
//                 { {
//                       .uuid = &gatt_svr_dsc_uuid.u,
// #if CONFIG_EXAMPLE_ENCRYPTION
//                       .att_flags = BLE_ATT_F_READ | BLE_ATT_F_READ_ENC,
// #else
//                       .att_flags = BLE_ATT_F_READ,
// #endif
//                       .access_cb = gatt_svc_access,
//                     }, {
//                       0, /* No more descriptors in this characteristic */
//                     }
//                 },
            }, {
                0, /* No more characteristics in this service. */
            }
        },
    },

    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &time_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = &time_value_uuid.u,
            .access_cb = time_gatt_svc_access,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
            .val_handle = &custom_value_handle_1,
        }, {
            0, /* No more characteristics in this service */
        } },
    },

    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &led_brt_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = &led_brt_value_uuid.u,
            .access_cb = led_brt_gatt_svc_access,
            .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE,
            .val_handle = &custom_value_handle_2,
        }, {
            0, /* No more characteristics in this service */
        } },
    },

    {
        0, /* No more services. */
    },
};

static int
gatt_svr_write(struct os_mbuf *om, uint16_t min_len, uint16_t max_len,
               void *dst, uint16_t *len)
{
    uint16_t om_len;
    int rc;

    om_len = OS_MBUF_PKTLEN(om);
    if (om_len < min_len || om_len > max_len) {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    rc = ble_hs_mbuf_to_flat(om, dst, max_len, len);
    if (rc != 0) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    return 0;
}

/**
 * Access callback whenever a characteristic/descriptor is read or written to.
 * Here reads and writes need to be handled.
 * ctxt->op tells weather the operation is read or write and
 * weather it is on a characteristic or descriptor,
 * ctxt->dsc->uuid tells which characteristic/descriptor is accessed.
 * attr_handle give the value handle of the attribute being accessed.
 * Accordingly do:
 *     Append the value to ctxt->om if the operation is READ
 *     Write ctxt->om to the value if the operation is WRITE
 **/
static int
gatt_svc_access(uint16_t conn_handle, uint16_t attr_handle,
                struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    const ble_uuid_t *uuid;
    int rc;
    
    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_READ_CHR:
        if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
            MODLOG_DFLT(INFO, "Characteristic read; conn_handle=%d attr_handle=%d\n",
                        conn_handle, attr_handle);
        } else {
            MODLOG_DFLT(INFO, "Characteristic read by NimBLE stack; attr_handle=%d\n",
                        attr_handle);
        }
        uuid = ctxt->chr->uuid;
        if (attr_handle == gatt_svr_chr_val_handle) {
            rc = os_mbuf_append(ctxt->om,
                                &gatt_svr_chr_val,
                                sizeof(gatt_svr_chr_val));
            return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
        }
        goto unknown;

    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
            MODLOG_DFLT(INFO, "Characteristic write; conn_handle=%d attr_handle=%d",
                        conn_handle, attr_handle);
            uuid = ctxt->chr->uuid;
            if (attr_handle == gatt_svr_chr_val_handle) {
                rc = gatt_svr_write(ctxt->om,
                                    sizeof(gatt_svr_chr_val),
                                    sizeof(gatt_svr_chr_val),
                                    &gatt_svr_chr_val, NULL);
                ble_gatts_chr_updated(attr_handle);
                MODLOG_DFLT(INFO, "Notification/Indication scheduled for "
                            "all subscribed peers.\n");
                if(rc == 0)
                {
                    wrt_data = gatt_svr_chr_val;
                    bleinterruptTriggered = true;
                    //printf("In Fun: %d\n", wrt_data);
                }
                return rc;
            }
            
        } else {
            MODLOG_DFLT(INFO, "Characteristic write by NimBLE stack; attr_handle=%d",
                        attr_handle);
        }
        goto unknown;

    // case BLE_GATT_ACCESS_OP_READ_DSC:
    //     if (conn_handle != BLE_HS_CONN_HANDLE_NONE) {
    //         MODLOG_DFLT(INFO, "Descriptor read; conn_handle=%d attr_handle=%d\n",
    //                     conn_handle, attr_handle);
    //     } else {
    //         MODLOG_DFLT(INFO, "Descriptor read by NimBLE stack; attr_handle=%d\n",
    //                     attr_handle);
    //     }
    //     uuid = ctxt->dsc->uuid;
    //     if (ble_uuid_cmp(uuid, &gatt_svr_dsc_uuid.u) == 0) {
    //         rc = os_mbuf_append(ctxt->om,
    //                             &gatt_svr_dsc_val,
    //                             sizeof(gatt_svr_chr_val));
    //         return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    //     }
    //     goto unknown;

    // case BLE_GATT_ACCESS_OP_WRITE_DSC:
    //     goto unknown;

    default:
        goto unknown;
    }

unknown:
    /* Unknown characteristic/descriptor;
     * The NimBLE host should not have called this function;
     */
    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

static int time_gatt_svc_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg) {
    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR:
            if (attr_handle == custom_value_handle_1) {
                custom_value = gdata_test;
                return os_mbuf_append(ctxt->om, &custom_value, sizeof(custom_value));
            }
            break;

        case BLE_GATT_ACCESS_OP_WRITE_CHR:
            if (attr_handle == custom_value_handle_1) {
                custom_value = ctxt->om->om_data[0];
                //custom_value = gdata_test;//ble_send_write_data(20);       
                printf("Received value from app: %d\n", custom_value);
                return 0;
            }
            break;

        default:
            break;
    }

    return BLE_ATT_ERR_UNLIKELY;
}

static int led_brt_gatt_svc_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg) {
    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR:
            if (attr_handle == custom_value_handle_2) {
                //led_brightness = gdata_test;
                return os_mbuf_append(ctxt->om, &led_brightness, sizeof(led_brightness));
            }
            break;

        case BLE_GATT_ACCESS_OP_WRITE_CHR:
            if (attr_handle == custom_value_handle_2) {
                led_brightness = ctxt->om->om_data[0];
                //led_brightness = gdata_test;//ble_send_write_data(20);
                brightness_in = led_brightness;      
                //printf("Received value from app: %d\n", led_brightness);
                return 0;
            }
            break;

        default:
            break;
    }

    return BLE_ATT_ERR_UNLIKELY;
}

void
gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    char buf[BLE_UUID_STR_LEN];

    switch (ctxt->op) {
    case BLE_GATT_REGISTER_OP_SVC:
        MODLOG_DFLT(DEBUG, "registered service %s with handle=%d\n",
                    ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                    ctxt->svc.handle);
        break;

    case BLE_GATT_REGISTER_OP_CHR:
        MODLOG_DFLT(DEBUG, "registering characteristic %s with "
                    "def_handle=%d val_handle=%d\n",
                    ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                    ctxt->chr.def_handle,
                    ctxt->chr.val_handle);
        break;

    case BLE_GATT_REGISTER_OP_DSC:
        MODLOG_DFLT(DEBUG, "registering descriptor %s with handle=%d\n",
                    ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                    ctxt->dsc.handle);
        break;

    default:
        assert(0);
        break;
    }
}

int
gatt_svr_init(void)
{
    int rc;
    
    ble_svc_gap_init();
    ble_svc_gatt_init();
    ble_svc_ans_init();

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0) {
        return rc;
    }

    /* Setting a value for the read-only descriptor */
    //gatt_svr_dsc_val = 0x99;

    return 0;
}
