/**
  ******************************************************************************
  * @file    LwIP/LwIP_HTTP_Server_Raw/Src/app_ethernet.c 
  * @author  MCD Application Team
  * @version V1.4.1
  * @date    09-October-2015
  * @brief   Ethernet specefic module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lwip/opt.h"
#include "stm32f4xx_hal.h"
#include "lwip/dhcp.h"
#include "app_ethernet.h"
#include "ethernetif.h"
#include "main.h"
#include "config.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#define MAX_DHCP_TRIES  4
extern uint32_t DHCPfineTimer;
__IO uint8_t DHCP_state;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Notify the User about the nework interface config status 
  * @param  netif: the network interface
  * @retval None
  */
void User_notification(struct netif *netif) 
{
	if(netif_is_up(netif))
	{
		if(cfg->dhcpEnabled)
		{
			/* Update DHCP state machine */
			DHCP_state = DHCP_START;
		}
		else
		{
			uint8_t iptxt[20];
			ipaddr_ntoa_r(&cfg->ipAddr, (char*)iptxt, 20);
			//sprintf((char*)iptxt, "%d.%d.%d.%d", , IP_ADDR1, IP_ADDR2, IP_ADDR3);
			printf ("Static IP address: %s\n", iptxt);

		}
	}
	else
	{
		if(cfg->dhcpEnabled)
		{
			/* Update DHCP state machine */
			DHCP_state = DHCP_LINK_DOWN;
		}
		printf ("The network cable is not connected \n");
	}
}

/**
  * @brief  This function notify user about link status changement.
  * @param  netif: the network interface
  * @retval None
  */
void ethernetif_notify_conn_changed(struct netif *netif)
{
	struct ip4_addr ipaddr;
	struct ip4_addr netmask;
	struct ip4_addr gw;

	if(netif_is_link_up(netif))
	{

		if(cfg->dhcpEnabled)
		{
			printf("The network cable is now connected \n");

			/* Update DHCP state machine */
			DHCP_state = DHCP_START;
		}
		else
		{

			ipaddr = cfg->ipAddr;
			netmask = cfg->netmask;
			gw = cfg->gateway;

			uint8_t iptxt[20];

			ipaddr_ntoa_r(&cfg->ipAddr, (char*)iptxt, 20);

			printf ("Static IP address: %s\n", iptxt);

		}
		netif_set_addr(netif, &ipaddr , &netmask, &gw);

		/* When the netif is fully configured this function must be called.*/
		netif_set_up(netif);
	}
	else
	{
		if(cfg->dhcpEnabled)
		{
			/* Update DHCP state machine */
			DHCP_state = DHCP_LINK_DOWN;
		}
		/*  When the netif link is down this function must be called.*/
		netif_set_down(netif);

		printf("The network cable is not connected \n");

	}
}

/**
  * @brief  DHCP_Process_Handle
  * @param  None
  * @retval None
  */
void DHCP_Process(struct netif *netif)
{
	struct ip4_addr ipaddr;
	struct ip4_addr netmask;
	struct ip4_addr gw;
	uint32_t IPaddress = 0;

	switch(DHCP_state)
	{
		case DHCP_START:
		{
			netif->ip_addr.addr = 0;
			netif->netmask.addr = 0;
			netif->gw.addr = 0;
			IPaddress = 0;

			DHCP_state = DHCP_WAIT_ADDRESS;
			dhcp_start(netif);

			printf("  State: Looking for DHCP sever ...\n");
		}
		break;

		case DHCP_WAIT_ADDRESS:
		{
			/* Read the new IP address */
			IPaddress = netif->ip_addr.addr;

			if(IPaddress !=0)
			{
				DHCP_state = DHCP_ADDRESS_ASSIGNED;

				/* Stop DHCP */
				dhcp_stop(netif);

				uint8_t iptxt[20];

				ipaddr_ntoa_r(&netif->ip_addr, (char*)iptxt, 20);

				printf ("IP address assigned by a DHCP server: %s\n", iptxt);

			}
			else
			{

				struct dhcp* dhcp = netif_get_client_data(netif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP);

				if(dhcp->tries > MAX_DHCP_TRIES)
				{
					DHCP_state = DHCP_TIMEOUT;

					/* Stop DHCP */
					dhcp_stop(netif);

					/* Static address used */

					ipaddr = cfg->ipAddr;
					netmask = cfg->netmask;
					gw = cfg->gateway;

					netif_set_addr(netif, &ipaddr , &netmask, &gw);

					uint8_t iptxt[20];

					ipaddr_ntoa_r(&cfg->ipAddr, (char*)iptxt, 20);
					printf("DHCP timeout !!\n");
					printf("Static IP address  : %s\n", iptxt);

				}
			}
		}
		break;
		default: break;
	}
}

/**
  * @brief  DHCP periodic check
  * @param  localtime the current LocalTime value
  * @retval None
  */
void DHCP_Periodic_Handle(struct netif *netif)
{  
	/* Fine DHCP periodic process every 500ms */
	if (HAL_GetTick() - DHCPfineTimer >= DHCP_FINE_TIMER_MSECS)
	{
		DHCPfineTimer =  HAL_GetTick();
		if(	(DHCP_state != DHCP_ADDRESS_ASSIGNED) &&
			(DHCP_state != DHCP_TIMEOUT) &&
			(DHCP_state != DHCP_LINK_DOWN))
		{
			/* process DHCP state machine */
			DHCP_Process(netif);
		}
	}
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
