#include "stdafx.h"
#include "NpcThread.h"

#include "Extern.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define DELAY				250

DWORD	g_dwCurrTimeCount	= GetTickCount();
//////////////////////////////////////////////////////////////////////
// NPC Thread Callback Function
//
UINT NpcThreadProc(LPVOID pParam /* NPC_THREAD_INFO ptr */)
{
	NPC_THREAD_INFO*	pInfo	= (NPC_THREAD_INFO*)pParam;
	CNpc*				pNpc	= NULL;
	COM*				pCom	= NULL;
	CPoint				pt;

	int					i			= 0;
	DWORD				dwDiffTime	= 0;

	DWORD				dwSleep		= 250;

	DWORD				dwSecond	= 1000;

	//srand(GetCurrentThreadId());
	//myrand(1, 10000); myrand(1, 10000);

	if (!pInfo) return 0;

	while (!g_bNpcExit)
	{
		if (g_bShutDown)
		{
			Sleep(250);
			continue;
		}

		for (i = 0; i < NPC_NUM; i++)
		{
			pNpc = pInfo->pNpc[i];
			pCom = pInfo->pCom;
			if (!pNpc)continue;

			g_dwCurrTimeCount = GetTickCount();

			if (g_dwCurrTimeCount - pNpc->m_dwLastTimeCount >= 1000)
			{
				// Handle a poisoned NPC
				if (pNpc->m_sPoisonMagicNo != -1 && pNpc->m_dwPoisonMagicCount > 0)
				{
					--pNpc->m_dwPoisonMagicCount;
					pNpc->PoisonMagic(pCom);
				}
				pNpc->m_dwLastTimeCount = g_dwCurrTimeCount;
			}

			// Stops the NPC from doing things too often... but still searches for enemies..
			if (pNpc->m_Delay > (int)dwSleep /* && !pNpc->m_bFirstLive*/ && pNpc->m_Delay != 0)
			{
				pNpc->m_Delay -= (int)dwSleep;
				if (pNpc->m_Delay < 0) pNpc->m_Delay = 0;

				if (pNpc->m_NpcState == NPC_STANDING && pNpc->FindEnemy(pCom))
				{
					pNpc->m_NpcState = NPC_ATTACKING;

					pNpc->m_Delay = 0;//pNpc->m_sSpeed;
				}

				if (pNpc->m_NpcState != NPC_DEAD)
				{
					continue;
				}
			}

			switch (pNpc->m_NpcState)
			{
			case NPC_LIVE:
				pNpc->NpcLive(pCom);
				break;
			case NPC_DEAD:
				if (g_iClassWar == 0)
				{
					if (pNpc->m_sMid >= NPC_CLASS_BAND_DEVIL && pNpc->m_sMid < 30000)
						break;
				}
				else
				{
					if (pNpc->m_sMid >= NPC_CLASS_BAND_HUMAN && pNpc->m_sMid < NPC_CLASS_BAND_DEVIL)
						break;
				}

				if (pNpc->m_sRegenType == NPCREGENTYPE_NORMAL)
					pNpc->NpcDead(pCom);
				break;
			case NPC_STANDING:
				pNpc->NpcStanding(pCom);
				break;
			case NPC_MOVING:
				pNpc->NpcMoving(pCom);
				break;
			case NPC_ATTACKING:
				pNpc->NpcAttacking(pCom);
				break;
			case NPC_TRACING:
				pNpc->NpcTracing(pCom);
				break;
			case NPC_FIGHTING:
				pNpc->NpcFighting(pCom);
				break;
			case NPC_RECALL:
				pNpc->NpcRecall(pCom);
				break;
/*
			case NPC_BACK:
				pNpc->NpcBack(pCom);
				break;

			case NPC_STRATEGY:
				break;*/
			default:
				break;
			}

			pNpc->m_dwLastThreadTime = GetTickCount();
		}

		dwSleep = 250;
		Sleep(dwSleep);
	}

	return 0;
}

CNpcThread::CNpcThread()
{
	m_pCom =	NULL;
	m_pThread = NULL;

	for (int i = 0; i < NPC_NUM; i++)
	{
		m_pNpc[i] = NULL;
		m_ThreadInfo.m_lNpcUsed[i] = 0;
	}

}

CNpcThread::~CNpcThread()
{
	for (int i = 0; i < NPC_NUM; i++)
	{
		if (m_pNpc[i])
		{
			delete m_pNpc[i];
			m_pNpc[i] = NULL;
		}
	}
}

void CNpcThread::InitThreadInfo(HWND hwnd)
{
	m_ThreadInfo.hWndMsg	= hwnd;
	m_ThreadInfo.pCom		= m_pCom;
}
