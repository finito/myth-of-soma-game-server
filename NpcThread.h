#pragma once

#include "Npc.h"
#include "COM.h"

UINT NpcThreadProc(LPVOID pParam /* NPC_THREAD_INFO ptr */);

typedef struct _NPC_THREAD_INFO
{
	COM* pCom;
	CNpc* pNpc[NPC_NUM];
	long m_lNpcUsed[NPC_NUM];
	HWND hWndMsg;
} NPC_THREAD_INFO;

class CNpcThread
{
public:

	CNpcThread();
	virtual ~CNpcThread();

	void InitThreadInfo(HWND hwnd);
	COM* m_pCom;
	CNpc* m_pNpc[NPC_NUM];

	NPC_THREAD_INFO	m_ThreadInfo;
	CWinThread*	m_pThread;
};
