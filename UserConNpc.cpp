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

extern CSearch* g_pUserList;
extern CMy1p1EmuDlg* g_pMainDlg;

CNpc* USER::GetNpc(int nid)
{
	if (nid < 0 || nid >= g_arNpc.GetSize())return NULL;

	return g_arNpc[nid];
}

int USER::MakeRangeInfoToMe(CNpc* pNpc, BYTE tMode, TCHAR *pData)
{
	if (!pNpc) return 0;
	int index = 0;
	pNpc->FillNpcInfo(pData, index, tMode, m_pCom);
	return index;
}

void USER::AddRangeInfoToMe(CNpc *pNpc, BYTE tMode)
{
	TCHAR pData[1024];
	int index = MakeRangeInfoToMe(pNpc, tMode, pData);
	if (index)
	{
		MYSHORT slen;
		m_UserInfoBuf[m_UserInfoIndex++] = (char) PACKET_START1;
		m_UserInfoBuf[m_UserInfoIndex++] = (char) PACKET_START2;

		slen.i = index;

		m_UserInfoBuf[m_UserInfoIndex++] = (char) (slen.b[0]);
		m_UserInfoBuf[m_UserInfoIndex++] = (char) (slen.b[1]);

		if (m_CryptionFlag == 1)
		{
			SomaEncryption.Encode_Decode((BYTE*)m_UserInfoBuf + m_UserInfoIndex, (BYTE*)pData, index);
		}
		else
		{
			memcpy(m_UserInfoBuf + index, pData, index);
		}
		m_UserInfoIndex += index;

		m_UserInfoBuf[m_UserInfoIndex++] = (char) PACKET_END1;
		m_UserInfoBuf[m_UserInfoIndex++] = (char) PACKET_END2;

		if (m_UserInfoIndex >= 8000)
			SendRangeInfoToMe();
	}
}
