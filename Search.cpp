#include "stdafx.h"
#include "Search.h"

CSearch::CSearch()
{
}

CSearch::CSearch(int dim)
 : CSearchBase<USER>(dim)
{
}

CSearch::~CSearch()
{
}

USER* CSearch::GetUserUid(int uid)
{
	if (uid < 0 || uid >= m_arrayDim)return NULL;

	return GetObject(uid);
}

void CSearch::SetUserUid(int uid, USER *pUser)
{
	if (uid < 0 || uid >= m_arrayDim)return;

	SetObject(uid, pUser);
}

void CSearch::FreeUser(int uid)
{
	if (uid < 0 || uid >= m_arrayDim)return;

	SetObjectToFree(uid);
}
