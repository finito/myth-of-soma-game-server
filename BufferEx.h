#pragma once

#define AddString(p) Add(p, _tcslen(p))

class CBufferEx
{
public:
	void AddData(TCHAR* pBuf, int nLength);
	void AddLongString(TCHAR* pBuf, int nLength);
	int GetLength();
	void Add(DWORD nVal);
	void Add(WORD nVal);
	void Add(short nVal);
	void Add(int nVal);
	void Add(TCHAR* pBuf, int nLength);
	void Add(BYTE ch);

	CBufferEx(int nSize = MAX_PACKET_SIZE);
	virtual ~CBufferEx();

	inline operator LPTSTR()
	{
		return m_pData;
	}

private:
	TCHAR* m_pData;
	UINT m_nIndex;
	UINT m_nSize;
};
