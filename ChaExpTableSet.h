#pragma once

class CChaExpTableSet : public CRecordset
{
public:
	CChaExpTableSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CChaExpTableSet)

// Field/Param Data
	//{{AFX_FIELD(CChaExpTableSet, CRecordset)
	int m_sid;
	int m_slevel;
	int m_cha;
	int m_rdcha;
	int m_wdcha;
	//}}AFX_FIELD
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChaExpTableSet)
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
