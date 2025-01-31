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


// aboutdlg.cpp : implementation of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HydraIRC.h"

LRESULT CAboutDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  CStatic m_AboutInfo;
	CenterWindow(GetParent());
  m_AboutInfo.Attach(GetDlgItem(IDC_ABOUT_VERSIONINFO));
  m_AboutInfo.SetWindowText("HydraIRC " VERSIONSTRING "\n"
                            "\n"
                            "Written by Dominic Clifton\n"
                            "(c) Copyright 2002-2006\n"
                            "Most icons created by srobot (many thanks!)\n"
                            "Nice images created by digismack\n"
                            "\n"
                            "Special Thanks to Zann, Imajes, 'duckman, hdaalz, fryguy, \n"
							"digismack and the regular #HydraIRC idlers for continued support\n"
                            "gdwnldsKSC on github providing updates and fixes to newer\n"
							"OS support and Visual Studio versions in 2022!\n"
							"\n"
                            "HydraIRC is GPLv3! See orignial author's repositiory! \n"
							"https://github.com/HydraIRC/hydrairc \n");
  m_AboutInfo.Detach();

	return TRUE;
}

LRESULT CAboutDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}
