/***************************************************************************
    ccomport.cxx  -  Class for handling serial comms.

    begin                : Wed Sep 25 18:58:35 BST 2002
    copyright            : (C) 2002 by Greg Reynolds, Jim Hawkridge
    email                : j114@eee-fs4.bham.ac.uk

    2005-11-17: changes by Thomas Fischl <tfischl@gmx.de>
                - added method flash()

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "ccomport.h"

CCOMPort::CCOMPort()
{
  m_fd = -1;
  memset(&m_trmOptions, 0, sizeof(termios));
}

CCOMPort::~CCOMPort()
{

}


/* open the port */
int CCOMPort::openPort(const char* pszDev)
{
  assert( pszDev );

  /* open the port */
  m_fd = open(pszDev, O_RDWR | O_NOCTTY | O_NDELAY);

  if( m_fd == -1 ) {
    perror("\nCCOMPort::openPort: Unable to open specified port.");
    return 0;
  }

  /* read is blocking */
  fcntl(m_fd, F_SETFL, 0);

  
  return 1;
}

/* close the port */
int CCOMPort::closePort()
{
  assert( m_fd != -1 );

  if( close(m_fd) == -1 ) {
    perror("\nCCOMPort::close:: Couldn't close the open port.");
    return 0;
  }

  return 1;
}

/* set baud rate */
int CCOMPort::setPortOptions(speed_t spdBaud)
{
  assert( m_fd != -1);

  /* retrieve the current options */
  tcgetattr(m_fd, &m_trmOptions);

  /* set baudrate */
  cfsetispeed(&m_trmOptions, spdBaud);
  cfsetospeed(&m_trmOptions, spdBaud);

  /* set 8 bit, no parity, 1 stop bit */
  m_trmOptions.c_cflag &= ~PARENB;
  m_trmOptions.c_cflag &= ~CSTOPB;
  m_trmOptions.c_cflag &= ~CSIZE;
  m_trmOptions.c_cflag |= CS8;

  /* enable software flow control  */
  m_trmOptions.c_iflag |= (IXON | IXOFF | IXANY);

  /* chose raw input */
  m_trmOptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  /* choose raw output */
  m_trmOptions.c_oflag &= ~OPOST;

  /* apply the options back to the port */
  tcsetattr(m_fd, TCSANOW, &m_trmOptions);

  return 1;
}

/* send data */
int CCOMPort::sendBytes(const unsigned char* pBuffer, int nBytes)
{
  assert( pBuffer );
  assert( nBytes > 0 );
  assert( m_fd );

  /* write the data */
  if( write(m_fd, pBuffer, nBytes) != nBytes ) {
    perror("\nCCOMPort::sendBytes::write(...) failed.");
    return 0;
  }

  return 1;
}

#define SSIZE_MAX 20

/* read back some data */
int CCOMPort::readBytes(unsigned char* pBuffer, int nBytes)
{
  assert( pBuffer );
  assert( m_fd );
  int n;

//   /* do it efficiently if necessary */
//   if( nBytes < SSIZE_MAX ) {

//     /* get back nBytes */
//     if( read(m_fd, pBuffer, nBytes) != nBytes ) {
//       perror("\nCCOMPort::readBytes::read failed to return correct byte count.");
//       return 0;
//     }
//   }

  /* read them back one at time */
  //  else {
  {
    
    for( n = 0; n < nBytes; n++ ) {

      /* read the byte */
      if( read(m_fd, &pBuffer[n], 1) == -1 ) {
				perror("\nCCOMPort::readBytes::read failed in biger than SSIZE_MAX.");
				return 0;
      }
    }
  }

  return 1;
}

int CCOMPort::flush()
{
  assert( m_fd );

  tcflush(m_fd, TCIOFLUSH);

  return 1;
}

