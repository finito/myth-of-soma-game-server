#pragma once

class CExpTableDSet : public CRecordset
{
public:
	CExpTableDSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CExpTableDSet)

// Field/Param Data
	//{{AFX_FIELD(CExpTableDSet, CRecordset)
	int m_sid;
	int m_slevel;
	int m_maxexp;
	int m_rdexp;
	int m_wdexp;
	//}}AFX_FIELD
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExpTableDSet)
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
