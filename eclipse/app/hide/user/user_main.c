/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2014/1/1, v1.0 create this file.
*******************************************************************************/
#include "ets_sys.h"
#include "osapi.h"

#include "user_interface.h"

#include "user_devicefind.h"
#include "user_webserver.h"

#include "uart.h"

#include "at_custom.h"

#if ESP_PLATFORM
#include "user_esp_platform.h"
#endif

void user_rf_pre_init(void)
{
}



static void ICACHE_FLASH_ATTR
at_espconn_demo_response(const uint8*data,uint32 length){
	if((data == NULL) || (length == 0)) {
		return;
	}
	uint8 buffer[1024] = {0};
	//uart0_sendStr("send data recieve\n");
	os_sprintf(buffer,"%s\n", data);
	os_printf("demo: %s\n", buffer);
}


static void ICACHE_FLASH_ATTR
at_exeCmdTest(uint8_t id)
{
     at_port_print("at_exeCmdTest");

     at_fake_uart_enable(TRUE,at_espconn_demo_response);

     at_response_ok();
}

at_funcationType at_custom_cmd[] = {
    {"+TEST", 5, NULL, NULL, NULL, at_exeCmdTest}
};

/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
void user_init(void)
{
    os_printf("SDK version hhh:%s\n", system_get_sdk_version());


#if ESP_PLATFORM
    /*Initialization of the peripheral drivers*/
    /*For light demo , it is user_light_init();*/
    /* Also check whether assigned ip addr by the router.If so, connect to ESP-server  */
    user_esp_platform_init();
#endif
    /*Establish a udp socket to receive local device detect info.*/
    /*Listen to the port 1025, as well as udp broadcast.
    /*If receive a string of device_find_request, it rely its IP address and MAC.*/
    user_devicefind_init();

    /*Establish a TCP server for http(with JSON) POST or GET command to communicate with the device.*/
    /*You can find the command in "2B-SDK-Espressif IoT Demo.pdf" to see the details.*/
    /*the JSON command for curl is like:*/
    /*3 Channel mode: curl -X POST -H "Content-Type:application/json" -d "{\"period\":1000,\"rgb\":{\"red\":16000,\"green\":16000,\"blue\":16000}}" http://192.168.4.1/config?command=light      */
    /*5 Channel mode: curl -X POST -H "Content-Type:application/json" -d "{\"period\":1000,\"rgb\":{\"red\":16000,\"green\":16000,\"blue\":16000,\"cwhite\":3000,\"wwhite\",3000}}" http://192.168.4.1/config?command=light      */
#ifdef SERVER_SSL_ENABLE
    user_webserver_init(SERVER_SSL_PORT);
#else
    user_webserver_init(SERVER_PORT);
#endif

    uart_init(BIT_RATE_115200, BIT_RATE_115200);

    at_init();
	at_port_print("\r\nready\r\n");
	at_cmd_array_regist(&at_custom_cmd[0], sizeof(at_custom_cmd)/sizeof(at_custom_cmd[0]));
	//at_fake_uart_enable(TRUE,at_espconn_demo_response);


   /*while(1){
	   at_port_print("55555\r\n");
	   os_delay_us(100000);
   }*/
}

