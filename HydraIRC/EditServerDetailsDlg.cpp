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

/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "HydraIRC.h"
#include ".\editserverdetailsdlg.h" //TODO: move to hydrairc.h

char *g_PriorityNumbersNames[] = // see enum PriorityNumbers
{
  "Disabled",
  "Lowest",
  "Low",
  "Normal",
  "High",
  "Highest"
};

BOOL CEditServerDetailsDlg::Validate( void )
{
  BOOL Result = TRUE;
  char *NumStr = NULL;
  char *Password1 = NULL;
  char *Password2 = NULL;
  int Index;
  int Port; // note int, not unsigned short!

  if (!m_Initialised)
    Result = FALSE;
  else
  {

    CopyWindowText(&m_DescriptionCtrl,m_pServerDetails->m_Description);
    CopyWindowText(&m_AddressCtrl,m_pServerDetails->m_Name);
    CopyWindowText(&m_PasswordCtrl,Password1);
    CopyWindowText(&m_Password2Ctrl,Password2);
    
    CopyWindowText(&m_PortCtrl,NumStr);
    Port = NumStr ? atoi(NumStr) : -1;

    Index = m_PriorityCtrl.GetCurSel();
    if (Index != CB_ERR)
      m_pServerDetails->m_Priority = Index; // actually, Index - PRI_DISABLED, but index is 0 based too..

    Index = m_NetworkCtrl.GetCurSel(); // do this one last, see below!
    if (Index != CB_ERR)
      m_pServerDetails->m_NetworkID = (int)m_NetworkCtrl.GetItemDataPtr(Index);

    Result = Index != CB_ERR && // check Index and NetworkID are valid!
            m_pServerDetails->m_NetworkID != -1 && // -1 == error from GetItemDataPtr()
            m_pServerDetails->m_Description &&
            m_pServerDetails->m_Name &&
            m_pServerDetails->m_Priority >= PRI_DISABLED && m_pServerDetails->m_Priority <= PRI_HIGHEST &&
            Port > 0 && Port <= 65535;

    if (Result)
      m_pServerDetails->m_Port = Port;

    // check passwords match (if we have them)
    if (Password1 && Password2)
      if (stricmp(Password1,Password2) == 0)
        SetString(m_pServerDetails->m_Password,Password1);
      else
        Result = FALSE;
    else
    {
      // password in first password field, but not in second password field?
      if (Password1 && *Password1 && (!Password2 || (Password2 && !*Password2)))
        Result = FALSE;
      // password in second password field, but not in first password field?
      if (Password2 && *Password2 && (!Password1 || (Password1 && !*Password1)))
        Result = FALSE;
    }

  }
  m_OKCtrl.EnableWindow(Result);

  if (NumStr) free(NumStr);
  if (Password1) free(Password1);
  if (Password2) free(Password2);
  return Result;
}

LRESULT CEditServerDetailsDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
  int i,id;
  IRCNetworkDetails *pNetworkDetails;
  char NumStr[10];

  CenterWindow(GetParent());
  DlgResize_Init();

  m_DescriptionCtrl.Attach(GetDlgItem(IDC_ESRV_DESCRIPTION));
  m_AddressCtrl.Attach(GetDlgItem(IDC_ESRV_ADDRESS));
  m_PortCtrl.Attach(GetDlgItem(IDC_ESRV_PORT));
  m_PasswordCtrl.Attach(GetDlgItem(IDC_ESRV_PASSWORD));
  m_Password2Ctrl.Attach(GetDlgItem(IDC_ESRV_PASSWORD2));
  m_NetworkCtrl.Attach(GetDlgItem(IDC_ESRV_NETWORK));
  m_PriorityCtrl.Attach(GetDlgItem(IDC_ESRV_PRIORITY));
  m_OKCtrl.Attach(GetDlgItem(ID_ESRV_OK));

  m_DescriptionCtrl.SetWindowText(m_pServerDetails->m_Description);
  m_AddressCtrl.SetWindowText(m_pServerDetails->m_Name);
  _snprintf(NumStr,sizeof(NumStr),"%d",m_pServerDetails->m_Port);
  m_PortCtrl.SetWindowText(NumStr);
  m_PasswordCtrl.SetWindowText(m_pServerDetails->m_Password);
  m_Password2Ctrl.SetWindowText(m_pServerDetails->m_Password);


  for (i = PRI_DISABLED ; i <= PRI_HIGHEST; i++)
  {
    id = m_PriorityCtrl.AddString(g_PriorityNumbersNames[i]);
    if (id != CB_ERR && m_pServerDetails->m_Priority == i)
    {
      m_PriorityCtrl.SetCurSel(id); // set the selection
    }
  }

  for (i = 0 ; i < g_NetworkList.GetSize(); i++)
  {
    pNetworkDetails = g_NetworkList[i];
    id = m_NetworkCtrl.AddString(pNetworkDetails->m_Name);
    if (id != CB_ERR)
    {
      m_NetworkCtrl.SetItemData(id,(DWORD_PTR)pNetworkDetails->m_ID); // store the ID
      if (m_pServerDetails->m_NetworkID == pNetworkDetails->m_ID)
        m_NetworkCtrl.SetCurSel(id); // set the selection
    }
  }

  m_Initialised = TRUE;
  Validate();
	return TRUE;
}

LRESULT CEditServerDetailsDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}

LRESULT CEditServerDetailsDlg::OnOKCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  if (Validate())
  {
    EndDialog(1);    
  }
	return 0;
}

LRESULT CEditServerDetailsDlg::OnSomethingChanged(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  Validate();
  return 0;
}

