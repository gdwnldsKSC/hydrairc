// sysinfo-console.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "sysinfo.h"
#include <windows.h>

char *osversion()
{
  char buf[256];
  strcpy(buf, "");

   OSVERSIONINFOEX osvi;
   BOOL bOsVersionInfoEx;

   // Try calling GetVersionEx using the OSVERSIONINFOEX structure.
   // If that fails, try using the OSVERSIONINFO structure.

   ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
   osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

   if( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
   {
      osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
      if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) 
         return _strdup(buf);
   }

   if (osvi.dwPlatformId == 2)
   {
         // Test for the specific product family.
		 if ( osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0)
		 {
			if(osvi.dwBuildNumber > 22000)
				sprintf (buf+strlen(buf), "Microsoft Windows 11, ");
			else
				sprintf (buf+strlen(buf), "Microsoft Windows 10, ");
		 }

         if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
            sprintf (buf+strlen(buf), "Microsoft Windows Server 2003 family, ");

         if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
            sprintf (buf+strlen(buf), "Microsoft Windows XP ");

         if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
            sprintf (buf+strlen(buf), "Microsoft Windows 2000 ");

         if ( osvi.dwMajorVersion <= 4 )
            sprintf (buf+strlen(buf), "Microsoft Windows NT ");

         // Test for specific product on Windows NT 4.0 SP6 and later.
         if( bOsVersionInfoEx )
         {
            // Test for the workstation type.
            if ( osvi.wProductType == VER_NT_WORKSTATION )
            {
               if( osvi.dwMajorVersion == 4 )
                  sprintf (buf+strlen(buf), "Workstation 4.0 " );
               else if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
                  sprintf (buf+strlen(buf), "Home Edition " );
			   else
                  sprintf (buf+strlen(buf), "Undetermined (Possibly Pro or Enterprise) " );
            }
            
            // Test for the server type.
            else if ( osvi.wProductType == VER_NT_SERVER )
            {
               if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
               {
                  if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                     sprintf (buf+strlen(buf), "Datacenter Edition " );
                  else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                     sprintf (buf+strlen(buf), "Enterprise Edition " );
                  else if ( osvi.wSuiteMask == VER_SUITE_BLADE )
                     sprintf (buf+strlen(buf), "Web Edition " );
                  else
                     sprintf (buf+strlen(buf), "Standard Edition " );
               }

               else if( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
               {
                  if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
                     sprintf (buf+strlen(buf), "Datacenter Server " );
                  else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                     sprintf (buf+strlen(buf), "Advanced Server " );
                  else
                     sprintf (buf+strlen(buf), "Server " );
               }

               else  // Windows NT 4.0 
               {
                  if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
                     sprintf (buf+strlen(buf), "Server 4.0, Enterprise Edition " );
                  else
                     sprintf (buf+strlen(buf), "Server 4.0 " );
               }
            }
         }
         else  // Test for specific product on Windows NT 4.0 SP5 and earlier
         {
            HKEY hKey;
            char szProductType[BUFSIZE];
            DWORD dwBufLen=BUFSIZE;
            LONG lRet;

            lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
               "SYSTEM\\CurrentControlSet\\Control\\ProductOptions",
               0, KEY_QUERY_VALUE, &hKey );
            if( lRet != ERROR_SUCCESS )
               return _strdup(buf);

            lRet = RegQueryValueEx( hKey, "ProductType", NULL, NULL,
               (LPBYTE) szProductType, &dwBufLen);
            if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) )
               return _strdup(buf);

            RegCloseKey( hKey );

            if ( lstrcmpi( "WINNT", szProductType) == 0 )
               sprintf (buf+strlen(buf), "Workstation " );
            if ( lstrcmpi( "LANMANNT", szProductType) == 0 )
               sprintf (buf+strlen(buf), "Server " );
            if ( lstrcmpi( "SERVERNT", szProductType) == 0 )
               sprintf (buf+strlen(buf), "Advanced Server " );

            printf( "%d.%d ", osvi.dwMajorVersion, osvi.dwMinorVersion );
         }

         // Display service pack (if any) and build number.
         sprintf (buf+strlen(buf), "%s (Build %d)", osvi.szCSDVersion, osvi.dwBuildNumber & 0xFFFF);
   }
   return _strdup(buf);
}

char *cpuinfo()
{
  char buf[256];

  HKEY hKey;
  LONG lRet;
  DWORD dwBufLen = 256;

  lRet = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
    "Hardware\\Description\\System\\CentralProcessor\\0",
    0, KEY_QUERY_VALUE, &hKey );
  if( lRet == ERROR_SUCCESS ) {
    lRet = RegQueryValueEx( hKey, "ProcessorNameString", NULL, NULL,
      (LPBYTE)buf, &dwBufLen);
  } else { // Windows NT 4.0 prior to SP6a
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    sprintf(buf, "%d%d", sysinfo.dwProcessorType, sysinfo.dwNumberOfProcessors);
  }

  RegCloseKey( hKey );

  return _strdup(buf);
}

char *meminfo()
{
  char buf[256];
  MEMORYSTATUSEX memstatus;
  memstatus.dwLength = sizeof(memstatus);
  GlobalMemoryStatusEx(&memstatus);
  sprintf(buf, "Memory: %llu/%llu MB", memstatus.ullAvailPhys/(1024*1024), memstatus.ullTotalPhys/(1024*1024));
  return _strdup(buf);
}