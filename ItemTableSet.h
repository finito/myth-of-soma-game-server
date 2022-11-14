#pragma once

class CItemTableSet : public CRecordset
{
public:
	CItemTableSet(CDatabase* pDatabase = NULL);
	DECLARE_DYNAMIC(CItemTableSet)

// Field/Param Data
	//{{AFX_FIELD(CItemTableSet, CRecordset)
	int m_sNum;
	int m_sPicNum1;
	int m_sPicNum2;
	int m_sPicNum3;
	int m_sPicNum4;
	CString	m_strName;
	BYTE	m_bType;
	BYTE	m_bArm;
	BYTE	m_bGender;
	int	m_sAb1;
	int	m_sAb2;
	int	m_sWgt;
	long m_iCost;
	int	m_sDur;
	int	m_sNeedStr;
	int	m_sNeedInt;
	int	m_sMinExp;
	int	m_sStr;
	int	m_sDex;
	int	m_sInt;
	int	m_sAt;
	int	m_sDf;
	int	m_sHP;
	int	m_sMP;
	int	m_sMagicNo;
	int	m_sMagicOpt;
	int	m_sTime;
	int	m_sSpecial;
	int	m_sSpOpt1;
	int	m_sSpOpt2;
	int	m_sSpOpt3;

	//}}AFX_FIELD


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItemTableSet)
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
