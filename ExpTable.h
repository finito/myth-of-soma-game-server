#pragma once

class CExpTable  
{
public:
	short m_sid;
	short m_slevel;
	DWORD m_maxexp;
	DWORD m_rdexp;
	DWORD m_wdexp;
	CExpTable();
	~CExpTable();
};
