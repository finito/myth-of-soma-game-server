#pragma once

class CStatExpTableSet : public CRecordset
{
public:
	CStatExpTableSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CStatExpTableSet)

// Field/Param Data
	//{{AFX_FIELD(CStatExpTableSet, CRecordset)
	int m_sid;
	int m_slevel;
	int m_str;
	int m_dex;
	int m_int;
	int m_wis;
	int m_rdstr;
	int m_wdstr;
	int m_rddex;
	int m_wddex;
	int m_rdint;
	int m_wdint;
	int m_rdwis;
	int m_wdwis;
	//}}AFX_FIELD
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatExpTableSet)
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
