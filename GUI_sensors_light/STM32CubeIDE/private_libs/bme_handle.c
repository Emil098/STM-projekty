/*
 * bme_handle.c
 *
 *  Created on: Feb 14, 2024
 *      Author: emilr
 */
#include <bme_spi.h>
#include "bme_handle.h"
#include "bme68x.h"
#include "gui_handle.h"
#include <string.h>
#include "cmsis_os.h"


struct bme68x_dev bme;
int8_t rslt;
struct bme68x_conf conf;
struct bme68x_heatr_conf heatr_conf;
struct bme68x_data data[3];
uint32_t del_period;
uint32_t time_ms = 0;
uint8_t n_fields;
uint16_t sample_count = 1;
enviroment_struct env;
char alert[100] = {};

/* Heater temperature in degree Celsius */
uint16_t temp_prof[10] = { 200, 240, 240, 240, 240, 240, 240, 240, 240, 200 };

/* Heating duration in milliseconds */
uint16_t dur_prof[10] = { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 };

void bme_init()
{

  // assigning HAL SPI function for communication
  rslt = bme68x_interface_init(&bme, BME68X_SPI_INTF);
  bme68x_check_rslt("bme68x_interface_init", rslt, alert);

  if(rslt != BME68X_OK)
    return;

  // initializing bme registers
  rslt = bme68x_init(&bme);
  bme68x_check_rslt("bme68x_init", rslt, alert);

  if(rslt != BME68X_OK)
    return;

  // reading saved bme filter and oversampling settings
  rslt = bme68x_get_conf(&conf, &bme);
  bme68x_check_rslt("bme68x_get_conf", rslt, alert);

  if(rslt != BME68X_OK)
    return;

  //udpating filter and oversampling settings
  conf.filter = BME68X_FILTER_SIZE_3;
  conf.odr = BME68X_ODR_20_MS;
  conf.os_hum = BME68X_OS_2X;
  conf.os_pres = BME68X_OS_4X;
  conf.os_temp = BME68X_OS_8X;
  rslt = bme68x_set_conf(&conf, &bme);
  bme68x_check_rslt("bme68x_set_conf", rslt, alert);

  if(rslt != BME68X_OK)
    return;

  // gas resistance measurment settings
  heatr_conf.enable = BME68X_ENABLE;
  heatr_conf.heatr_temp_prof = temp_prof;
  heatr_conf.heatr_dur_prof = dur_prof;
  heatr_conf.profile_len = 10;
  rslt = bme68x_set_heatr_conf(BME68X_SEQUENTIAL_MODE, &heatr_conf, &bme);
  bme68x_check_rslt("bme68x_set_heatr_conf", rslt, alert);

  if(rslt != BME68X_OK)
    return;

  // setting operation mode to sequential read
  rslt = bme68x_set_op_mode(BME68X_SEQUENTIAL_MODE, &bme);
  bme68x_check_rslt("bme68x_set_op_mode", rslt, alert);
}

void bme_handle()
{
  if(rslt != BME68X_OK)
  {
    // after losing communication with bme we need to try to do reinit
    osDelay(TIME_TO_REINIT);
    bme_init();
  }
  // calculating delay time for next measurment
  del_period = bme68x_get_meas_dur(BME68X_SEQUENTIAL_MODE, &conf, &bme) + (heatr_conf.heatr_dur_prof[0] * 1000) + 20000;
  bme.delay_us(del_period , bme.intf_ptr);

  // getting data from bme
  rslt = bme68x_get_data(BME68X_SEQUENTIAL_MODE, data, &n_fields, &bme);
  bme68x_check_rslt("bme68x_get_data", rslt, alert);

  // passing needed data to new structure
  env.humi = data[0].humidity;
  env.temp = data[0].temperature;
  env.preas = (uint16_t)(data[0].pressure / 100);

  if(env.preas == 0)
    rslt = BME68X_E_COM_FAIL;

  bme68x_check_rslt("bme68x_data_check", rslt, alert);
  strcpy(env.alert,alert);

  // updating data in gui_handler library
  update_env_struct(env);
}
