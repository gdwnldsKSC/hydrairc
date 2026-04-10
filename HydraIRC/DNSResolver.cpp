/*

  HydraIRC
  Copyright (C) 2002-2006 Dominic Clifton aka Hydra

  This program is free software: you can redistribute it and/or modify  
  it under the terms of the GNU General Public License as published by  
  the Free Software Foundation, version 3.
 
  This program is distributed in the hope that it will be useful, but 
  WITHOUT ANY WARRANTY; without even the implied warranty of 
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
  General Public License for more details.
 
  You should have received a copy of the GNU General Public License 
  along with this program. If not, see <http://www.gnu.org/licenses/>.

*/

// CDNSResolver.cpp : implements a threaded DNS Resolver
//
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "HydraIRC.h"

#define MAX_HOSTNAME 128

CDNSResolver::CDNSResolver(void)
{
  m_Thread1.Start(this, THID_DNSRESOLVER);
}

CDNSResolver::~CDNSResolver(void)
{
  m_Thread1.Stop();
}

LRESULT CDNSResolver::OnDNSEvent(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
{
  bHandled = TRUE;

  DNSResolveItem *pDNSRI = (DNSResolveItem *)lParam;
  ATLASSERT(pDNSRI);

  // resolve the address! :)
  BOOL resolved = FALSE;
  struct addrinfo hints, *res = NULL;

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_CANONNAME | AI_ADDRCONFIG; // AI_ADDRCONFIG means we return IPv4 addresses only on IPv4 only systems
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(pDNSRI->m_fqdn, NULL, &hints, &res) == 0)
  {
    for (addrinfo *p = res; p != NULL; p = p->ai_next)
    {
      if ((p->ai_family != AF_INET && p->ai_family != AF_INET6) || p->ai_addr == NULL)
        continue;

      if (p->ai_addrlen > sizeof(pDNSRI->m_address))
        continue;

      memcpy(&pDNSRI->m_address, p->ai_addr, p->ai_addrlen);
      pDNSRI->m_addressLength = (int)p->ai_addrlen;
      resolved = TRUE;
      break;
    }

    freeaddrinfo(res);
  }

  // ok, some time might have passed, and the user may have closed the window
  // so check to see if the window still exists before attempting to connect.

  if (IsWindow(pDNSRI->m_hWnd))
  {
    ::PostMessage(pDNSRI->m_hWnd,WM_DNSEVENT,resolved,(LPARAM) pDNSRI);
  }
  else
  {
    // window was deleted, so we can free this...
    delete pDNSRI;
  }

  return FALSE;
}

void CDNSResolver::StartResolve(char *fqdn, HWND hWnd)
{
  // Note: pDNSRI must be freed by the calling thread when it gets this
  // back from WM_DNSEVENT message's LPARAM

  DNSResolveItem *pDNSRI = new DNSResolveItem(fqdn, hWnd);

  // Post a message to the DNSResolver thread's message queue.
  // this will then (eventually) trigger a call to OnDNSEvent which does
  // the work.
  BOOL Success = m_Thread1.PostMessage(WM_DNSEVENT,NULL,(LPARAM) pDNSRI);
  ATLASSERT(Success);
}
