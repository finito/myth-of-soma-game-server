#pragma once

class CStatExpTable  
{
public:
	short m_sid;
	short m_slevel;
	DWORD m_str;
	DWORD m_dex;
	DWORD m_int;
	DWORD m_wis;
	DWORD m_rdstr;
	DWORD m_wdstr;
	DWORD m_rddex;
	DWORD m_wddex;
	DWORD m_rdint;
	DWORD m_wdint;
	DWORD m_rdwis;
	DWORD m_wdwis;

	CStatExpTable();
	~CStatExpTable();
};
