#pragma once

class CMakeSkillTable  
{
public:
	short m_sId;
	short m_sType;
	CString m_strName;
	short m_sMinExp;
	CString m_strInform;
	CString m_strGetInfo;

	CMakeSkillTable();
	~CMakeSkillTable();
};
