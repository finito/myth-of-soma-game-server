#include "stdafx.h"
#include "1p1emu.h"
#include "Extern.h"
#include "USER.h"
#include "COM.h"
#include "CircularBuffer.h"

#include "Search.h"
#include "1p1emudlg.h"

#include "BufferEx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CSearch *g_pUserList;
extern CMy1p1EmuDlg *g_pMainDlg;

void USER::PartyDissolve(USER *pLeader)
{
	if (!pLeader) return;
	if (!pLeader->m_bInParty) return;
	if (pLeader->m_Uid != pLeader->m_PartyMembers[0].uid ||
		strcmp(pLeader->m_strUserId, pLeader->m_PartyMembers[0].m_strUserId) != 0) return;

	// Send message to all players still within the party
	// informing them that the party has been disbanded.
	for (int i = 0; i < MAX_PARTY_USER_NUM; i++)
	{
		if (pLeader->m_PartyMembers[i].uid != -1)
		{
			USER *pMember = g_pUserList->GetUserUid(pLeader->m_PartyMembers[i].uid);
			if (!pMember) continue;
			if (pMember->m_State != STATE_GAMESTARTED) continue;
			// Below check is done incase the uid has been reused for another user
			// and for some reason user is still in party list.
			if (strcmp(pMember->m_strUserId, pLeader->m_PartyMembers[i].m_strUserId) != 0) continue;

			pMember->m_bInParty = false;
			pMember->m_PartyMembers[0].uid = -1;
			ZeroMemory(pMember->m_PartyMembers[0].m_strUserId, sizeof(pMember->m_PartyMembers[0].m_strUserId));

			pMember->SendServerChatMessage(IDS_USER_PARTY_DISBANDED, TO_ME);

			pMember->SendMyInfo(TO_ME, INFO_MODIFY);

			for (int j = 0; j < MAX_PARTY_USER_NUM; j++)
			{
				int index = 0;
				SetByte(m_TempBuf, PKT_PARTY_INVITE_RESULT, index);
				SetByte(m_TempBuf, DELETE_MEMBER, index);
				SetInt(m_TempBuf, pLeader->m_PartyMembers[i].uid + USER_BAND, index);
				// Source client does not need this but euro client does
				// and got no clue what its even for...
				SetByte(m_TempBuf, 1, index);
				pMember->Send(m_TempBuf, index);
			}
		}

		pLeader->m_PartyMembers[i].uid = -1;
		ZeroMemory(pLeader->m_PartyMembers[i].m_strUserId, sizeof(pLeader->m_PartyMembers[i].m_strUserId));
	}
}

void USER::PartyWithdraw()
{
	PartyWithdraw(m_strUserId);
}

void USER::PartyWithdraw(char* strUserId)
{
	m_bInParty = false;
	USER* pLeader = g_pUserList->GetUserUid(m_PartyMembers[0].uid);
	if (pLeader)
	{
		// Validate that it is the leader of this user's party
		if (strcmp(pLeader->m_strUserId, m_PartyMembers[0].m_strUserId) == 0 &&
			pLeader->m_Uid == m_PartyMembers[0].uid)
		{
			// Remove the member from the leader party list
			for (int i = 0; i < MAX_PARTY_USER_NUM; i++)
			{
				if (strcmp(pLeader->m_PartyMembers[i].m_strUserId, strUserId) == 0)
				{
					ZeroMemory(pLeader->m_PartyMembers[i].m_strUserId, sizeof(pLeader->m_PartyMembers[i].m_strUserId));
					pLeader->m_PartyMembers[i].uid = -1;
				}
			}

			// Generate the message to be sent to players
			CString strLeave;
			strLeave.Format(IDS_USER_PARTY_LEAVE, strUserId);

			// Send message to all players still within the party
			// informing them that someone has left.
			for (int i = 0; i < MAX_PARTY_USER_NUM; i++)
			{
				if (pLeader->m_PartyMembers[i].uid != -1)
				{
					USER *pMember = g_pUserList->GetUserUid(pLeader->m_PartyMembers[i].uid);
					if (!pMember) continue;
					if (pMember->m_State != STATE_GAMESTARTED) continue;
					// Below check is done incase the uid has been reused for another user
					// and for some reason user is still in party list.
					if (strcmp(pMember->m_strUserId, pLeader->m_PartyMembers[i].m_strUserId) != 0) continue;

					pMember->SendServerChatMessage((LPTSTR)(LPCTSTR)strLeave, TO_ME);

					int index = 0;
					SetByte(m_TempBuf, PKT_PARTY_INVITE_RESULT, index);
					SetByte(m_TempBuf, DELETE_MEMBER, index);
					SetInt(m_TempBuf, m_Uid + USER_BAND, index);
					// Source client does not need this but euro client does
					// and got no clue what its even for... SetByte(m_TempBuf, unknown, index);
					pMember->Send(m_TempBuf, index);
				}
			}
		}

		SendServerChatMessage(IDS_USER_PARTY_WITHDRAW, TO_ME);
	}

	ZeroMemory(m_PartyMembers[0].m_strUserId, sizeof(m_PartyMembers[0].m_strUserId));
	m_PartyMembers[0].uid = -1;

	SendMyInfo(TO_ME, INFO_MODIFY);

	int iMembers = GetPartyMemberCount(pLeader);
	if (iMembers <= 1)
	{
		PartyDissolve(pLeader);
	}
}

int USER::GetPartyMemberCount(USER* pLeader)
{
	if (!pLeader) return 0 ;
	if (!pLeader->m_bInParty) return 0;

	int iMembers = 0;
	for (int i = 0; i < MAX_PARTY_USER_NUM; i++)
	{
		if (pLeader->m_PartyMembers[i].uid != -1 &&
			pLeader->m_PartyMembers[i].m_strUserId[0] != 0)
		{
			++iMembers;
		}
	}
	return iMembers;
}

bool USER::CheckPartyLeader()
{
	if (!m_bInParty) return false;

	bool bLeader = true;
	if (m_Uid != m_PartyMembers[0].uid ||
		strcmp(m_strUserId, m_PartyMembers[0].m_strUserId) != 0)
	{
		bLeader = false;
	}

	return bLeader;
}

void USER::PartyCreate(char* strUserId)
{
	// Check if already in a party therefore its already created.
	if (m_bInParty) return;

	USER* pMember = GetUserId(strUserId);
	if (!pMember)
	{
		CString strMessage = _T("");
		strMessage.Format(IDS_USER_NOT_CONNECTED_NOW, strUserId);
		SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
		return;
	}

	if (pMember->m_State != STATE_GAMESTARTED)
	{
		CString strMessage = _T("");
		strMessage.Format(IDS_USER_NOT_CONNECTED_NOW, strUserId);
		SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
		return;
	}

	// Cannot party with another race
	if (CheckDemon(m_sClass) != CheckDemon(pMember->m_sClass))
	{
		SendServerChatMessage(IDS_USER_CANNOT_PARTY_OTHER_RACE, TO_ME);
		return;
	}

	// Cannot party if player is blocking party invitations
	if (pMember->m_bPartyDeny)
	{
		CString strMessage = _T("");
		strMessage.Format(IDS_USER_BLOCKING_PARTY_INVITE, strUserId);
		SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
		return;
	}

	// Cannot party if the player is already been sent a party invitation
	// and the invitation is still active
	if (pMember->m_bPartyInvite)
	{
		CString strMessage = _T("");
		strMessage.Format(IDS_USER_BLOCKING_PARTY_INVITE, strUserId);
		SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
		return;
	}

	// Cannot party if the player is already in a party with another player
	if (pMember->m_bInParty)
	{
		CString strMessage = _T("");
		strMessage.Format(IDS_USER_BLOCKING_PARTY_INVITE, strUserId);
		SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
		return;
	}

	m_bPartyInvite = true;
	m_iPartyInviteUid = pMember->m_Uid;

	pMember->m_bPartyInvite = true;
	pMember->m_iPartyInviteUid = m_Uid;

	// Inform client for both players of the invitation to create party
	int index = 0;
	SetByte(m_TempBuf, PKT_PARTY_INVITE, index);
	SetInt(m_TempBuf, m_Uid + USER_BAND, index);
	SetInt(m_TempBuf, pMember->m_Uid + USER_BAND, index);
	SetVarString(m_TempBuf, m_strUserId, strlen(m_strUserId), index);
	Send(m_TempBuf, index);
	pMember->Send(m_TempBuf, index);
}

void USER::PartyInvite(char* strUserId)
{
	// Cannot invite a player into a party if not in a party
	if (!m_bInParty) return;

	// Validate the leader of party
	if (strcmp(m_strUserId, m_PartyMembers[0].m_strUserId) != 0 ||
		m_Uid != m_PartyMembers[0].uid)
	{
		return;
	}

	// Check for empty slot in the party
	int iEmpty = 0;
	for (int i = 0; i < MAX_PARTY_USER_NUM; i++)
	{
		if (m_PartyMembers[i].uid == -1)
		{
			iEmpty = i;
			break;
		}
		++iEmpty;
	}

	// See if the party has reached its max amount of players allowed
	if (iEmpty >= MAX_PARTY_USER_NUM)
	{
		SendServerChatMessage(IDS_USER_PARTY_REACHED_MAX_ALLOWED, TO_ME);
		return;
	}

	// Get the player who is being added to the party
	USER* pMember = GetUserId(strUserId);
	if (!pMember)
	{
		CString strMessage = _T("");
		strMessage.Format(IDS_USER_NOT_CONNECTED_NOW, strUserId);
		SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
		return;
	}

	if (pMember->m_State != STATE_GAMESTARTED)
	{
		CString strMessage = _T("");
		strMessage.Format(IDS_USER_NOT_CONNECTED_NOW, strUserId);
		SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
		return;
	}

	// Cannot party with another race
	if (CheckDemon(m_sClass) != CheckDemon(pMember->m_sClass))
	{
		SendServerChatMessage(IDS_USER_CANNOT_PARTY_OTHER_RACE, TO_ME);
		return;
	}

	// Cannot party if player is blocking party invitations
	if (pMember->m_bPartyDeny)
	{
		CString strMessage = _T("");
		strMessage.Format(IDS_USER_BLOCKING_PARTY_INVITE, strUserId);
		SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
		return;
	}

	// Cannot party if the player is already been sent a party invitation
	// and the invitation is still active
	if (pMember->m_bPartyInvite)
	{
		CString strMessage = _T("");
		strMessage.Format(IDS_USER_ALREADY_IN_PARTY, strUserId);
		SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
		return;
	}

	// Cannot party if the player is already in a party with another player
	if (pMember->m_bInParty)
	{
		CString strMessage = _T("");
		strMessage.Format(IDS_USER_ALREADY_IN_PARTY, strUserId);
		SendServerChatMessage((LPTSTR)(LPCTSTR)strMessage, TO_ME);
		return;
	}

	m_bPartyInvite = true;
	m_iPartyInviteUid = pMember->m_Uid;

	pMember->m_bPartyInvite = true;
	pMember->m_iPartyInviteUid = m_Uid;

	// Inform client for both players of the invitation to create party
	int index = 0;
	SetByte(m_TempBuf, PKT_PARTY_INVITE, index);
	SetInt(m_TempBuf, m_Uid + USER_BAND, index);
	SetInt(m_TempBuf, pMember->m_Uid + USER_BAND, index);
	SetVarString(m_TempBuf, m_strUserId, strlen(m_strUserId), index);
	Send(m_TempBuf, index);
	pMember->Send(m_TempBuf, index);
}
