#pragma once

class CItemTable  
{
public:
	short m_sNum;
	short m_sPicNum1;
	short m_sPicNum2;
	short m_sPicNum3;
	short m_sPicNum4;
	CString	m_strName;
	BYTE	m_bType;
	BYTE	m_bArm;
	BYTE	m_bGender;
	short	m_sAb1;
	short	m_sAb2;
	short	m_sWgt;
	int		m_iCost;
	short	m_sDur;
	short	m_sNeedStr;
	short	m_sNeedInt;
	short	m_sMinExp;
	short	m_sStr;
	short	m_sDex;
	short	m_sInt;
	short	m_sAt;
	short	m_sDf;
	short	m_sHP;
	short	m_sMP;
	short	m_sMagicNo;
	short	m_sMagicOpt;
	short	m_sTime;
	short	m_sSpecial;
	short	m_sSpOpt[MAX_SPECIAL_OPT];
	CItemTable();
	~CItemTable();
};
