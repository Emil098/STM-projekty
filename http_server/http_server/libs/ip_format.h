/*
 * ip_format.h
 *
 *  Created on: Feb 18, 2026
 *      Author: emilr
 */

#ifndef IP_FORMAT_H_
#define IP_FORMAT_H_

#define IPv4TOU32(x1,x2,x3,x4) \
        (((uint32_t)((x4) & 0xff) << 24) | \
         ((uint32_t)((x3) & 0xff) << 16) | \
         ((uint32_t)((x2) & 0xff) << 8)  | \
          (uint32_t)((x1) & 0xff))

#define IPv4_FORMAT     "%lu.%lu.%lu.%lu"
#define IPv4_PARAMS(x)  (x) & 0xFF, ((x) >> 8) & 0xFF, ((x) >> 16) & 0xFF, ((x) >> 24) & 0xFF

#define MAC_FORMAT     "%02x-%02x-%02x-%02x-%02x-%02x"
#define MAC_PARAMS(x)  x[0], x[1], x[2], x[3], x[4], x[5]

#endif /* IP_FORMAT_H_ */
