/***************************************************************************
                          cpage.cxx  -  class for handling pages of embedded
                          								device memory.
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

#include <stdio.h>

#include "cpage.h"

/* give me the size of this page */
CPage::CPage(int nSize)
{
  assert( nSize > 0 );
  m_nSize = nSize;

  /* allocate some space for this page */
  m_pData = new unsigned char[m_nSize];
  memset(m_pData, 0xff, m_nSize);
 
  m_lAddress = 0x00;
  m_pNext = NULL;
}

CPage::~CPage()
{
  assert( m_pData );
  delete m_pData;

  if( m_pNext != NULL ) {
    delete m_pNext;
    m_pNext = NULL;
  }
}

/* display this page's contents */
void CPage::display()
{
  int n;

  printf("\n\nAddress %6lX = ", m_lAddress);
  printf("\n");

  for( n = 0; n < m_nSize; n++ )
    printf("%2X ", m_pData[n]);

  printf("\n\n----END OF PAGE----");

  fflush(stdout);
}
