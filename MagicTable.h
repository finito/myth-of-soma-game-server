#pragma once

class CMagicTable  
{
public:
	short m_sMid;
	BYTE m_tClass;
	CString m_strName;
	BYTE m_tType01;
	BYTE m_tType02;
	short m_sStartTime;
	BYTE m_tTarget;
	short m_sMpdec;
	BYTE m_tEValue;
	short m_sEDist;
	short m_sRange;
	short m_sDamage;
	int m_iTime;
	int m_iTerm;
	short m_sMinLevel;
	short m_sMinMxp;
	short m_sMinInt;
	short m_sMinMoral;
	short m_sMaxMoral;
	CString m_strInform;
	CString m_strGetInfo;
	short m_sRate;
	short m_sPlusExpRate;
	short m_sPlusType;

	CMagicTable();
	~CMagicTable();
};
