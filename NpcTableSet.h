#pragma once

/////////////////////////////////////////////////////////////////////////////
// CNpcTableSet recordset

class CNpcTableSet : public CRecordset
{
public:
	CNpcTableSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CNpcTableSet)

// Field/Param Data
	//{{AFX_FIELD(CNpcTableSet, CRecordset)
	CString		m_sSid;
	CString		m_sPid;
	CString	m_tType;
	CString		m_sTypeAI;
	CString		m_sMinDamage;
	CString	m_sClass;
	CString	m_strName;
	CString		m_sBlood;
	CString	m_sLevel;
	CString	m_iMaxExp;
	CString	m_sStr;
	CString	m_sWStr;
	CString		m_sAStr;
	CString	m_sDex_at;
	CString		m_sDex_df;
	CString	m_sIntel;
	CString		m_sCharm;
	CString		m_sWis;
	CString		m_sCon;
	CString		m_sMaxHp;
	CString	m_sMaxMp;
	CString	m_iStatus;
	CString	m_iMoral;
	CString	m_at_type;
	CString	m_can_escape;
	CString	m_can_find_enemy;
	CString	m_can_find_our;
	CString	m_have_item_num;
	CString	m_haved_item;
	CString	m_have_magic_num;
	CString	m_haved_magic;
	CString	m_have_skill_num;
	CString	m_haved_skill;
	CString	m_search_range;
	CString	m_movable_range;
	CString	m_move_speed;
	CString	m_standing_time;
	CString	m_regen_time;
	CString	m_bmagicexp;
	CString		m_wmagicexp;
	CString	m_dmagicexp;
	CString	m_sRangeRate;
	CString	m_sBackRate;
	CString		m_sHowTarget;
	CString		m_sMoneyRate;
	CString	m_sMagicNum01;
	CString	m_sMagicRate01;
	CString	m_sMagicNum02;
	CString	m_sMagicRate02;
	CString	m_sMagicNum03;
	CString	m_sMagicRate03;
	CString	m_sDistance;

/*	int		m_sSid;
	int		m_sPid;
	BYTE	m_tType;
	int		m_sTypeAI;
	int		m_sMinDamage;
	int		m_sClass;
	CString	m_strName;
	int		m_sBlood;
	int		m_sLevel;
	long	m_iMaxExp;
	int		m_sStr;
	int		m_sWStr;
	int		m_sAStr;
	int		m_sDex_at;
	int		m_sDex_df;
	int		m_sIntel;
	int		m_sCharm;
	int		m_sWis;
	int		m_sCon;
	int		m_sMaxHp;
	int		m_sMaxMp;
	long	m_iStatus;
	long	m_iMoral;
	BYTE	m_at_type;
	BYTE	m_can_escape;
	BYTE	m_can_find_enemy;
	BYTE	m_can_find_our;
	BYTE	m_have_item_num;
	BYTE	m_haved_item;
	BYTE	m_have_magic_num;
	BYTE	m_haved_magic;
	BYTE	m_have_skill_num;
	BYTE	m_haved_skill;
	BYTE	m_search_range;
	BYTE	m_movable_range;
	int		m_move_speed;
	int		m_standing_time;
	int		m_regen_time;
	int		m_bmagicexp;
	int		m_wmagicexp;
	int		m_dmagicexp;
	int		m_sRangeRate;
	int		m_sBackRate;
	int		m_sHowTarget;
	int		m_sMoneyRate;
	int		m_sMagicNum01;
	int		m_sMagicRate01;
	int		m_sMagicNum02;
	int		m_sMagicRate02;
	int		m_sMagicNum03;
	int		m_sMagicRate03;
	int		m_sDistance;*/
	//}}AFX_FIELD
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNpcTableSet)
	public:
	virtual CString GetDefaultConnect();    // Default connection string
	virtual CString GetDefaultSQL();    // Default SQL for Recordset
	virtual void DoFieldExchange(CFieldExchange* pFX);  // RFX support
	//}}AFX_VIRTUAL

// Implementation
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
