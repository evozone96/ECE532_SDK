/******************************************************************************/
/*                                                                            */
/* TCPEchoClient -- A DEIPcK TCP Client application to demonstrate how to use */
/*                  the TcpClient Class. This can be used in conjunction with */
/*                  TCPEchoServer.                                            */
/*                                                                            */
/******************************************************************************/
/* Author: Keith Vogel                                                        */
/* Copyright 2014, Digilent Inc.                                              */
/******************************************************************************/
/*
 *
 * Copyright (c) 2013-2014, Digilent <www.digilentinc.com>
 * Contact Digilent for the latest version.
 *
 * This program is free software; distributed under the terms of
 * BSD 3-clause license ("Revised BSD License", "New BSD License", or "Modified BSD License")
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1.    Redistributions of source code must retain the above copyright notice, this
 *        list of conditions and the following disclaimer.
 * 2.    Redistributions in binary form must reproduce the above copyright notice,
 *        this list of conditions and the following disclaimer in the documentation
 *        and/or other materials provided with the distribution.
 * 3.    Neither the name(s) of the above-listed copyright holder(s) nor the names
 *        of its contributors may be used to endorse or promote products derived
 *        from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/******************************************************************************/
/* Revision History:                                                          */
/*                                                                            */
/*    05/14/2014(KeithV):   Created                                           */
/*    08/09/2016(TommyK):   Modified to use Microblaze/Zynq                   */
/*    12/02/2017(atangzwj): Validated for Vivado 2016.4                       */
/*    01/20/2018(atangzwj): Validated for Vivado 2017.4                       */
/*                                                                            */
/******************************************************************************/

#include "PmodWIFI.h"
#include "xil_cache.h"
#include "xil_types.h"
#include "string.h"

extern "C"
{
	#include "PmodALS.h"
}
#include "sleep.h"
#include "xparameters.h"

PmodALS ALS;

#define IPU
#define CORE_NAME 0
#define OP_CTRL 1
#define IB_ADDR_BASE 2
#define FRAME_SIZE 3
#define OP_STATUS 4
#define REF_IMG_BASE 0x100
#define PRCD_IMG_BASE 0x9800
#define BLOBS_MAP_BASE 0xA200
#define ROW_0_NUM 0xA201
#define BLOBS_BASE 0xA400



#ifdef __MICROBLAZE__
#define PMODWIFI_VEC_ID XPAR_INTC_0_PMODWIFI_0_VEC_ID
#else
#define PMODWIFI_VEC_ID XPAR_FABRIC_PMODWIFI_0_WF_INTERRUPT_INTR
#endif

/************************************************************************/
/*                                                                      */
/*              SET THESE VALUES FOR YOUR NETWORK                       */
/*                                                                      */
/************************************************************************/

const char *szIPServer = "192.168.1.28"; // Server to connect to
//const char *szIPServer = "192.168.5.122"; // Server to connect to
uint16_t portServer = DEIPcK::iPersonalPorts44 + 400; // Port 44300
volatile unsigned int * FRAME_BUFFER_BASE_ADDR  = (unsigned int *)0xc0000000;
//volatile unsigned int * FRAME_SIZE = (unsigned int *)0x00000003;

volatile unsigned int IPU_BASE = (unsigned int ) 0x44A40000;
// Specify the SSID
const char *szSsid = "ece532group6";
// Select 1 for the security you want, or none for no security
#define USE_WPA2_PASSPHRASE
//#define USE_WPA2_KEY
//#define USE_WEP40
//#define USE_WEP104
//#define USE_WF_CONFIG_H

// Modify the security key to what you have.
#if defined(USE_WPA2_PASSPHRASE)

   const char *szPassPhrase = "12345678";
   #define WiFiConnectMacro() deIPcK.wfConnect(szSsid, szPassPhrase, &status)

#elif defined(USE_WPA2_KEY)

   WPA2KEY key = { 0x27, 0x2C, 0x89, 0xCC, 0xE9, 0x56, 0x31, 0x1E,
                   0x3B, 0xAD, 0x79, 0xF7, 0x1D, 0xC4, 0xB9, 0x05,
                   0x7A, 0x34, 0x4C, 0x3E, 0xB5, 0xFA, 0x38, 0xC2,
                   0x0F, 0x0A, 0xB0, 0x90, 0xDC, 0x62, 0xAD, 0x58 };
   #define WiFiConnectMacro() deIPcK.wfConnect(szSsid, key, &status)

#elif defined(USE_WEP40)

   const int iWEPKey = 0;
   WEP40KEY keySet = { 0xBE, 0xC9, 0x58, 0x06, 0x97,   // Key 0
                       0x00, 0x00, 0x00, 0x00, 0x00,   // Key 1
                       0x00, 0x00, 0x00, 0x00, 0x00,   // Key 2
                       0x00, 0x00, 0x00, 0x00, 0x00 }; // Key 3
   #define WiFiConnectMacro() deIPcK.wfConnect(szSsid, keySet, iWEPKey, &status)

#elif defined(USE_WEP104)

   const int iWEPKey = 0;
   WEP104KEY keySet = { 0x3E, 0xCD, 0x30, 0xB2, 0x55, 0x2D, 0x3C, 0x50, 0x52, 0x71, 0xE8, 0x83, 0x91,   // Key 0
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // Key 1
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // Key 2
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; // Key 3
   #define WiFiConnectMacro() deIPcK.wfConnect(szSsid, keySet, iWEPKey, &status)

#elif defined(USE_WF_CONFIG_H)

   #define WiFiConnectMacro() deIPcK.wfConnect(0, &status)

#else // No security - OPEN

   #define WiFiConnectMacro() deIPcK.wfConnect(szSsid, &status)

#endif

//******************************************************************************************
//******************************************************************************************
//***************************** END OF CONFIGURATION ***************************************
//******************************************************************************************
//******************************************************************************************

typedef enum {
   NONE = 0,
   CONNECT,
   TCPCONNECT,
   RESOLVEENDPOINT,
   WRITE,
   WRITE_2,
   CALIB,
   WAIT,
   START,
   CHECKWRITE,
   CHECKWRITE_2,
   READ,
   READ_2,
   CLOSE,
   DONE,
} STATE;

STATE state = CONNECT;

unsigned tStart = 0;
unsigned tWait = 5000;

TCPSocket tcpSocket;

// This is for Print.write to print
byte rgbWrite[] = {'G','G', 'G'};
int cbWrite = sizeof(rgbWrite);

const unsigned width = 320;
const unsigned height = 240;
// This is for tcpSocket.writeStream to print
u8 rgbWriteStream[width*2+4];// = {'*','W','r','o','t','e',' ','f','r','o','m',' ','t','c','p','C','l','i','e','n','t','.','w','r','i','t','e','S','t','r','e','a','m','*','\n'};
int cbWriteStream = sizeof(rgbWriteStream);
u8 headerWriteStream[] = {'I', 'M', 'A', 'G', 'E', ' ', '3','2','0','x','2','4','0', '\n'};
int sizeHeaderWriteStream = sizeof(headerWriteStream);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Must have a datagram cache
UDPSocket udpClient;

// Our sketch datagram buffer
byte rgbRead[1024];

u8 blobDatagram[9] ;
int sizeBlobDatagram = sizeof(blobDatagram);


// This is for udpClient.writeDatagram to write
byte rgbWriteDatagram[] = {'*','W','r','o','t','e',' ','f','r','o','m',' ','u','d','p','C','l','i','e','n','t','.','w','r','i','t','e','D','a','t','a','g','r','a','m','*','\n'};
int cbWriteDatagram = sizeof(rgbWriteDatagram);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

long blobs[500][7];
int num_blobs = 0;
void blob_detect(int row) {
	long xal, xar, xbr, xbl;
	volatile unsigned int  runs_head;
	long value;
	int num_runs;
	runs_head = (unsigned int)((unsigned int *)IPU_BASE + BLOBS_MAP_BASE);
	num_runs = *((unsigned int *)IPU_BASE + ROW_0_NUM );
	if (row == 0) {
		for (unsigned i; i < num_runs; i++) {
			blobs[i][0] = 1;
			blobs[i][1] = 0;						//top
			blobs[i][2] = 0;						//bottom
			value = (*(unsigned int *)(IPU_BASE+runs_head+i*4));
			blobs[i][3] = value%(256*256);  //last row left

			blobs[i][4] = value/(256*256);  //last row right
			blobs[i][5] = value%(256*256);	 //min row left

			blobs[i][6] = value/(256*256);	 //max row right
			num_blobs += 1;


		}
	}
	else if (row != 0) {
		runs_head = (*((unsigned int *)IPU_BASE + BLOBS_MAP_BASE+2*row));
		num_runs = *((unsigned int *)IPU_BASE + ROW_0_NUM + 2*row);
		if (num_runs > 0) {
			for (int i = 0; i < num_runs; i++) {
				bool new_blob = true;
				value = (*(unsigned int *)(IPU_BASE+runs_head+i*4));
				xal = value%(256*256);
				xar = value/(256*256);
				xil_printf("run %d %d %d %d/%d\r\n", xal, xar, row, i, num_runs);
				for (unsigned ii = 0; ii < num_blobs; ii++) {
					xbl = blobs[ii][3];
					xbr = blobs[ii][4];
					if (blobs[ii][2] == row - 1  && new_blob) {
						if (!(xbl > xar) && !(xbr < xal))
						{
							new_blob = false;
							blobs[ii][2] = row;
							blobs[ii][3] = xal;
							blobs[ii][4] = xar;
							blobs[ii][3] = min(xal, xbl);
							blobs[ii][4] = max(xar, xbr);
							blobs[ii][5] = min(xal, xbl);
							blobs[ii][6] = max(xar, xbr);

						}
					}
					if (blobs[ii][2] == row  && !new_blob) {
						if (!(xbl > blobs[ii][4]) && !(xbr < blobs[ii][3]))
						{
							blobs[ii][2] = row;
							blobs[ii][3] = xal;
							blobs[ii][4] = xar;
							blobs[ii][3] = min(xal, xbl);
							blobs[ii][4] = max(xar, xbr);
							blobs[ii][5] = min(xal, xbl);
							blobs[ii][6] = max(xar, xbr);

						}
					}

				}

				if (new_blob) {
					blobs[num_blobs][0] = 1;
					blobs[num_blobs][1] = row;						//top
					blobs[num_blobs][2] = row;						//bottom
					blobs[num_blobs][3] = xal;  //last row left
					blobs[num_blobs][4] = xar;  //last row right
					blobs[num_blobs][5] = xal;	 //min row left
					blobs[num_blobs][6] = xar;	 //max row right
					num_blobs += 1;

				}

			}
		}
	}

}

IPSTATUS status = ipsSuccess;
IPEndPoint epRemote;

void DemoInitialize();
void DemoRun();


int main(void) {
   Xil_ICacheEnable();
   Xil_DCacheEnable();

   xil_printf("Connecting to network...\r\n");
   DemoInitialize();
   DemoRun();
   return 0;
}

void DemoInitialize() {
   ALS_begin(&ALS, XPAR_PMODALS_0_AXI_LITE_SPI_BASEADDR);
   setPmodWifiAddresses(
      XPAR_PMODWIFI_0_AXI_LITE_SPI_BASEADDR,
      XPAR_PMODWIFI_0_AXI_LITE_WFGPIO_BASEADDR,
      XPAR_PMODWIFI_0_AXI_LITE_WFCS_BASEADDR,
      XPAR_PMODWIFI_0_S_AXI_TIMER_BASEADDR
   );
   setPmodWifiIntVector(PMODWIFI_VEC_ID);
   *((unsigned int *)IPU_BASE+ IB_ADDR_BASE) = (unsigned int)FRAME_BUFFER_BASE_ADDR;
   *((unsigned int *)IPU_BASE+ FRAME_SIZE)= 0x00F00140;
}

void DemoRun() {
   int cbRead = 0;
   int i = 0;
   unsigned counter = 0;
   long a;
   u8 light;
   unsigned j = 0;
   u8 checksums[height];
   u8 check_checksum;
   int ipu_state = 0;
   int check_i;
   int ipu_status = 0;
   long max_area;
   unsigned max_index;
   unsigned counterx = 0;
   while (1) {

	   //if (counterx %255 == 0) {
		  light = ALS_read(&ALS);
		  ipu_status = *((unsigned int *)IPU_BASE + OP_STATUS);
		  if (ipu_state ==1 && ipu_status%2 == 1) {
			  ipu_state =2;
		  } else if (ipu_state == 3 && ( ipu_status /2)%2 == 1) {
			  blob_detect(counterx);

			   counterx++;
			   //processed all rows
			  if (counterx == 239) {
				  ipu_state = 4;
				  counterx = 0;
				  max_area = 0;
				  max_index = 0;

				  //print out blob information
				  for (unsigned ii = 0; ii < num_blobs; ii++){
						xil_printf("blob %d %d %d %d\r\n", blobs[ii][1], blobs[ii][2], blobs[ii][5], blobs[ii][6]);
					  long area = (blobs[ii][2] - blobs[ii][1])*(blobs[ii][6] - blobs[ii][5]);
					  if (max_area < (area)) {
						  max_area = area;
						  max_index = ii;
					  }
				  }
				  blobDatagram[0] = 'b';
				  blobDatagram[1] = (u8)blobs[max_index][5]/256;
				  blobDatagram[2] = (u8)blobs[max_index][5]%256;
				  blobDatagram[3] = (u8)blobs[max_index][1]/256;
				  blobDatagram[4] = (u8)blobs[max_index][1]%256;
				  blobDatagram[5] = (u8)(blobs[max_index][6] - blobs[max_index][5])/256;
				  blobDatagram[6] = (u8)(blobs[max_index][6] - blobs[max_index][5])%256;
				  blobDatagram[7] = (u8)(blobs[max_index][2] - blobs[max_index][1])/256;
				  blobDatagram[8] = (u8)(blobs[max_index][2] - blobs[max_index][1])%256;
		        	udpClient.writeDatagram(blobDatagram, sizeBlobDatagram);

		            xil_printf("num_blobs: %d\r\n", num_blobs);
			  }

		  }
	   //}
      switch (state) {
      case CONNECT:
         if (WiFiConnectMacro()) {
         	   *((unsigned int *)IPU_BASE+ OP_CTRL)= 0x000601;
         	  ipu_state =1;
            xil_printf("WiFi connected\r\n");
            deIPcK.begin();
            state = RESOLVEENDPOINT;
         } else if (IsIPStatusAnError(status)) {
            xil_printf("Unable to connect, status: 0x%X\r\n", status);
            state = CLOSE;
         }
         break;

      case RESOLVEENDPOINT:
         if (deIPcK.resolveEndPoint(szIPServer, portServer, epRemote,
               &status)) {
            if (deIPcK.udpSetEndPoint(epRemote, udpClient,
                  portDynamicallyAssign, &status)) {
            	//state = START;
               //state = CALIB;
            	state = WRITE;

            }
         }

         // Always check the status and get out on error
         if (IsIPStatusAnError(status)) {
            xil_printf("Unable to resolve endpoint, error: 0x%X\r\n", status);
            state = CLOSE;
         }
         break;

      // Write out the strings
      case WRITE:

    	  //fridge open, time to start image detection
  		  if (light > 70 && ipu_state == 2) {
  			ipu_state = 3;
			  *((unsigned int *)IPU_BASE+OP_CTRL )= 0xa0402;
  		  }
         if (deIPcK.isIPReady(&status)) {
        	 // send rows
        	 checksums[i] = 0;
        	for (j = 0; j < width; j++) {
        		a = (*(FRAME_BUFFER_BASE_ADDR+(i*width + j)));
        		rgbWriteStream[(j+1)*2+1] = (u8)(a%256);
        		rgbWriteStream[(j+1)*2+2] = (u8)(a/256);
        		checksums[i] = checksums[i]^((u8)(a%256))^((u8)(a/256));

        	}

        	rgbWriteStream[0] = 'a';
        	rgbWriteStream[1] = (u8)(i/256);
        	rgbWriteStream[2] = (u8)(i%256);

        	rgbWriteStream[width*2+3] = (u8)checksums[i];
        	udpClient.writeDatagram(rgbWriteStream, cbWriteStream);
            state = CHECKWRITE;
         } else if (IsIPStatusAnError(status)) {
            xil_printf("Lost the network, error: 0x%X\r\n", status);
            state = CLOSE;
         }
         break;
      case WRITE_2:

    	  //fridge closed
  		 if (light < 5 && ipu_state == 4) {
			  ipu_state =2;
  		  }

         if (deIPcK.isIPReady(&status)) {

        	 checksums[i] = 0;

        	for (j = 0; j < width; j++) {
        		unsigned int * addr = (unsigned int *)IPU_BASE+PRCD_IMG_BASE+((i*width + j)/32);
        		unsigned int pix= *(addr);
        		unsigned diff = ((pix>>((i*width+j)%32))%2);

        		rgbWriteStream[(j+1)*2+1] = (u8)(diff*255);
        		rgbWriteStream[(j+1)*2+2] = (u8)(diff*15);
        		checksums[i] = checksums[i]^((u8)(diff*255))^((u8)(diff*15));

        	}
        	rgbWriteStream[0] = 'a';
        	rgbWriteStream[1] = (u8)(i/256);
        	rgbWriteStream[2] = (u8)(i%256);

        	rgbWriteStream[width*2+3] = (u8)checksums[i];
        	udpClient.writeDatagram(rgbWriteStream, cbWriteStream);
            state = CHECKWRITE_2;


         } else if (IsIPStatusAnError(status)) {
            xil_printf("Lost the network, error: 0x%X\r\n", status);
            state = CLOSE;
         }
         break;
      case CHECKWRITE:

          if ((cbRead = udpClient.available()) > 0) {

             cbRead = cbRead < (int) sizeof(rgbRead) ? cbRead : sizeof(rgbRead);
             cbRead = udpClient.readDatagram(rgbRead, cbRead);
             check_checksum = rgbRead[0];
             check_i = rgbRead[1]*256+rgbRead[2];

             if (check_checksum == checksums[check_i])
             {
            	 i = (i+1)%height;
             }
             else
             {
            	 //xil_printf("%u %u %u %u %u %u \r\n", rgbRead, checksums[check_i], check_checksum, i, rgbRead[1], rgbRead[2]);
             }
             rgbRead[cbRead] = 0;

         	if (i == height-1 && ipu_state == 4) {
         		state = WRITE_2;

         	} else {
             state = WRITE;
         	}
          } else
          {
        	  state = WRITE;
          }
          break;
      case CHECKWRITE_2:

          if ((cbRead = udpClient.available()) > 0) {

             cbRead = cbRead < (int) sizeof(rgbRead) ? cbRead : sizeof(rgbRead);
             cbRead = udpClient.readDatagram(rgbRead, cbRead);
             check_checksum = rgbRead[0];
             check_i = rgbRead[1]*256+rgbRead[2];

             if (check_checksum == checksums[check_i])
             {
            	 i = (i+1)%height;
             }
             else
             {
            	 xil_printf("%u %u %u %u %u %u out of order \r\n", rgbRead, checksums[check_i], check_checksum, i, rgbRead[1], rgbRead[2]);
             }
             rgbRead[cbRead] = 0;
             if (i == height-1) {
				state = WRITE;
			} else {
             state = WRITE_2;
                      	}
          } else
          {
        	  state = WRITE_2;
          }


         break;
      case CALIB:

    	  break;
      case START:
          if (deIPcK.isIPReady(&status)) {

         	udpClient.writeDatagram(rgbWrite, cbWrite);
             state = WAIT;

             tStart = (unsigned) SYSGetMilliSecond();
          }

    	  break;
      case WAIT:
          if ((cbRead = udpClient.available()) > 0) {

             cbRead = cbRead < (int) sizeof(rgbRead) ? cbRead : sizeof(rgbRead);
             cbRead = udpClient.readDatagram(rgbRead, cbRead);
             rgbRead[cbRead] = 0;


             state = WRITE;
          }

    	  break;

      // Look for the echo back
      case READ:
         // See if we got anything to read
         if ((cbRead = udpClient.available()) > 0) {

            cbRead = cbRead < (int) sizeof(rgbRead) ? cbRead : sizeof(rgbRead);
            cbRead = udpClient.readDatagram(rgbRead, cbRead);
            rgbRead[cbRead] = 0;

            xil_printf("%s\r\n", rgbRead);

            // Give us some more time to wait for stuff to come back
            //tStart = (unsigned) SYSGetMilliSecond();
            state = WRITE;
         }


         break;
      // Done, so close up the tcpClient
      case CLOSE:
         udpClient.close();
         xil_printf("Closing udpClient, Done with sketch.\r\n");
         state = DONE;
         break;

      case DONE:
    	  break;

      case NONE:
      	  break;

      default:
         break;
      }

      // Keep the stack alive each pass through the loop()
      DEIPcK::periodicTasks();
   }
}


