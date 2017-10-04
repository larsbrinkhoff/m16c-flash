/***************************************************************************
    cmotfile.cxx - class for handling Motorola files.

    begin                : Wed Sep 25 18:58:35 BST 2002
    copyright            : (C) 2002 by Greg Reynolds, Jim Hawkridge
    email                : j114@eee-fs4.bham.ac.uk

    2005-11-17: modified by Thomas Fischl <tfischl@gmx.de>
                - added support for S1-Files
                - new parse method

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "cmotfile.h"

CMOTFile::CMOTFile()
{
  m_fp = NULL;
  m_pHead = NULL;
  m_pTail = NULL;
}

CMOTFile::~CMOTFile()
{

}

int CMOTFile::readFile(const char* pszFilename, int nPageSize)
{
  assert( pszFilename );

  unsigned int nType;
  unsigned int nLength;
 
  m_fp = fopen(pszFilename, "r");

  if( m_fp == NULL ) {
    printf("\nM16 Flash ERROR: couldn't open specified file.");
    fflush(stdout);
    return 0;
  }
 
  if( fscanf(m_fp, "S%1u%2X", &nType, &nLength) != 2 ) {
    printf("\nM16 Flash ERROR: badly formatted S-record file.");
    fflush(stdout);
    fclose(m_fp);
    return 0;
  }

  if( nType != 0 ) {
    printf("\nM16 Flash ERROR: there must be an S0 record (a dummy will do).");
    fflush(stdout);
    fclose(m_fp);
    return 0;
  }

  /* if we got here then there is an S0 record which is a good start */
  /* advance m_fp to point to the start of the next line */
  fscanf(m_fp, "%*[^\n]s");
  fscanf(m_fp, "\n");

  /* we should check for \r\n in case S records created on windows */
  
  /* now read the file into our linked list of pages */
  if( processFile(nPageSize) == 0 )
    return 0;
  else
    return 1;
}

/* return page which belongs to given address. NULL if page does not exist */
CPage* CMOTFile::getPageToAddress(int nPageSize, long lAddress)
{
  long lBaseAddress = lAddress - (lAddress % nPageSize);

  /* if no page in list */
  if (m_pTail == NULL) return NULL;
  
  if (m_pTail->getAddress() == lBaseAddress) return m_pTail;

  /* search for page with given base address */
  CPage* pCurrent = m_pHead;
  do {
    if (pCurrent->getAddress() == lBaseAddress) return pCurrent;
    pCurrent = pCurrent->getNext();
  } while (pCurrent != NULL);
    
  /* no page found */
  return NULL;  
}

/* convert the file contents into a linked list of pPages */
int CMOTFile::processFile(int nPageSize)
{
  assert( m_fp );
  assert( nPageSize > 0 );

  unsigned int nType;
  unsigned int nLength;
  unsigned long lAddress;
  unsigned int nVal;
  unsigned int n;


  while( true ) {

    /* read a line's header data */
    if( fscanf(m_fp, "S%1u%2X", &nType, &nLength) != 2 ) {
      printf("\nM16 Flash ERROR: badly formatted input file.");
      fflush(stdout);
      return 0;
    }

    /* extract address */
    switch (nType) {
    case 1:
      // 16 bit addresses
      if( fscanf(m_fp, "%4lX", &lAddress) != 1 ) {
	printf("\nM16 Flash ERROR: bad address format.");
	fflush(stdout);
	return 0;
      }
      nLength-=3;
      break;
    case 2:
      // 24 bit addresses
      if( fscanf(m_fp, "%6lX", &lAddress) != 1 ) {
	printf("\nM16 Flash ERROR: bad address format.");
	fflush(stdout);
	return 0;
      }
      nLength-=4;
      break;
    default:
      // no address
      break;
    }

    /* process file */
    switch (nType) {
    case 1:
    case 2:
      
      CPage* page;
      /* now read the data */
      for( n = 0; n < nLength; n++ ) {
      
	if( fscanf(m_fp, "%2X", &nVal) != 1 ) {
	  printf("\nM16 Flash ERROR: bad data record in file.");
	  fflush(stdout);
	  fclose(m_fp);
	  return 0;
	}

	page = getPageToAddress(nPageSize, lAddress);
	if (page == NULL) {
	  /* new page */

	  page = new CPage(nPageSize);
	  page->setAddress(lAddress - (lAddress % nPageSize));

	  if (m_pHead == NULL) m_pHead = page;
	  else m_pTail->setNext(page);

	  m_pTail = page;
	}

	//	printf("adr: %6lX   base: %6lX   val: %2X\n", lAddress, page->getAddress(), nVal);
	page->setByte(lAddress % nPageSize, nVal);
	
	lAddress++;
      }
      
      break;
      
    case 8:
    case 9:
      /* S8 and S9 indicates end of file, so clean exit */
      fclose(m_fp);
      return 8;
      break;
    case 0:
      /* header do nothing */
      break;
    default:
      /* unknown record type */
      printf("\nM16 Flash ERROR: unknown record type");
      fflush(stdout);
      fclose(m_fp);
      return 0;      
    }

    /* loose the checksum and end of line */
    fscanf(m_fp, "%*[^\n]");
    fscanf(m_fp, "\n");    
  }
}
