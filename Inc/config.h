/*
 * config.h
 *
 *  Created on: 12.10.2017
 *      Author: admin
 */

#ifndef CONFIG_H_
#define CONFIG_H_
#include "lwip.h"

typedef struct {
	uint8_t dhcpEnabled;
	ip4_addr_t ipAddr;
	ip4_addr_t netmask;
	ip4_addr_t gateway;
}sysconf_t;

sysconf_t *cfg;

#endif /* CONFIG_H_ */
