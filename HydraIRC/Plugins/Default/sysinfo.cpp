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
	     if ( osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0 && osvi.wProductType == VER_NT_WORKSTATION)
		 {
			if(osvi.dwBuildNumber > 22000)
				sprintf (buf+strlen(buf), "Microsoft Windows 11, ");
			else
				sprintf (buf+strlen(buf), "Microsoft Windows 10, ");
		 }

		 if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3 && osvi.wProductType == VER_NT_WORKSTATION)
			sprintf (buf+strlen(buf), "Microsoft Windows 8.1, ");

		 if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3 && osvi.wProductType == VER_NT_SERVER)
			sprintf (buf+strlen(buf), "Microsoft Windows Server 2012 R2, ");

		 if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2 && osvi.wProductType == VER_NT_WORKSTATION)
			sprintf (buf+strlen(buf), "Microsoft Windows 8, ");

		 if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2 && osvi.wProductType == VER_NT_SERVER)
			sprintf (buf+strlen(buf), "Microsoft Windows Server 2012, ");

		 if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 && osvi.wProductType == VER_NT_WORKSTATION)
			sprintf (buf+strlen(buf), "Microsoft Windows 7, ");

		 if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 && osvi.wProductType == VER_NT_SERVER)
			sprintf (buf+strlen(buf), "Microsoft Windows Server 2008 R2, ");

		 if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 && osvi.wProductType == VER_NT_WORKSTATION)
			sprintf (buf+strlen(buf), "Microsoft Windows Vista, ");

		 if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 && osvi.wProductType == VER_NT_SERVER)
			sprintf (buf+strlen(buf), "Microsoft Windows Server 2008, ");

         if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
            sprintf (buf+strlen(buf), "Microsoft Windows Server 2003 family, ");

         if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
            sprintf (buf+strlen(buf), "Microsoft Windows XP ");

         // Test for specific product on Windows NT 4.0 SP6 and later.
         if( bOsVersionInfoEx )
         {
            // Test for the workstation type.
            if ( osvi.wProductType == VER_NT_WORKSTATION )
            {
			   // NT4...... remove in future
               if( osvi.dwMajorVersion == 4 )
                  sprintf (buf+strlen(buf), "Workstation 4.0 " );
               else if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
                  sprintf (buf+strlen(buf), "Home Edition " );
			   else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
				  sprintf (buf+strlen(buf), "Enterprise Edition " );
			   // Windows XP

			   // Windows 10
			   else if (osvi.dwMajorVersion == 10 && osvi.dwBuildNumber < 22000 )
			   {
				  sprintf (buf+strlen(buf), "10 DETECTION");

				  if ( osvi.dwBuildNumber >= 19536 && osvi.dwBuildNumber < 21327 )				  
					sprintf (buf+strlen(buf), " (Active Development Branch) ");
			   }

			   // Windows 11
			   else if( osvi.dwMajorVersion == 10 && osvi.dwBuildNumber >= 22000 )
			   {
				  if ( (osvi.wSuiteMask & PRODUCT_PROFESSIONAL) != 0 )
					sprintf (buf+strlen(buf), "Professional Edition ");
				  else if ( (osvi.wSuiteMask & PRODUCT_ENTERPRISE) != 0 )
				    sprintf (buf+strlen(buf), "Enterprise Edition" );
				  else
				    sprintf (buf+strlen(buf), "11 EDITION NOT DETECTED " );

				  if ( osvi.dwBuildNumber > 22621 )
					sprintf (buf+strlen(buf), " (Active Development Branch) ");
			   }

			   else
                  sprintf (buf+strlen(buf), "Edition not detected " );
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

               else  // Catchall 
                  sprintf (buf+strlen(buf), "Server 4.0, Enterprise Edition " );
               
            }
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