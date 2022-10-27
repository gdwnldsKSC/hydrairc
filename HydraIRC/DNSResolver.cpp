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

  struct hostent *hp;
  // Allocate full size of HP to prevent access violations and other nastyness
  hp = (struct hostent *)calloc(1,sizeof(struct hostent));

  struct addrinfo hints, *res;
  // Init hints with zeros before usage
  memset(&hints, 0, sizeof(struct addrinfo));
  // vector that will contain all returned resolution addresses
  std::vector<in_addr*> in_addrs;
  std::vector<in_addr6*> in_addrs6;
  
  hints.ai_family = AF_UNSPEC;
  hints.ai_flags = AI_CANONNAME | AI_ADDRCONFIG; // AI_ADDRCONFIG means we return IPv4 addresses only on IPv4 only systems
  hints.ai_socktype = SOCK_STREAM;

  getaddrinfo(pDNSRI->m_fqdn, NULL, &hints, &res);

  // implement our old IPv4 only support 
  for(addrinfo *p = res; p != NULL; p = p->ai_next)
  {
	  char ipstring[MAX_HOSTNAME];
	  if(p->ai_family == AF_INET) { // IPv4
		  hp->h_name = p->ai_canonname;
		  hp->h_aliases = NULL;
		  hp->h_addrtype = p->ai_family;
		  hp->h_length = p->ai_addrlen;

		  in_addrs.push_back(&reinterpret_cast<sockaddr_in*>(p->ai_addr)->sin_addr);
		  in_addrs.push_back(NULL);
		  inet_ntop(AF_INET, (void*) &((struct sockaddr_in*)p->ai_addr)->sin_addr, ipstring, sizeof(ipstring));
		  sys_Printf(BIC_ERROR, "IPv4: %s\n", ipstring);

		  hp->h_addr_list = reinterpret_cast<char**>(&in_addrs[0]);
		  break;
	  } else { // IPv6
		  hp->h_name = p->ai_canonname;
		  hp->h_aliases = NULL;
		  hp->h_addrtype = p->ai_family;
		  hp->h_length = p->ai_addrlen;

		  in_addrs6.push_back(&reinterpret_cast<sockaddr_in6*>(p->ai_addr)->sin6_addr);
		  in_addrs6.push_back(NULL);

		  inet_ntop(AF_INET6, (void*) &((struct sockaddr_in6*) res->ai_addr)->sin6_addr, ipstring, sizeof(ipstring));
		  sys_Printf(BIC_ERROR, "IPv6: %s\n", ipstring);
		  
		  hp->h_addr_list = reinterpret_cast<char**>(&in_addrs6[0]);
		  break; // - commented out, once IPv6 works fully will re-add
	  }
  }


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
