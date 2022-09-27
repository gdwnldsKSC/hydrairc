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
  // Allocate full size of HP to prevent access violations and other nastyness
  hp = (struct hostent *)calloc(1,sizeof(struct hostent));

  struct addrinfo hints, *res;
  // Init hints with zeros before usage
  memset(&hints, 0, sizeof(hints));
  // vector that will contain all returned resolution addresses
  std::vector<in_addr*> in_addrs;
  // simple error handling for getaddrinfo
  int err = 0;
 
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_CANONNAME;
  // Change to AF_UNSPEC to perform dual stack lookups with IPv6 preferred, AF_INET for IPv4 only
  hints.ai_family = AF_INET;

  if ((err = getaddrinfo(pDNSRI->m_fqdn, NULL, &hints, &res)) != 0) {
	  sys_Printf(BIC_INFO, "error %d\n", err);
  }
 
  // prepare to build hostent struct from getaddrinfo data - take first address and put it into in_addrs
  // using vector we can potentially rotate addresses in a future release from a single DNS resolution
  for(addrinfo *p_addr = res; p_addr != NULL; p_addr = p_addr->ai_next) {
      in_addrs.push_back(&reinterpret_cast<sockaddr_in*>(p_addr->ai_addr)->sin_addr);
  }
  in_addrs.push_back(NULL);

  // Here we take the output of getaddrinfo and convert it to a hostent for existing code compatibility
  hp->h_name = res->ai_canonname;
  hp->h_aliases = NULL;
  hp->h_addrtype = AF_INET;
  hp->h_length = sizeof(in_addr);
  hp->h_addr_list = reinterpret_cast<char**>(&in_addrs[0]);

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
