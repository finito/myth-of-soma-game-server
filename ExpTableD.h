#pragma once

class CExpTableD 
{
public:
	short m_sid;
	short m_slevel;
	DWORD m_maxexp;
	DWORD m_rdexp;
	DWORD m_wdexp;

	CExpTableD();
	~CExpTableD();
};
