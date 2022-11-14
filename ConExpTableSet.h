#pragma once

class CConExpTableSet : public CRecordset
{
public:
	CConExpTableSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CConExpTableSet)

// Field/Param Data
	//{{AFX_FIELD(CConExpTableSet, CRecordset)
	int m_sid;
	int m_slevel;
	int m_con;
	int m_rdcon;
	int m_wdcon;
	//}}AFX_FIELD
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConExpTableSet)
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
