/***************************************************************************
                flashm16.cxx  -  A flash programmer for an M16C/62.
                             -------------------

    begin                : Wed Sep 25 18:58:35 BST 2002
    copyright            : (C) 2002 by Greg Reynolds, Jim Hawkridge
    email                : j114@eee-fs4.bham.ac.uk

 
    2006-08-22: set version to "0.1"
    2005-11-18: modified by Thomas Fischl <tfischl@gmx.de>
                - added support for R8C                
                - added support for S1-mot-files
    2005-11-20: - minor fixes
                - show new ID when written

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


/*

A flash programmer for the M16C/62.

GMR 06-September-2002 with (as ever)inspired additions by Jim on 18-September-2002.

TTDs:
Make sure the COM port times out after a while - rather than waiting forever.

Make sure that the status registers are checked after every operation - not done at the
moment.

Give the user the option to change the baud rate, currently defaults to the safe but
slow 9600.

Give the user the option to read back pages and verify them against what was supposed
to have been written (all the code for reading has been written, just simple stuff
required).

Embed the PIC16F84 programmer source into this.

Add all the functions that can be used in the Windows version.
*/

#include "cm16flash.h"

#define VERSION "0.1"

void displayUsage(void) 
{
  printf("Programs an M16C/62 or R8C microcontroller in asynchronous mode.\n\n");
  printf("Usage: flash [COM_DEV] [CPU_TYPE] [FILE] [ID1:...:ID7] [OPTIONS]\n");
  printf("Examples:\n");
  printf("\t flash /dev/ttyS1 M16C simple.s 0:0:0:0:0:0:0\n");
  printf("\t flash /dev/ttyS1 R8C test.mot ff:ff:ff:ff:ff:ff:ff\n\n");
  fflush(stdout);
}

int main(int argc, char* argv[])
{
  CCOMPort port;
  unsigned char ucSRDs[2];
  char szVersion[9];
  unsigned int nID[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  CMOTFile mot;
  CPage* p;
  unsigned char ucCPUType = 0;

  printf("M16C-Flash Programmer Version %s\n", VERSION);
  fflush(stdout);

  /* check for sufficient parameters */
  if( argc < 5 ) {
    displayUsage();
    exit(-1);
  }

  /* check for a COM port device */
  if( port.openPort(argv[1]) == 0 ) {
    printf("\nM16 Flash Error: could not open specified serial device.\n");
    fflush(stdout);
    exit(-1);
  }
  
  /* now check the CPU type */
  if( strcmp(argv[2], "M16C") == 0 ) {
    ucCPUType = M16_CPU;
  } else if( strcmp(argv[2], "R8C") == 0 ) {
    ucCPUType = R8C_CPU;
  }

  if (ucCPUType == 0) {
    printf("\nM16 Fash Error: you must specify a cpu type.\n");
    fflush(stdout);
    exit(-1);
  }

  /* now try and read the file they have given us */
  if( mot.readFile(argv[3], M16_PAGE_SIZE) == 0 ) {
    printf("\nM16 Flash Error: the input file was not acceptable.\n");
    fflush(stdout);

    /* free up the pages so we can make a clean exit */
    if( mot.getFirstPage() != NULL )   
      delete mot.getFirstPage();

    port.closePort();
    exit(-1);
  }

  /* get the ID */
  if( sscanf(argv[4], "%2X:%2X:%2X:%2X:%2X:%2X:%2X", 
		&nID[0],
		&nID[1],
		&nID[2],
		&nID[3],
		&nID[4],
		&nID[5],
		&nID[6]) != 7 ) {

    printf("\nM16 Flash Error: badly formatted ID string.\n");
    fflush(stdout);

    /* free up the pages so we can make a clean exit */
    if( mot.getFirstPage() != NULL )   
      delete mot.getFirstPage();

    port.closePort();
    exit(-1);

  }

  /* create the CM16Flash object */
  CM16Flash flash(&port, ucCPUType);

  /* do the initial probing for the device */
  if( flash.initComms() == 0 ) {

    /* free up the pages so we can make a clean exit */
    if( mot.getFirstPage() != NULL )   
      delete mot.getFirstPage();

    port.closePort();
    exit(-1);
  }

  /* get status registers */
  if( flash.readStatusRegisters(ucSRDs) == 0 ) {
    printf("\nM16 Flash Error: could not read status registers.\n");
    fflush(stdout);

    /* free up the pages so we can make a clean exit */
    if( mot.getFirstPage() != NULL )   
      delete mot.getFirstPage();

    port.closePort();
    exit(-1);
  }

  /* get boot ROM version info */
  if( flash.readVersionInfo(szVersion) == 0 ) {
    printf("\nM16 Flash Error: couldn't read boot loader ROM code version.\n");
    fflush(stdout);

    /* free up the pages so we can make a clean exit */
    if( mot.getFirstPage() != NULL )   
      delete mot.getFirstPage();

    port.closePort();
    exit(-1);
  }

  /* send ID check */
  if( flash.sendID(nID) == 0 ) {
    printf("\nM16 Flash Error: ID check failed.\n");
    fflush(stdout);
    
    /* free up the pages so we can make a clean exit */
    if( mot.getFirstPage() != NULL )   
      delete mot.getFirstPage();

    port.closePort();
    exit(-1);
  }

  /* erase the chip */
  if( flash.eraseChip() == 0 ) {
    printf("\nM16 Flash Error: erasing of all blocks failed.\n");
    fflush(stdout);

    /* free up the pages so we can make a clean exit */
    if( mot.getFirstPage() != NULL )   
      delete mot.getFirstPage();

    port.closePort();
    exit(-1);
  }
  
  /* this is the loop that does the writing */
  p = mot.getFirstPage();
  while (p != NULL) {
    flash.writePage(p);
    p = p->getNext();
  }
  
  /* free up the pages so we can make a clean exit */
  if( mot.getFirstPage() != NULL )   
    delete mot.getFirstPage();

  port.closePort();

  printf("\nfinished.\n");

  return 0;
}
