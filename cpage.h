/***************************************************************************
                          cpage.h  -  header file for cpage.cxx.
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
#include <assert.h>
#include <string.h>
#include <stdlib.h>

class CPage {

 public:
  CPage(int nSize);
  ~CPage();
  void display();


  inline void setAddress(long lAddress);
  inline long getAddress();
  inline long getSize();
  inline void setByte(int nIndex, unsigned char ucVal);
  inline unsigned char getByte(int nIndex);
  inline void setNext(CPage* pNext);
  inline CPage* getNext();
  inline unsigned char* getPage();

 protected:
  long m_lAddress;
  int m_nSize;
  unsigned char* m_pData;

  CPage* m_pNext;
};

inline unsigned char* CPage::getPage()
{
  assert( m_pData );
  return m_pData;
}

inline void CPage::setNext(CPage* pNext)
{
  assert( pNext );
  m_pNext = pNext;
}

inline CPage* CPage::getNext()
{
  return m_pNext;
}

inline unsigned char CPage::getByte(int nIndex)
{
  assert( nIndex < m_nSize );
  return m_pData[nIndex];
}

inline void CPage::setByte(int nIndex, unsigned char ucVal)
{
  assert( nIndex < m_nSize );
  m_pData[nIndex] = ucVal;
}

inline void CPage::setAddress(long lAddress)
{
  assert( lAddress >= 0 );
  m_lAddress = lAddress;
}

inline long CPage::getAddress()
{
  return m_lAddress;
}

inline long CPage::getSize()
{
  return m_nSize;
}

