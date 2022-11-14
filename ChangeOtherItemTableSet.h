#pragma once

class CChangeOtherItemTableSet : public CRecordset
{
public:
	CChangeOtherItemTableSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CChangeOtherItemTableSet)

// Field/Param Data
	//{{AFX_FIELD(CChangeOtherItemTableSet, CRecordset)
	int m_sId;
	int m_sSpecialNum;
	int m_sItemNum;
	CString m_strItemName;
	//}}AFX_FIELD
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChangeOtherItemTableSet)
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
