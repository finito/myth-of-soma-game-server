#pragma once

class CSkillExpTableDSet : public CRecordset
{
public:
	CSkillExpTableDSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CSkillExpTableDSet)

// Field/Param Data
	//{{AFX_FIELD(CSkillExpTableDSet, CRecordset)
	int m_sid;
	int m_slevel;
	int m_axe;
	int m_bow;
	int m_knuckle;
	int m_spear;
	int m_staff;
	int m_sword;
	int m_darkmg;
	int m_whitemg;
	int m_bluemg;
	int m_wpmake;
	int m_ammake;
	int m_acmake;
	int m_pomake;
	int m_ckmake;
	int m_rdaxe;
	int m_wdaxe;
	int m_rdbow;
	int m_wdbow;
	int m_rdknuckle;
	int m_wdknuckle;
	int m_rdspear;
	int m_wdspear;
	int m_rdstaff;
	int m_wdstaff;
	int m_rdsword;
	int m_wdsword;
	int m_rddarkmg;
	int m_wddarkmg;
	int m_rdwhitemg;
	int m_wdwhitemg;
	int m_rdbluemg;
	int m_wdbluemg;
	//}}AFX_FIELD
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSkillExpTableDSet)
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
