#pragma once

class CPlusSpecialTableSet : public CRecordset
{
public:
	CPlusSpecialTableSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CPlusSpecialTableSet)

// Field/Param Data
	//{{AFX_FIELD(CPlusSpecialTableSet, CRecordset)

	CString m_sId;
	CString m_sSpecial;
	CString m_sSpOpt1;
	CString m_sSpOpt2;
	CString m_sSpOpt3;
	//}}AFX_FIELD
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPlusSpecialTableSet)
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
