/*
 * def.h
 *
 *  Created on: 23 paź 2023
 *      Author: emilr
 */

#ifndef DRIVERS_BT_DEF_H_
#define DRIVERS_BT_DEF_H_

#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

// _fastX types mean at least X, smaller types are expanded to the architecture's natural type
#define fu8  uint_fast8_t
#define fu16 uint_fast16_t
#define fu32 uint_fast32_t
#define fu64 uint_fast64_t

#define WKLADKA_L
//#define WKLADKA_P

#ifdef WKLADKA_L
  #define BT_NAME               "Lfeet"
  #define SERVICE_UUID          "0xFFE0"
#else
  #define BT_NAME               "Rfeet"
  #define SERVICE_UUID          "0xFFE0"
#endif

#define STATUS_CHAR_UUID      "0xFFE1"
#define SEN_1_CHAR_UUID       "0xFFE0"
#define SEN_2_CHAR_UUID       "0xFFDF"
#define SEN_3_CHAR_UUID       "0xFFDE"
#define SEN_4_CHAR_UUID       "0xFFDD"
#define SEN_5_CHAR_UUID       "0xFFDC"


#endif /* DRIVERS_BT_DEF_H_ */
