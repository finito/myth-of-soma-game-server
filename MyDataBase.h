#pragma once

#define MAX_DB_USE_TIME	2000

typedef struct _DB_CONNECTION
{
	CDatabase db;
	BOOL bConnect;
	LONG lRef;
	int	iCount;
	DWORD dwGetDBTime;
} DB_CONNECTION;

typedef CTypedPtrArray <CPtrArray, DB_CONNECTION*> DBArray;

class CMyDB
{
public:
	void ReleaseDB(int index);
	CDatabase* GetDB(int &index);
	BOOL DBConnect(LPCTSTR strConnect);
	void Init(int num);

	DBArray	m_arDB;
	int	m_iTotalConnection;
	CString	m_strConnect;

	CMyDB();
	virtual ~CMyDB();
};
