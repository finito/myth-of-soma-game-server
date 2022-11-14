#pragma once

class CSpecialTable  
{
public:
	short m_sMid;
	BYTE m_tClass;
	CString m_strName;
	BYTE m_tType;
	BYTE m_tTarget;
	short m_sHpdec;
	short m_sMpdec;
	short m_sStmdec;
	BYTE m_tEValue;
	short m_sEDist;
	short m_sDamage;
	int m_iTime;
	int m_iTerm;
	int m_iDecTerm;
	short m_sTHp;
	short m_sTMp;
	short m_sMinExp;
	CString m_strInform;
	CString m_strGetInfo;
	short m_sRate;
	short m_sPlusExpRate;
	short m_sPlusType;

	CSpecialTable();
	~CSpecialTable();
};
