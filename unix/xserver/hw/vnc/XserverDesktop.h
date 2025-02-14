/* Copyright (C) 2002-2005 RealVNC Ltd.  All Rights Reserved.
 * Copyright 2009-2015 Pierre Ossman for Cendio AB
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 */
//
// XserverDesktop.h
//

#ifndef __XSERVERDESKTOP_H__
#define __XSERVERDESKTOP_H__

#ifdef HAVE_DIX_CONFIG_H
#include <dix-config.h>
#endif

#include <map>

#include <stdint.h>

#include <rfb/SDesktop.h>
#include <rfb/PixelBuffer.h>
#include <rfb/Configuration.h>
#include <rfb/VNCServerST.h>
#include <unixcommon.h>
#include "Input.h"

namespace rfb {
  class VNCServerST;
}

namespace network { class SocketListener; class Socket; class SocketServer; }

class XserverDesktop : public rfb::SDesktop, public rfb::FullFramePixelBuffer,
                       public rfb::VNCServerST::QueryConnectionHandler,
                       public rfb::Timer::Callback {
public:

  XserverDesktop(int screenIndex,
                 std::list<network::SocketListener*> listeners_,
                 const char* name, const rfb::PixelFormat &pf,
                 int width, int height, void* fbptr, int stride);
  virtual ~XserverDesktop();

  // methods called from X server code
  void blockUpdates();
  void unblockUpdates();
  void setFramebuffer(int w, int h, void* fbptr, int stride);
  void refreshScreenLayout();
  void requestClipboard();
  void clearLocalClipboards();
  void announceClipboard(bool available);
  void clearBinaryClipboardData();
  void sendBinaryClipboardData(const char* mime, const unsigned char *data,
                               const unsigned len);
  void getBinaryClipboardData(const char *mime, const unsigned char **ptr,
                              unsigned *len);
  void bell();
  void setLEDState(unsigned int state);
  void setDesktopName(const char* name);
  void setCursor(int width, int height, int hotX, int hotY,
                 const unsigned char *rgbaData);
  void setCursorPos(int x, int y, bool warped);
  void add_changed(const rfb::Region &region);
  void add_copied(const rfb::Region &dest, const rfb::Point &delta);
  void handleSocketEvent(int fd, bool read, bool write);
  void blockHandler(int* timeout);
  void addClient(network::Socket* sock, bool reverse);
  void disconnectClients();

  // QueryConnect methods called from X server code
  // getQueryConnect()
  //   Returns information about the currently waiting query
  //   (or an id of 0 if there is none waiting)
  void getQueryConnect(uint32_t* opaqueId, const char** address,
                       const char** username, int *timeout);

  // approveConnection()
  //   Used by X server code to supply the result of a query.
  void approveConnection(uint32_t opaqueId, bool accept,
                         const char* rejectMsg=0);

  // rfb::SDesktop callbacks
  virtual void pointerEvent(const rfb::Point& pos, const rfb::Point& abspos, int buttonMask,
                            const bool skipClick, const bool skipRelease, int scrollX = 0, int scrollY = 0);
  virtual void keyEvent(rdr::U32 keysym, rdr::U32 keycode, bool down);
  virtual unsigned int setScreenLayout(int fb_width, int fb_height,
                                       const rfb::ScreenSet& layout);
  virtual void handleClipboardAnnounce(bool available);
  virtual void handleClipboardAnnounceBinary(const unsigned num, const char mimes[][32]);

  // rfb::PixelBuffer callbacks
  virtual void grabRegion(const rfb::Region& r);

  // rfb::VNCServerST::QueryConnectionHandler callback
  virtual rfb::VNCServerST::queryResult queryConnection(network::Socket* sock,
                                                        const char* userName,
                                                        char** reason);

protected:
  bool handleListenerEvent(int fd,
                           std::list<network::SocketListener*>* sockets,
                           network::SocketServer* sockserv);
  bool handleSocketEvent(int fd,
                         network::SocketServer* sockserv,
                         bool read, bool write);

  virtual bool handleTimeout(rfb::Timer* t);

private:

  int screenIndex;
  rfb::VNCServerST* server;
  std::list<network::SocketListener*> listeners;
  bool directFbptr;

  uint32_t queryConnectId;
  network::Socket* queryConnectSocket;
  rfb::CharArray queryConnectAddress;
  rfb::CharArray queryConnectUsername;
  rfb::Timer queryConnectTimer;

  OutputIdMap outputIdMap;

  rfb::Point oldCursorPos;

  bool resizing;
};
#endif
