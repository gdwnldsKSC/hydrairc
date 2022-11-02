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

// ChannelPropertiesDlg.cpp : implementation of the CChannelPropertiesDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HydraIRC.h"
#include "ChannelPropertiesDlg.h"

LRESULT CChannelPropertiesDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{  
	CenterWindow(GetParent());

  DlgResize_Init();

  // Init the controls

  m_TopicCtrl.Attach(GetDlgItem(IDC_CHANNELPROPERTIES_TOPIC));
  //m_TopicCtrl.SubclassWindow(GetDlgItem(IDC_CHANNELPROPERTIES_TOPIC));
  m_TopicCtrl.Clear();
  m_TopicCtrl.SetLimitText(510); // TODO: Set to same value that the server gives us when we connect e.g. "TOPICLEN=120"
  if (m_pProperties->m_Topic)
    m_TopicCtrl.AppendText(m_pProperties->m_Topic);

  m_KeyStringCtrl.Attach(GetDlgItem(IDC_CHANNELPROPERTIES_KEYSTRING));
  m_KeyStringCtrl.Clear();
  m_KeyStringCtrl.SetLimitText(510); // Is there a maximum length for this ? using IRC protocol length for now
  if (m_pProperties->m_Key)
    m_KeyStringCtrl.AppendText(m_pProperties->m_Key);

  m_KeyCtrl.Attach(GetDlgItem(IDC_CHANNELPROPERTIES_KEY));
  m_KeyCtrl.SetCheck((m_pProperties->m_Modes & CHANNELMODE_KEY) ? BST_CHECKED : BST_UNCHECKED);

  m_LimitStringCtrl.Attach(GetDlgItem(IDC_CHANNELPROPERTIES_LIMITSTRING));
  m_LimitStringCtrl.Clear();
  m_LimitStringCtrl.SetLimitText(10); // pretty big number.. :)
  if (m_pProperties->m_Modes & CHANNELMODE_LIMIT)
  {
    char numstr[10];
    _snprintf(numstr,sizeof(numstr),"%d",m_pProperties->m_Limit);
    m_LimitStringCtrl.AppendText(numstr);
  }

  m_LimitCtrl.Attach(GetDlgItem(IDC_CHANNELPROPERTIES_LIMIT));
  m_LimitCtrl.SetCheck((m_pProperties->m_Modes & CHANNELMODE_LIMIT) ? BST_CHECKED : BST_UNCHECKED);

  m_InviteCtrl.Attach(GetDlgItem(IDC_CHANNELPROPERTIES_INVITE));
  m_InviteCtrl.SetCheck((m_pProperties->m_Modes & CHANNELMODE_INVITE) ? BST_CHECKED : BST_UNCHECKED);

  // Disable controls depending on what access the user has in the channel
/*
  IRCUser *pUser;
  pUser = m_pChannel->FindUser(m_pChannel->m_pServer->GetNick());
  if (pUser)
  {
    if (pUser->m_UserType < USER_TYPE_HALFOP) // TODO: check HALFOP or OP ???
    {
      m_TopicCtrl.EnableWindow(FALSE);

      m_KeyStringCtrl.EnableWindow(FALSE);
      m_KeyCtrl.EnableWindow(FALSE);

      m_LimitStringCtrl.EnableWindow(FALSE);
      m_LimitCtrl.EnableWindow(FALSE);

      m_InviteCtrl.EnableWindow(FALSE);
    }
  }
*/
	return TRUE;
}

LRESULT CChannelPropertiesDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  EndDialog(0);    
  return 0;
}

void CChannelPropertiesDlg::OnOK( void )
{
  char *tmpstr;
  int len;

  len = m_TopicCtrl.GetWindowTextLength();
  if (len)
  {
    // get the new topic
    tmpstr = (char *)malloc(len + 1); //add space for null terminator
    m_TopicCtrl.GetWindowText(tmpstr,len+1);
    tmpstr[len] = 0; // null terminate to make sure!
    m_pProperties->SetTopic(tmpstr);
    free(tmpstr);
  }
  /*
  else
  {
    m_pProperties->SetTopic(NULL); // user removed the topic!
  }
  */

  len = m_KeyStringCtrl.GetWindowTextLength();
  if (len)
  {
    // get the new channel key
    tmpstr = (char *)malloc(len + 1); //add space for null terminator
    m_KeyStringCtrl.GetWindowText(tmpstr,len+1);
    tmpstr[len] = 0; // null terminate to make sure!
    m_pProperties->SetKey(tmpstr);
    free(tmpstr);
  }
  /*
  else
  {
    m_pProperties->SetKey(NULL); // user removed the topic!
  }
  */

  len = m_LimitStringCtrl.GetWindowTextLength();
  if (len)
  {
    // get the new channel key
    tmpstr = (char *)malloc(len + 1); //add space for null terminator
    m_LimitStringCtrl.GetWindowText(tmpstr,len+1);
    tmpstr[len] = 0; // null terminate to make sure!
    m_pProperties->m_Limit = atoi(tmpstr);
    free(tmpstr);
  }

  if (m_KeyCtrl.GetCheck() == BST_CHECKED)
    m_pProperties->m_Modes |= CHANNELMODE_KEY;
  else
    m_pProperties->m_Modes &= ~CHANNELMODE_KEY;

  if (m_LimitCtrl.GetCheck() == BST_CHECKED)
    m_pProperties->m_Modes |= CHANNELMODE_LIMIT;
  else
    m_pProperties->m_Modes &= ~CHANNELMODE_LIMIT;

  if (m_InviteCtrl.GetCheck() == BST_CHECKED)
    m_pProperties->m_Modes |= CHANNELMODE_INVITE;
  else
    m_pProperties->m_Modes &= ~CHANNELMODE_INVITE;

  // All OK!
	EndDialog(1);    
}

LRESULT CChannelPropertiesDlg::OnOKCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
  OnOK();
	return 0;
}

CChannelPropertiesDlg::CChannelPropertiesDlg( IRCChannel *pChannel, CChannelProperties *pProperties )
{
  m_pProperties = pProperties;
  m_pChannel = pChannel;
}

CChannelPropertiesDlg::~CChannelPropertiesDlg( void )
{
}
