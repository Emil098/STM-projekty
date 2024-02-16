/*
 * ble.c
 *
 *  Created on: 23 paź 2023
 *      Author: emilr
 */


#include "../my_libs/ble.h"

#include "main.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "stdbool.h"

#include "../my_libs/hm10.h"
#include "../my_libs/motor.h"
#include "../my_libs/sens.h"

#define RESPONSE_BUF_SIZE    128
static char response_buf[RESPONSE_BUF_SIZE];
uint32_t  respos = 0;

HM10_stat_s stats = {HM10_NOT_INITED, {}, DO_NOT_CHANGE};
extern all_sens_info sens;

// basic send string function
static void HM10_AT_SendData(const char* cmd)
{
  while (*cmd) {
    if (!HM10_io_cmd_putc(*cmd, 1000))
      break;
    cmd++;
  }
}

// sometimes we need to send api binary data (sending '0' would end transmission too soon)
static void HM10_AT_SendnData(const char* cmd,uint16_t qt)
{
  while (qt) {
    if (!HM10_io_cmd_putc(*cmd, 1000))
      break;
    cmd++;
    qt--;
  }
}

static void HM10_hard_reset()
{
  HAL_GPIO_WritePin(BT_RST_GPIO_Port, BT_RST_Pin, 0);
  osDelay(500);
  HAL_GPIO_WritePin(BT_RST_GPIO_Port, BT_RST_Pin, 1);
}

static HM10_err_t HM10_AT_GetResponse(const char *cmd, const char *resp_ok, uint32_t timeout_ms)
{
  uint8_t   d = 0;
  uint32_t  start = HAL_GetTick();
  uint32_t  respos = 0;
  uint16_t  resp_len = strlen(resp_ok);

  // clearing response buffer
  memset(response_buf,0,RESPONSE_BUF_SIZE);

  while (HAL_GetTick() - start <= timeout_ms)
  {
    if (HM10_io_cmd_getc(&d)) {
      // adding next char to string, calculating pointer to compare module answer with the right one, until timeout will occur
      response_buf[respos] = d;
      respos++;
      uint16_t resp_pt = respos > resp_len ? respos - resp_len : 0;
      if(strcmp(response_buf + resp_pt,resp_ok) == 0)
        return HM10_OK;
    }
  }

  // error when hm10 sent something but it didn't match
  if(respos != 0)
    return HM10_RESP_ERROR;

  // if buffer is empty
  return HM10_NO_RESPONSE;
}


static HM10_err_t HM10_AT_SetParamCommand(const char* cmd, const char* param, uint32_t timeout_ms)
{
  HM10_err_t status;
  uint8_t repeat = 3; // maybe it's unnecessary
  char tx_cmd[20] = {};
  char rx_resp[20] = {};

  // preparing tx data to be sent, and rx expected answer
  sprintf(tx_cmd,"%s%s",cmd,param);
  sprintf(rx_resp,"OK+Set:%s",param); // for most of the set commands it's OK+Set:param but not for all


  // cleaning rx buffer
  while(HM10_io_cmd_getc(NULL));

  if (repeat <= 0)
    repeat = 1;

  do {
    // repeated send receive routine
    HM10_AT_SendData(tx_cmd);
    status = HM10_AT_GetResponse(cmd, rx_resp, timeout_ms);
    repeat--;
    osDelay(10);
  }
  while ((repeat > 0) && (status != HM10_OK));

  return status;
}

static HM10_err_t HM10_AT_SendCommandAndGetResponse(const char* cmd, const char *resp_ok, uint32_t timeout_ms, int repeat)
{
  HM10_err_t err;

  if (repeat <= 0)
    repeat = 1;

  do {
    // simplified send receive routine for non param commands
    HM10_AT_SendData(cmd);
    err = HM10_AT_GetResponse(cmd, resp_ok, timeout_ms);
    repeat--;
    osDelay(10);
  }
  while ((repeat > 0) && (err != HM10_OK));

  return err;
}

HM10_status_info HM10_configure_device()
{
  HM10_err_t resp = HM10_OK;

  HM10_io_init();  // init uart interrupts (interrupt handle function is added in stm32l1xx_it.c)

  resp = HM10_AT_SendCommandAndGetResponse("AT", "OK", HM10_SHORT_TIMEOUT, 5); // checking is ble available

  if(resp == HM10_NO_RESPONSE)
  {
    stats.status = HM10_NOT_RESPONDING;
    return stats.status;
  }

  if(resp == HM10_OK)
    resp = HM10_AT_SetParamCommand("AT+IMME", "1", HM10_SHORT_TIMEOUT); // 1 - answer AT commands after waking or exiting sleep mode (0 - default do not answer)

  if(resp == HM10_OK)
    resp = HM10_AT_SetParamCommand("AT+NOTI", "1", HM10_SHORT_TIMEOUT); //inform about connecting and disconnecting ble (OK+CONN, OK+LOST)

  if(resp == HM10_OK)
    resp = HM10_AT_SetParamCommand("AT+NOTP", "1", HM10_SHORT_TIMEOUT); //print address of device after establishing connection

  if(resp == HM10_OK)
    resp = HM10_AT_SetParamCommand("AT+SHOW", "1", HM10_SHORT_TIMEOUT); //show device names during searching

  if(resp == HM10_OK)
    resp = HM10_AT_SetParamCommand("AT+NAME", BT_NAME            , HM10_SHORT_TIMEOUT);   // ble name

  if(resp == HM10_OK)
    resp = HM10_AT_SetParamCommand("AT+UUID", SERVICE_UUID       , HM10_SHORT_TIMEOUT);    // service

  if(resp == HM10_OK)
      resp = HM10_AT_SetParamCommand("AT+CHAR", STATUS_CHAR_UUID, HM10_SHORT_TIMEOUT);    // characteristic


  if(resp == HM10_OK)
    resp = HM10_AT_SetParamCommand("AT+ROLE", "0", HM10_SHORT_TIMEOUT); // 0 - slave 1 - master

  if(resp == HM10_OK)
    resp = HM10_AT_SetParamCommand("AT+ADVI", "8", HM10_SHORT_TIMEOUT); // advertise every 1s

  if(resp == HM10_OK)
    resp = HM10_AT_SetParamCommand("AT+IBEA", "0", HM10_SHORT_TIMEOUT); //turn off iBeacone

  if(resp == HM10_OK)
    resp = HM10_AT_SetParamCommand("AT+ADTY", "0", HM10_SHORT_TIMEOUT); // Advertising, ScanResponse, Connectable

  if(resp == HM10_OK)
    resp = HM10_AT_SendCommandAndGetResponse("AT+FLAG0", "OK+FLAG0", HM10_SHORT_TIMEOUT, 2); // anything must be set to make ble visible

  if(resp == HM10_OK)
    resp = HM10_AT_SendCommandAndGetResponse("AT+RESET", "OK+RESET", HM10_LONG_TIMEOUT, 3);

  if(resp != HM10_OK)
  {
    // return if ble is not responding properly and set status (for further attempts in task loop)
    stats.status = HM10_RESET_FAIL;
    return stats.status;
  }

  // waiting for completing reset (it takes about 1.5s)
  osDelay(HM10_LONG_TIMEOUT);

  if(resp == HM10_OK)
    resp = HM10_AT_SendCommandAndGetResponse("AT", "OK", HM10_SHORT_TIMEOUT, 2); // is ble responding?

  if(resp == HM10_OK)
    resp = HM10_AT_SendCommandAndGetResponse("AT+START", "OK+START", HM10_LONG_TIMEOUT, 3);  // sending start command to make ble visible

  if(resp == HM10_OK)
  {
    // if everything went well set status and vibrate shortly to inform user that ble is ready for establishing connection
    stats.status = HM10_READY;
    motor_vibrate(SHORT_WIBRATION);
  }
  else
    stats.status = HM10_START_FAIL; // if after reset command we got error ble need to be started again

  return stats.status;
}

/*
those won't be needed (app will have all necessary data)
HM10_err_t HM10_neighbour_vals(char *msg)
{
  uint8_t sens_val[10] = {};

  memcpy(sens_val,msg+7,10);

  for(uint8_t i=0;i<5;i++)
  {
    if(sens_val[i*2] == 0 || sens_val[(i*2)+1] == 0)
      return HM10_EXPECT_MORE;
    sens.neigbour[i].weight =((float)(sens_val[i*2]) + ((float)(sens_val[(i*2)+1]) / 100));
  }
  return HM10_OK;
}*/

//static void HM10_reset_session()
//{
//  if(HM10_AT_SendCommandAndGetResponse("AT+RESET", "OK+RESET", HM10_LONG_TIMEOUT, 3) != HM10_OK)
//    stats.status = HM10_NOT_RESPONDING;
//}

static void HM10_restart_session()
{
  // characteristic won't appear until flag isn't set (after disconnecting)
  HM10_err_t resp = HM10_AT_SendCommandAndGetResponse("AT+FLAG0", "OK+FLAG0", HM10_SHORT_TIMEOUT, 3);

  if(resp == HM10_OK)
    resp = HM10_AT_SendCommandAndGetResponse("AT+RESET", "OK+RESET", HM10_SHORT_TIMEOUT, 3);  // trzeba wyslac reset (bez tego start nie zadziala)

  osDelay(HM10_LONG_TIMEOUT);

  if(resp == HM10_OK)
    resp = HM10_AT_SendCommandAndGetResponse("AT", "OK", HM10_SHORT_TIMEOUT, 3);

  if(resp == HM10_OK)
    resp = HM10_AT_SendCommandAndGetResponse("AT+START", "OK+START", HM10_SHORT_TIMEOUT, 2);  // trzeba wyslac start zeby tel mogl sie polaczyc

  if(resp == HM10_OK)
  {
    stats.status = HM10_READY;
    motor_vibrate(SHORT_WIBRATION);
  }
  else
    stats.status = HM10_NOT_RESPONDING;
}

static void HM10_set_sleep_mode(bool sleep)
{
  HM10_err_t  status    = HM10_OK;
//  memset(msg,'>',100); // sending 80 bytes should cause exiting sleep mode (but it's not working)


  if(sleep)
  {

    // if device is connected ble will send "OK+LOST" after using "AT" command | otherwise ble will response with "OK"
    char *resp = stats.status == HM10_CONNECTED ? "OK+LOST" : "OK";

    status = HM10_AT_SendCommandAndGetResponse("AT", resp, HM10_LONG_TIMEOUT, 2);

    if(status == HM10_OK)
      status = HM10_AT_SendCommandAndGetResponse("AT+SLEEP", "OK+SLEEP", HM10_LONG_TIMEOUT, 2);

    // jak uda sie wywolac komende uspienia obudzenia resetujemy zmienne wywolujace ta funkcje (jakby komenda nie dotarla do modulu resetujemy go i jezeli mial isc spac to funkcja sie wywola jeszcze raz)
    // after executing sleep command we need to update status (if something went wrong this function will be executed repeatedly in task routine as long as we'll get success)
    stats.status = status == HM10_OK ? HM10_SLEEP_MODE : HM10_NOT_RESPONDING;

  }
  else
  {
    // nawet jak modul nie odpowie na komende to zostanie zresetowany i nie bedzie potrzebowal dodatkowego wybudzania
    // if ble won't answer after resetting then it will be reinited anyway, so we shall change sleep status variable here
    stats.sleep_mode_change = DO_NOT_CHANGE;

    HM10_hard_reset();
    HM10_AT_GetResponse(NULL, "OK+WAKE", HM10_LONG_TIMEOUT);

    // after resetting, ble should be reinited to establish new connection
    stats.status = HM10_NOT_INITED;

    // po poprawnym wybudzeniu nalezy po resecie zrobic reinit aby modul mogl nawiazac polaczenie (bez staru nie nawiaze)
  }

}

// this function is needed to search rx data buffer (between messages in there, module could send '0' byte and normal strstr would return NULL in that case)
static char *my_strstr(char *dest,char *src,uint32_t left_bytes)
{
  uint32_t src_size = strlen(src);
  left_bytes -= src_size;

  while(strncmp(dest,src,src_size) != 0)
  {
    dest++;
    if(!(left_bytes--))
      return 0;
  }
  return dest;
}

uint32_t last_msg_ts = 0;
uint32_t last_msg_t = 0;


void HM10_handle_connection()
{
  uint8_t     d;
  char *      msg_pt;
  bool        empty_buff = false;
  HM10_err_t  stat = HM10_NO_RESPONSE;
  int         id[3];
  uint32_t    now;

  // adding next received bytes from ble (till buffer overflow)
  while(HM10_io_cmd_getc(&d)) {
    if(respos < RESPONSE_BUF_SIZE - 1)
    {
      response_buf[respos] = d;
      respos++;
    }
    else
    {
      memset(response_buf,0,RESPONSE_BUF_SIZE);
      respos = 0;
    }
  }


  // searching for messages from hm10 (connection status)
  msg_pt = my_strstr((char *)response_buf,"OK+",RESPONSE_BUF_SIZE);
  while((msg_pt = my_strstr(msg_pt,"OK+",RESPONSE_BUF_SIZE)) != NULL)
  {
    uint8_t addr_size = sscanf(msg_pt, "OK+CONN:%4x%4x%4x", &id[0], &id[1], &id[2]);
    if ((addr_size <= 3 && addr_size > 0) || (strncmp(msg_pt, "OK+CONN",7) == 0))
    {
      // after establishing new connection reset sleep mode time stamp
      sens_reset_timestamp();
      if(addr_size == 3)
        memcpy(stats.master_addr,id,3);
      // in case we were in the sleep mode and just woke up
      stats.sleep_mode_change = DO_NOT_CHANGE;
      stats.status = HM10_CONNECTED;
      empty_buff = true;
    }
    else if(strcmp(msg_pt, "OK+LOST") == 0)
    {
      stats.status = HM10_CONN_LOST;
      empty_buff = true;
    }
    else
      empty_buff = false;

    // increasing pointer to search the rest of the buffer
    msg_pt += 3;
  }

  // searching for api commands (from the beginning of the buffer)
  msg_pt = my_strstr((char *)response_buf,"MS+",RESPONSE_BUF_SIZE);
  while((msg_pt = my_strstr(msg_pt,"MS+",RESPONSE_BUF_SIZE)) != NULL)
  {

    if(strncmp(msg_pt,MS_GET_SENS_VAL_U8,strlen(MS_GET_SENS_VAL_U8)) == 0)
    {
      now = HAL_GetTick();
      last_msg_t = now - last_msg_ts;
      last_msg_ts = now;
      // user app requested weight data
      HM10_AT_SendnData(sens.wei_tx_data,10);
      stat = HM10_OK;
    }

    else if(strncmp(msg_pt,MS_GET_SENS_COLOURS,strlen(MS_GET_SENS_COLOURS)) == 0)
    {
      // user app requested weight colors data
      HM10_AT_SendnData(sens.col_tx_data,10);
      stat = HM10_OK;
    }
    else if(strncmp(msg_pt,MS_GET_STATUS,strlen(MS_GET_STATUS)) == 0)
    {
      // user app requested feet position
      HM10_AT_SendnData(sens.stat_tx_data,3);
      stat = HM10_OK;
    }
    else if(strncmp(msg_pt,MS_SET_NEIGHBOUR_VALS,strlen(MS_SET_NEIGHBOUR_VALS)) == 0)
    {
      // probably won't be needed
      // stat = HM10_neighbour_vals(msg_pt);
    }
    else if(strncmp(msg_pt,MS_CALIBRATE_SENSORS,strlen(MS_CALIBRATE_SENSORS)) == 0)
    {
      // user app requested immediate sensor calibration (currently user is standing in the proper position)
      stat = HM10_OK;
      sens_calibrate();
    }

    // increasing pointer to search the rest of the buffer
    msg_pt += 3;

    if(stat == HM10_OK && my_strstr(msg_pt,"MS+",RESPONSE_BUF_SIZE) == NULL)
      empty_buff = true;
  }


  if(empty_buff || (respos >= (RESPONSE_BUF_SIZE - 20)))
  {
    // clearing buffer if we received full commands or buffer is about to overflow
    memset(response_buf,0,RESPONSE_BUF_SIZE);
    respos = 0;
  }

  // handling hm10 status
  switch(stats.status)
  {

  case HM10_CONN_LOST:
  case HM10_START_FAIL:
    // if ble lost connection or failed to start new session it needs to be restarted by AT commands
    HM10_restart_session();
    break;

  case HM10_NOT_RESPONDING:
    // if module is not responding we will repeatedly reset ble till we got any communication
    HM10_hard_reset();
    osDelay(1000);
    if(HM10_AT_SendCommandAndGetResponse("AT", "OK", HM10_SHORT_TIMEOUT, 5) == HM10_OK)
      stats.status = HM10_NOT_INITED;
    break;

  case HM10_NOT_INITED:
  case HM10_RESET_FAIL:
    //initializing ble after recovering communication or error during last init
    HM10_configure_device();
    break;

  case HM10_CONNECTED:
  case HM10_READY:
    // if device is connected or just initialized and we got sleep mode request
    if(stats.sleep_mode_change == SET_N_STAY_SLEEP_MODE)
      HM10_set_sleep_mode(true);
    break;

  case HM10_SLEEP_MODE:
    // exiting sleep mode if we got request
    if(stats.sleep_mode_change == DISABLE_SLEEP_MODE)
      HM10_set_sleep_mode(false);
    break;

  }
}

void ble_enable_sleep_mode() { stats.sleep_mode_change = SET_N_STAY_SLEEP_MODE;  };

void ble_disable_sleep_mode(){ stats.sleep_mode_change = stats.status == HM10_SLEEP_MODE ? DISABLE_SLEEP_MODE : DO_NOT_CHANGE;     };

bool ble_am_i_sleeping()
{
  return (stats.sleep_mode_change == SET_N_STAY_SLEEP_MODE ? true : false);
}



