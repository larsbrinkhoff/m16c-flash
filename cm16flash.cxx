/***************************************************************************
    cm16flash - class for handling flash programming of an M16C 
    microprocessor.
    
    begin                : Wed Sep 25 18:58:35 BST 2002
    copyright            : (C) 2002 by Greg Reynolds, Jim Hawkridge
    email                : j114@eee-fs4.bham.ac.uk

    2005-11-18: modified by Thomas Fischl <tfischl@gmx.de>
                - added support for R8C             
    2005-11-20: - show new ID   

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "cm16flash.h"

CM16Flash::CM16Flash(CCOMPort* pPort, unsigned char cuCPUType)
{
  assert( pPort );

  m_pPort = pPort;
  m_CPUType = cuCPUType;
}

CM16Flash::~CM16Flash()
{

}

/* send a byte with the minimum delay */
int CM16Flash::sendCommandByte(unsigned char ucCommand)
{
  assert( m_pPort );

  /* apparently this is important */
  usleep(M16_DELAY);

  m_pPort->sendBytes(&ucCommand, 1);
  
  /* this can only succeed */
  return 1;
}

/* send a byte without delay */
int CM16Flash::sendByte(unsigned char ucVal)
{
  assert( m_pPort );
  m_pPort->sendBytes(&ucVal, 1);
  
 /* this can only succeed */
  return 1;
}

/* do the initial comms as per the datasheet page 257 */
int CM16Flash::initComms()
{
  assert( m_pPort );

  unsigned char ucReply;
  unsigned char ucSRDs[2];
  int n;

  /* set the board rate to required */
  m_pPort->setPortOptions(B9600);

  /* now follow what the datasheet says */
  printf("\nLooking for M16C/R8C...");
  fflush(stdout);
  sendCommandByte(M16_CONFIRM_CLOCK);
  
  usleep(M16_DELAY);

  printf("\nConfirming...");
  fflush(stdout);

  /* send the 16 0s */
  for( n = 0; n < 16; n++ ) {
    sendCommandByte(0);
  }
  
  usleep(M16_DELAY);
  m_pPort->flush();
  sendCommandByte(M16_CONFIRM_CLOCK);
  
  if( m_pPort->readBytes(&ucReply, 1) == 0 ) {
    printf("\nM16 Flash ERROR: the board did not respond.");
    fflush(stdout);
    return 0;
  }

  if( M16_CONFIRM_CLOCK == ucReply ) {
    printf("board replied with clock confirm of %X.", ucReply);
    fflush(stdout);
  }
  else {
    printf("board does not have a 10MHz or 16MHz clock, quitting.");
    fflush(stdout);
    return 0;
  }
  
  if( readStatusRegisters(ucSRDs) == 0 ) {
    printf("\nM16 Flash ERROR: failed reading back SRDs.");
    fflush(stdout);
    return 0;
  }

    printf("\nStatusregister: %2X, %2X\n", ucSRDs[0], ucSRDs[1]);

  return 1;
}

/* send it the ID so that we can send all other commands */
int CM16Flash::sendID(unsigned int pIDCode[7])
{
  long lAddress;
  int n;
  unsigned char ucSRDs[2];

  printf("\nSending ID data %2X:%2X:%2X:%2X:%2X:%2X:%2X...", pIDCode[0], 
	 pIDCode[1], pIDCode[2], pIDCode[3], pIDCode[4], pIDCode[5], 
	 pIDCode[6]);
  fflush(stdout);

  if (m_CPUType == R8C_CPU) {
    lAddress = R8C_ID_ADDRESS;
  } else {
    lAddress = M16_ID_ADDRESS;
  }
  
  /* we need to send 0x070FFFD4 according to the source (fanwp.pdp:138) */
  sendCommandByte(M16_ID_CHECK);
  sendByte(ADDRESS_LOW(lAddress));
  sendByte(ADDRESS_MIDDLE(lAddress));
  sendByte(ADDRESS_HIGH(lAddress));
    
  /* send size of ID bytes */
  sendByte(M16_ID_SIZE);

  /* send the ID bytes */
  for( n = 0; n < 7; n++ )
    sendByte(pIDCode[n]);

  printf("OK.");
  fflush(stdout);

  /* clear dstatus */
  sendCommandByte(0x50);

  /* read back SRDs to confirm ID was OK */
  if( readStatusRegisters(ucSRDs) == 0 ) {
    printf("\nM16 Flash ERROR: failed reading back SRDs after ID.");
    fflush(stdout);
    return 0;
  }

  /* SRD1 bit 3 and bit 2 are set if ID verified ok (62aeds.pdf:254) */
  if( ((ucSRDs[1] & M16_SR10) == 0) || ((ucSRDs[1] & M16_SR11) == 0) ) {
    printf("\nWARNING: ID check did not succeed. ");
    printf("Try cycling power or different ID.");
    fflush(stdout);
    return 0;
  }
  
  /* getting here is very good */
  printf("\nID verified, additional commands may now be issued.");
  fflush(stdout);
  return 1;
}

/* read back SRD0 and SRD1 */
int CM16Flash::readStatusRegisters(unsigned char* pSRD)
{
  assert( m_pPort );
  assert( pSRD );

  //printf("\nReading status registers...");
  //fflush(stdout);

  /* tell it we want a page of ROM at the address given */
  sendCommandByte(M16_READ_SRD);
  
  if( m_pPort->readBytes(pSRD, 2) == 0 ) {
    printf("\nM16 Flash ERROR: board timed out.");
    fflush(stdout);
    return 0;
  }

  //printf("OK.");
  //fflush(stdout);
  return 1;
}

/* clear SRD0 and SRD1 */
int CM16Flash::clearStatusRegisters()
{
  assert( m_pPort );
  
  sendCommandByte(M16_CLEAR_SRD);

  return 1;
}

int CM16Flash::readVersionInfo(char* pszVersionInfo)
{
  assert( m_pPort );
  assert( pszVersionInfo );

  printf("\nReading boot ROM version info... ");
  fflush(stdout);

  /* tell it we want a page of ROM at the address given */
  sendCommandByte(M16_READ_VERSION_INFO);
  
  pszVersionInfo[8] = 0; 
  
  if( m_pPort->readBytes((unsigned char*)pszVersionInfo, 8) == 0 ) {
    printf("\nM16 Flash ERROR: board timed out.");
    fflush(stdout);
    return 0;
  }
  
  printf(pszVersionInfo);
//  printf("OK.");
  fflush(stdout);
  return 1;
}

/* read a page of data back from the chip */
int CM16Flash::readPage(CPage* pPage)
{
  assert( m_pPort );
  assert( pPage );

  printf("\nReading page at %lX...", pPage->getAddress());
  fflush(stdout);

  /* tell it we want a page of ROM at the address given */
  sendCommandByte(M16_READ_PAGE);
  sendByte(ADDRESS_MIDDLE(pPage->getAddress()));
  sendByte(ADDRESS_HIGH(pPage->getAddress()));

  /* await a reply */
  if( m_pPort->readBytes(pPage->getPage(), M16_PAGE_SIZE) == 0 ) {
    printf("\nM16 Flash ERROR: board timed out.");
    perror("\n");
    fflush(stdout);
    return 0;
  }

  printf("OK.");

  return 1;
}

/* write a page of data to the chip, not erase must be capped first */
int CM16Flash::writePage(CPage* pPage)
{
  assert( pPage );
  assert( m_pPort );

  int n;
  unsigned char ucSRDs[2];

  /* disable the lock bit - this might not be necessary after an erase */
  sendCommandByte(M16_LOCK_DISABLE);
 
  clearStatusRegisters();
 
  if( readStatusRegisters(ucSRDs) == 0 )
    return 0;

  /* check for success here */
  //printf("\n\nSRD0 = %lX, SRD1 = %lX.", ucSRDs[0], ucSRDs[1]);
  //fflush(stdout);
  //printf("OK.");
  //fflush(stdout);

  printf("\nWriting page %6lX...", pPage->getAddress());
  fflush(stdout);

  /* we are writing a page */
  sendCommandByte(M16_PAGE_WRITE);
  sendByte(ADDRESS_MIDDLE(pPage->getAddress()));
  sendByte(ADDRESS_HIGH(pPage->getAddress()));

  for( n = 0; n < M16_PAGE_SIZE; n++ ) {
    sendByte(pPage->getByte(n));
    //      printf("\b\b\b\b\b\b\b\b\b%6lX...", pPage->getAddress() + n);
    //      fflush(stdout);
  }

  printf("OK.");
  if ((m_CPUType == R8C_CPU) && (pPage->getAddress() == 0x00FF00)) {
    printf(" New ID: %02X:%02X:%02X:%02X:%02X:%02X:%02X",
	   pPage->getByte(0xDF),
	   pPage->getByte(0xE3),
	   pPage->getByte(0xEB),
	   pPage->getByte(0xEF),
	   pPage->getByte(0xF3),
	   pPage->getByte(0xF7),
	   pPage->getByte(0xFB));
  }

  fflush(stdout);

  clearStatusRegisters();

  /* now check if it worked */
  if( readStatusRegisters(ucSRDs) == 0 )
    return 0;

  /* need to check value here */

  return 1;

}

/* erase a block, which is not the same size as a page, (62aeds.pdf:220) */
int CM16Flash::eraseBlock(long lAddress)
{
  assert( m_pPort );

  unsigned char ucSRDs[2];

  sendCommandByte(M16_LOCK_DISABLE);

  sendCommandByte(M16_ERASE_BLOCK);
  sendByte(ADDRESS_MIDDLE(lAddress));
  sendByte(ADDRESS_HIGH(lAddress));
  sendByte(M16_ERASE_VERIFY);
  
  /* status register check should be done here */
  readStatusRegisters(ucSRDs);

  return 1;
}

/* erase the entire chip */
int CM16Flash::eraseChip()
{
  int n;
  int count;
  int lBlocks[7];

  if (m_CPUType == R8C_CPU) {

    lBlocks[0] = R8C_BLOCK0_BEGIN;
    lBlocks[1] = R8C_BLOCK1_BEGIN;    
    count = R8C_BLOCK_COUNT;
  } else {

    lBlocks[6] = M16_BLOCK6_BEGIN;
    lBlocks[5] = M16_BLOCK5_BEGIN;
    lBlocks[4] = M16_BLOCK4_BEGIN;
    lBlocks[3] = M16_BLOCK3_BEGIN;
    lBlocks[2] = M16_BLOCK2_BEGIN;
    lBlocks[1] = M16_BLOCK1_BEGIN;
    lBlocks[0] = M16_BLOCK0_BEGIN;
    count = M16_BLOCK_COUNT;
  }

  /* erase every block on the chip */
  for( n = 0; n < count; n++ ) {
    
    /* we have to erase the page first */
    printf("\nErasing block at %6X...", lBlocks[n]);
    fflush(stdout);

    if( eraseBlock(lBlocks[n]) == 0 ) {
      printf("\nERROR.");
      fflush(stdout); 
      /* this needs some intelligence here */
      return 1;
    }

    printf("OK.");
    fflush(stdout);
  }

  return 1;
}
