#pragma once

class CClassStoreTableSet : public CRecordset
{
public:
	CClassStoreTableSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CClassStoreTableSet)

// Field/Param Data
	//{{AFX_FIELD(CClassStoreTableSet, CRecordset)
	CString m_sId;
	CString m_sType;
	CString m_sItemNum;
	CString m_strName;
	CString m_sNum;
	CString m_iPoint;

	//}}AFX_FIELD
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClassStoreTableSet)
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
