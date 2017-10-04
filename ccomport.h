/***************************************************************************
                          ccomport.h - header file for serial port comms
                          							class.
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
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <assert.h>

class CCOMPort {

 public:

  CCOMPort();
  ~CCOMPort();

  int openPort(const char* pszDev);
  int setPortOptions(speed_t spdBaud);
  int readBytes(unsigned char* pBuffer, int nBytes);
  int sendBytes(const unsigned char* pBuffer, int nBytes);
  int closePort();
  int flush();

 protected:
  int m_fd;                         /* file descriptor for port */
  termios m_trmOptions;             /* port options */


};
