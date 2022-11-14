#pragma once

class CMakeSkillTableSet : public CRecordset
{
public:
	CMakeSkillTableSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CMakeSkillTableSet)

// Field/Param Data
	//{{AFX_FIELD(CMakeSkillTableSet, CRecordset)

	int m_sId;
	int m_sType;
	CString m_strName;
	int m_sMinExp;
	CString m_strInform;
	CString m_strGetInfo;
	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMakeSkillTableSet)
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
