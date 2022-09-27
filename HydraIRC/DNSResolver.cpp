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

  struct hostent *hp;
  hp = gethostbyname(pDNSRI->m_fqdn);

// DEBUG: IPV6 PREPARATION: 
// PREPARING TO CHANGE IP RESOLUTION TO GETADDRINFO FROM GETHOSTBYNAME
  sys_Printf(BIC_INFO, "GetAddrInfo Test code following");
  struct addrinfo hints, *res;
  struct in_addr addr;
  int err;
  char address[46];
  DWORD size = 46;
  LPSOCKADDR sockaddr_ip;

  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_UNSPEC;

  if ((err = getaddrinfo(pDNSRI->m_fqdn, NULL, &hints, &res)) != 0) {
	  sys_Printf(BIC_INFO, "error %d\n", err);
  }

  addr.S_un = ((struct sockaddr_in *)(res->ai_addr))->sin_addr.S_un;

// InetPton is supported on Vista and later only so use WSAAddressToString

  sockaddr_ip = (LPSOCKADDR) res->ai_addr;

  WSAAddressToString(sockaddr_ip, (DWORD) res->ai_addrlen, NULL, address, &size);

  sys_Printf(BIC_INFO, "ip address : %s\n", address);


  // Signal the calling window
  if (hp != NULL)
  {
    // Success!
    memcpy(&pDNSRI->m_address, hp->h_addr_list[0], sizeof(DWORD));
  }

  // ok, some time might have passed, and the user may have closed the window
  // so check to see if the window still exists before attempting to connect.

  if (IsWindow(pDNSRI->m_hWnd))
  {
    ::PostMessage(pDNSRI->m_hWnd,WM_DNSEVENT,(hp != NULL),(LPARAM) pDNSRI);
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
