/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "fs_handler.h"
#include "post_handler.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lwip/apps/httpd.h"
#include "lwip/netif.h"
#include "lwip/netifapi.h"
#include "console.h"
#include "session.h"
#include "settings.h"
#include "ip_format.h"
#include "sensor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern struct netif gnetif;
QueueHandle_t mainTaskQH = NULL;

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
uint32_t defaultTaskBuffer[ 512 ];
osStaticThreadDef_t defaultTaskControlBlock;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadStaticDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 256, defaultTaskBuffer, &defaultTaskControlBlock);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief Main application task (system initialization + runtime service loop).
 *
 * This task acts as the central coordinator of the firmware. It is responsible for:
 *  - Creating the main message queue used for asynchronous configuration updates
 *  - Initializing HTTP server layers (fs handler, POST handler, sessions)
 *  - Loading persistent settings from internal Flash (with CRC validation)
 *  - Applying network configuration to lwIP netif
 *  - Periodically updating the sensor cache
 *  - Processing configuration update requests received from other modules (REST/CLI)
 *
 * Design notes:
 *  - A static FreeRTOS queue is used to avoid heap allocations.
 *  - Network reconfiguration and Flash writes are performed in this task to keep
 *    the HTTP callbacks lightweight and deterministic.
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  (void)argument;

  TOSMessage msg;
  static Flash_settings Settings;

  /* Create a small, statically allocated message queue for configuration updates. */
  static uint8_t mainTaskQS[2 * sizeof(TOSMessage)];
  static StaticQueue_t mainTaskQ;
  mainTaskQH = xQueueCreateStatic(2, sizeof(TOSMessage), mainTaskQS, &mainTaskQ);

  /* Initialize HTTP server integration layers (lwIP httpd hooks + session store). */
  fs_handlers_init();
  post_handlers_init();
  sessions_init();
  httpd_init();

  /* Initialize external sensor driver (readout is performed periodically in the loop). */
  init_sensor();

  conpln(GREEN "DefaultTask: start" CLRST);

  /* Load settings from Flash. If CRC check fails, restore defaults and save them. */
  if(settings_init(&Settings) == INT_FLASH_ERR)
  {
    conpln("Saving default settings to flash");
    flash_write_settings();
    get_settings(&Settings);
  }

  /* Apply network configuration to lwIP interface. */
  conpln("MAC: " MAC_FORMAT , MAC_PARAMS(gnetif.hwaddr));
  conpln("Static IP: " CYAN IPv4_FORMAT CLRST, IPv4_PARAMS(Settings.IP));
  netifapi_netif_set_addr(&gnetif,
                          (ip4_addr_t*)&Settings.IP,
                          (ip4_addr_t*)&Settings.mask,
                          (ip4_addr_t*)&Settings.gate);

  /* Main service loop:
   *  - handle asynchronous update messages
   *  - refresh sensor readings
   */
  for(;;)
  {
    /* Process queued requests (e.g., from REST endpoints) */
    if (xQueueReceive(mainTaskQH, &msg, 1000) == pdTRUE) {
      switch (msg.msg) {
      case OSMSG_NONE: break;

      case OSMSG_UPDATE_IP:
        /* Apply new IP settings and save them to internal Flash. */
        osDelay(1000);
        flash_update_settings(&msg.new_settings);
        get_settings(&Settings);
        conpln("New static IP: " CYAN IPv4_FORMAT CLRST " from CLI", IPv4_PARAMS(Settings.IP));
        netifapi_netif_set_addr(&gnetif, (ip4_addr_t*)&Settings.IP, (ip4_addr_t*)&Settings.mask, (ip4_addr_t*)&Settings.gate);
        flash_write_settings();
        break;

      case OSMSG_UPDATE_SETTINGS:
        /* Update authentication/settings data and save them to internal Flash. */
        osDelay(1000);
        flash_update_settings(&msg.new_settings);
        if(flash_write_settings() == INT_FLASH_OK)
          conpln("Settings updated");
        break;
      }
    }

    /* Periodically update cached sensor values (REST reads are non-blocking). */
    update_sensor();
    osDelay(200);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
