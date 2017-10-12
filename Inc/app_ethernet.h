/*
 * app_ethernet.h
 *
 *  Created on: 11.10.2017
 *      Author: admin
 */

#ifndef APP_ETHERNET_H_
#define APP_ETHERNET_H_


/* Includes ------------------------------------------------------------------*/
#include "lwip/netif.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* DHCP process states */
#define DHCP_OFF                   (uint8_t) 0
#define DHCP_START                 (uint8_t) 1
#define DHCP_WAIT_ADDRESS          (uint8_t) 2
#define DHCP_ADDRESS_ASSIGNED      (uint8_t) 3
#define DHCP_TIMEOUT               (uint8_t) 4
#define DHCP_LINK_DOWN             (uint8_t) 5

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void User_notification(struct netif *netif);

void DHCP_Process(struct netif *netif);
void DHCP_Periodic_Handle(struct netif *netif);

#endif /* APP_ETHERNET_H_ */
