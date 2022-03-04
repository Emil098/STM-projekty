/*
 * http_ssi.c
 *
 *  Created on: 01.03.2022
 *      Author: Emil
 */

#include"http_ssi.h"
#include "string.h"
#include "stdio.h"

#include "lwip/tcp.h"
#include "lwip/apps/httpd.h"

#include "stm32f1xx_hal.h"

/* CGI login led start */

const char* LedCGIhandler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]);

const tCGI LedCGI = { "/leds.cgi", LedCGIhandler };

/* CGI login led end */

/* CGI login page start */

const char* LoginCGIhandler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]);

const tCGI LoginCGI = { "/login.cgi", LoginCGIhandler };

tCGI theCGItable[2];

/* CGI login page end */

/* SSI login led start (updating 2 checkboxes status for LD1 and LD2, updating LD3 btn  */

u16_t mySSIHandler(int iIndex, char *pcInsert, int iInsertLen);

#define numSSItags 3

char const *theSSItags[numSSItags] = { "tag1", "tag2", "tag3" };
// tag1 - Led 1 checkbox
// tag2 - Led 2 checkbox
// tag3 - Led 3 button


/* SSI login led end */

const char* LedCGIhandler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {

	uint32_t i = 0;

	for (i = 0; i < iNumParams; i++) { // loop for checking all parameters

		if (strcmp(pcParam[i], "led") == 0)
		{
			if (strcmp(pcValue[i], "1") == 0) {

				// LD3 LED (red) on the board is ON!
				LD1ON = true;
			}

			else if (strcmp(pcValue[i], "2") == 0) {

				// LD2 LED (blue) on the board is ON!
				LD2ON = true;
			}
		}

		if(strcmp(pcParam[i], "onTime") == 0)
		{
			// if Led 3 btn was clicked then change LD3ON status
			if (strcmp(pcValue[i], "Led+3+Off") == 0) {
				LD3ON = true;
			}
			if (strcmp(pcValue[i], "Led+3+On") == 0) {
				LD3ON = false;
			}


			if(LD3ON != LD3ON_last){
				return "/leds.shtml";
			}
			LD3ON_last = LD3ON;
		}

	}

	// setting proper states on led gpio outputs

	HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, LD1ON);

	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, LD2ON);

	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, LD3ON);


	if(LD2ON)return "/index.html"; // after choosing Led 2 load index page
	else return "/leds.shtml";     // in other chases reloading led page (ssi updates checkboxes according to LDxON variables)

}

const char* LoginCGIhandler(int iIndex, int iNumParams, char *pcParam[],
		char *pcValue[]) {

	uint32_t i = 0;
	uint8_t log_ok=0,pas_ok=0;

	for (i = 0; i < iNumParams; i++) {  // loop for checking all parameters

		if (strcmp(pcParam[i], "username") == 0)
		{
			if (strcmp(pcValue[i], "emil.rudnicki%40pollub.edu.pl") == 0) {
				// if user name is ok then set 1 to log_ok variable
				log_ok = true;
			}
		}

		if (strcmp(pcParam[i], "password") == 0)
		{
			// if user password is ok then set 1 to pas_ok variable
			if (strcmp(pcValue[i], "AdminAdmin") == 0) {
				pas_ok = true;

			}

		}

	}


	if(log_ok && pas_ok)return "/leds.shtml"; // if login and pwd is correct then go to leds page
	else return "/login.shtml";  // in other cases reload login page

}

void myCGIinit(void) {

	// initializing cgi handler for led and login pages
	theCGItable[0] = LedCGI;
	theCGItable[1] = LoginCGI;
	http_set_cgi_handlers(theCGItable,2);
}


u16_t mySSIHandler(int iIndex, char *pcInsert, int iInsertLen) {

	// SSI updates all tags with " html input line" iIndex 1 and 2 for checkboxes, iIndex 3 for submit button
	if (iIndex == 0) {

		if (LD1ON == false) {

			char myStr1[] = "<input value=\"1\" name=\"led\" type=\"checkbox\">";

			strcpy(pcInsert, myStr1);

			return strlen(myStr1);
		}

		else if (LD1ON == true) {

			char myStr1[] =
					"<input value=\"1\" name=\"led\" type=\"checkbox\" checked>";
			strcpy(pcInsert, myStr1);

			return strlen(myStr1);

		}

	}

	else if (iIndex == 1)

	{
		if (LD2ON == false) {
			char myStr2[] = "<input value=\"2\" name=\"led\" type=\"checkbox\">";
			strcpy(pcInsert, myStr2);

			return strlen(myStr2);
		}

		else if (LD2ON == true) {

			// since the LD2 blue LED on the board is ON we make its checkbox checked!
			char myStr2[] =
					"<input value=\"2\" name=\"led\" type=\"checkbox\" checked>";
			strcpy(pcInsert, myStr2);

			return strlen(myStr2);

		}

	}

	else if (iIndex == 2)

	{
		if (LD3ON == false) {
			char myStr3[] =
			"<input type=\"submit\" id=\"onTime\" name=\"onTime\" value=\"Led 3 Off\">";

			strcpy(pcInsert, myStr3);

			return strlen(myStr3);
		}

		else if (LD3ON == true) {

			// since the LD2 blue LED on the board is ON we make its checkbox checked!
			char myStr3[] =
					"<input type=\"submit\" id=\"onTime\" name=\"onTime\" value=\"Led 3 On\">";
			strcpy(pcInsert, myStr3);

			return strlen(myStr3);

		}

	}

	return 0;

}

// function to initialize SSI handler
void mySSIinit(void) {

	http_set_ssi_handler(mySSIHandler, (char const**) theSSItags,
			numSSItags);
}


// function to initialize http server + SSI + CGI
void http_server_init (void)
{
	httpd_init();

	mySSIinit();

	myCGIinit();

}

