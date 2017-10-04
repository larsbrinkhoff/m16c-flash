/***************************************************************************
                         	cmotfile.h  -  header file for cmotfile.cxx.
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
#include <stdlib.h>
#include <assert.h>

#include "cpage.h"

class CMOTFile {

 public:
  CMOTFile();
  ~CMOTFile();

  int readFile(const char* pszFilename, int nPageSize);

  inline CPage* getFirstPage();

 protected:

  int processFile(int nPageSize);
  CPage* getPageToAddress(int nPageSize, long lAddress);
  
  FILE* m_fp;
  CPage* m_pHead;
  CPage* m_pTail;

};

inline CPage* CMOTFile::getFirstPage()
{
  return m_pHead;
}

