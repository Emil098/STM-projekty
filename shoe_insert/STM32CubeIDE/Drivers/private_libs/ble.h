/*
 * bt.h
 *
 *  Created on: 23 paź 2023
 *      Author: emilr
 */

#ifndef DRIVERS_BLE_BT_H_
#define DRIVERS_BLE_BT_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "../my_libs/def.h"

#define HM10_SHORT_TIMEOUT 1000
#define HM10_LONG_TIMEOUT  2000

// api commands
#define MS_GET_SENS_VAL_U8    "MS+GETVAL"
#define MS_GET_SENS_VAL_CHAR  "MS+GETVALSC"
#define MS_GET_SENS_COLOURS   "MS+GETCOLS"
#define MS_GET_STATUS         "MS+GETSTAT"
#define MS_SET_NEIGHBOUR_VALS "MS+SETV"
#define MS_CALIBRATE_SENSORS  "MS+CALIB"
#define MS_SET_SLEEP_MODE     "MS+SLEEP"
#define MS_WAKE_UP            "MS+WAKEUP"


typedef enum {
  HM10_OK                =   0,
  HM10_RESP_ERROR        =  -1,
  HM10_NO_RESPONSE       =  -2,

} HM10_err_t;

// types of ble device status
typedef enum {
  HM10_NOT_RESPONDING = 0,  // ble is not responding
  HM10_NOT_INITED,          // ble responding but it's not inited
  HM10_RESET_FAIL,          // ble reset command failed
  HM10_START_FAIL,          // ble start command failed
  HM10_READY,               // ble inited and waiting for new connection
  HM10_CONNECTED,           // ble connected
  HM10_CONN_LOST,           // ble just lost connection
  HM10_SLEEP_MODE,          // ble entered sleep mode
}HM10_status_info;

typedef enum {
  DO_NOT_CHANGE = 0,
  SET_N_STAY_SLEEP_MODE,
  DISABLE_SLEEP_MODE,
}sleep_mode_info;

typedef struct {
  HM10_status_info  status;
  int               master_addr[3];
  sleep_mode_info   sleep_mode_change;

}HM10_stat_s;

HM10_status_info HM10_configure_device();
void HM10_handle_connection();
void ble_enable_sleep_mode();
void ble_disable_sleep_mode();
bool ble_am_i_sleeping();

#endif /* DRIVERS_BT_BT_H_ */
