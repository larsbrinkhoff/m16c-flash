/***************************************************************************
                          cm16flash.h - header for cm16cflash.cxx.
                             -------------------
    begin                : Wed Sep 25 18:58:35 BST 2002
    copyright            : (C) 2002 by Greg Reynolds, Jim Hawkridge
    email                : j114@eee-fs4.bham.ac.uk
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

This class provides access to all flash ROM functions of the M16C's default
boot loader.

*/

#include "ccomport.h"

#define ADDRESS_HIGH(x) ((x >> 16) & 0xFF)
#define ADDRESS_MIDDLE(x) ((x >> 8) & 0xFF)
#define ADDRESS_LOW(x) ((x) & 0xFF)

#define M16_CPU 1
#define R8C_CPU 2

/* programmer specific defs */
#define M16_CONFIRM_CLOCK 0xB0
#define M16_DELAY 15000 // 15ms
#define M16_READ_PAGE 0xFF
#define M16_PAGE_WRITE 0x41
#define M16_PAGE_SIZE 256
#define M16_READ_SRD 0x70
#define M16_CLEAR_SRD 0x50
#define M16_READ_VERSION_INFO 0xFB
#define M16_ID_CHECK 0xF5
#define M16_ID_ADDRESS 0x0FFFDF
#define M16_ID_SIZE 0x07
#define M16_SR11 0x08
#define M16_SR10 0x04
#define M16_LOCK_DISABLE 0x75
#define M16_ERASE_BLOCK 0x20
#define M16_ERASE_VERIFY 0xD0

#define M16_BLOCK_COUNT 7
#define M16_BLOCK6_BEGIN 0x0C0000
#define M16_BLOCK5_BEGIN 0x0D0000
#define M16_BLOCK4_BEGIN 0x0E0000
#define M16_BLOCK3_BEGIN 0x0F0000
#define M16_BLOCK2_BEGIN 0x0F8000
#define M16_BLOCK1_BEGIN 0x0FA000
#define M16_BLOCK0_BEGIN 0x0FC000

#define R8C_ID_ADDRESS 0x00FFDF
#define R8C_BLOCK_COUNT 2
#define R8C_BLOCK0_BEGIN 0x0E000
#define R8C_BLOCK1_BEGIN 0x0C000

#include "cmotfile.h"

class CM16Flash {

 public:
  CM16Flash(CCOMPort* pPort, unsigned char cuCPUType);
  ~CM16Flash();

  int initComms();
  int readPage(CPage* pPage);
  int readStatusRegisters(unsigned char* pSRD);
  int clearStatusRegisters();
  int readVersionInfo(char* pszVersionInfo);
  int sendID(unsigned int pIDCode[7]);
  int writePage(CPage* pPage);
  int eraseBlock(long lAddress);
  int eraseChip();

 protected:
  int sendCommandByte(unsigned char ucCommand);
  int sendByte(unsigned char ucVal);

  CCOMPort* m_pPort;
 
  unsigned char m_CPUType;

};



