#pragma once

class CConExpTable  
{
public:
	short m_sid;
	short m_slevel;
	DWORD m_con;
	DWORD m_rdcon;
	DWORD m_wdcon;

	CConExpTable();
	~CConExpTable();
};
