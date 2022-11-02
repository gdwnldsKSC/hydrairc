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

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//


//////////////////////////////////////////////////////////////////////////
//
// Configuration Options!
//

//Uncomment this for font script testing stuff, to be completed by Zann
//#define ZANN_FONTS

//Enables extra debug output
//#define VERBOSE_DEBUG

//Enables XP-Like menus
//Note: the context menus don't work right with this enabled due to the
//way that windows passes owner draw messages to the wrong windows.
//the implementation of the xp-like menus needs to be adjusted.
//--> this has now been fixed, but there's a maximized MDI window problem
//    to iron out first.
//--> additionally, checked items that have icons are not shown/drawn as
//    checked.
//#define USE_XPCOMMANDBAR

// Enables Tabbed MDI
#define USE_TABBEDMDI

//////////////////////////////////////////////////////////////////////////

#define RELEASE_VERSION

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <malloc.h>
#include <crtdbg.h>

#pragma once

// Utilize Windows SDK 6.0 and up macro to handle aligned target versions
// Winver/WinIE/etc - Windows SDK 6.0 Minimum, VS2008 included 
#define NTDDI_VERSION	NTDDI_WINXPSP1
#define _WIN32_WINNT	_WIN32_WINNT_WINXP
// Updating to latest version of Rich Edit control included in windows
// as standard for additional unicode & language support. Req XP SP1 and up
// allows usage of latest richedit control - for example, on W11 it loads
// version 8.5, whereas on XP SP1 it loads 4.1.
#define _RICHEDIT_VER	0x0500


#define _WTL_NO_CSTRING
//#define _WTL_NO_WTYPES

// ATL/GUI includes start here
#include <atlbase.h>
#include <atlstr.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>

#include <atlctrlx.h>
#include <atlsplit.h>

#include <atlmisc.h>

//#include <atlcom.h>

//#include <atlcoll.h>
//#include <atltypes.h>

// atl should be downloaded and install from here http://www.codeplex.com/AtlServer/
#include <atlrx.h>  // RegExps

// Staging support for IPv6
#include <ws2tcpip.h>

/////////////////////////////////////////////////////////////////////////
//
// 3rd Party Include Files
//

#include <FixedMDICommandBar.h>

#ifdef USE_XPCOMMANDBAR
#define _WTL_USE_MDI
#include <commandbarxp/atlctrlxp.h>
#include <commandbarxp/atlctrlxp2.h>
#endif

// Message pump for threads.
#include "asyncmessagepump.h"

// Tabs
#include "atlgdix.h"

#include "CustomTabCtrl.h"
#include "DotNetTabCtrl.h"
#include "TabbedFrame.h"

// Docking Windows
#include <DWAutoHide.h>
#include <sstream>
#include <sstate.h>
#include <dwstate.h>
#include <dbstate.h>
#include <DockingFrame.h>
#include <ExtDockingWindow.h>
#include <TabDockingBox.h>
#include <DockingFocus.h>
#include <VC7LikeCaption.h>

// Tabbed Docking Window (needs 3rd Party Tabs)
#include <TabbedDockingWindow.h>

#ifdef USE_TABBEDMDI
#define _TABBEDMDI_MESSAGES_EXTERN_REGISTER
#define _TABBEDMDI_MESSAGES_NO_WARN_ATL_MIN_CRT
#include "TabbedMDI.h"
#endif

// Dialog COntainer
#include <DlgContainer.h>

// Taskbar Icon Class (well, systray icon really...)
#include "taskbaricon.h"

// libxml
#include <libxml/parser.h>

// General Includes
#include <time.h>

// These are for using _open(), _write(), etc..
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

// HydraIRC Stuff
#include <CList.h>
#include <CIDManager.h>
#include <CObjectManager.h>
#include <CBufferedFile.h>

// HydraControls
#include <HydraPaneContainer.h>
#include <HydraSplitter.h>

// Other WTL Misc controls
#include "gradientlabel\GradientLabel.h"

// Other GUI related items
#include <DropFileHandler.h>
