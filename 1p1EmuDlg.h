// 1p1EmuDlg.h : header file
//

#pragma once

#include "Iocport.h"
#include "Ini.h"
#include "COM.h"

#define	WM_CLOSE_PROCESS		(WM_USER + 10)

// CMy1p1EmuDlg dialog
class CMy1p1EmuDlg : public CDialog
{
// Construction
public:
	CMy1p1EmuDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MY1P1EMU_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	LONG OnCloseProcess(UINT wParam, LONG lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL DestroyWindow();
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()	

private:

	// Other
public:
	static CIOCPort	m_Iocport;
	int m_nYear, m_nMonth, m_nDay, m_nHour, m_nMin, m_nWeather;
	COM	m_Com;
	CIni m_Ini;	// Soma.ini contains settings etc

	void InitEnvironment();
	void UserFree(int uid);
	COM *GetCOM() { return &m_Com; }

	void LogToFile(CString entry);

private:
	// Log
	CFile m_LogFile;

	bool GetServerInfoData();
	// Monster
	bool GetNpcTableData();
	bool CreateNpcThread();
	bool GetNpcChatTable();
	void ResumeAI();

	// Item
	bool GetItemTable();

	bool GetPlusSpecialTable();
	bool GetMagicTable();
	bool GetSpecialAttackTable();
	bool GetMakeSkillTable();
	bool GetClassStoreTable();
	bool GetStoreTable();
	bool GetTownPortal();
	bool GetTownPortalFixed();
	bool GetChangeOtherItem();
	bool GetTraderExchangeTable();

	// Level up (EXP) tables HSOMA
	bool GetExpTable();
	bool GetSkillExpTable();
	bool GetStatExpTable();
	bool GetConExpTable();
	bool GetChaExpTable();

	// Level up (EXP) tables DSOMA
	bool GetExpTableD();
	bool GetSkillExpTableD();
	bool GetStatExpTableD();
	bool GetConExpTableD();
	bool GetChaExpTableD();
	
	bool LoadZoneData(); // Loads map and event files
	void UpdateGameTime();
	void GetTimeFromIni();
	void SetTimeToIni();
	void UpdateWeather();

	void GetClassWarFromIni();

	// Zone (Human / Devil)
	bool GetZoneClass();

	// Zone (Extra details for esoma client)
	void GetZoneDetails();

	// Guild
	bool GetGuildData();
	bool GetGuildMemberData();
	bool GetGuildReqData();

	// Event View Data
	bool GetEventViewData();

	// Special Item Data
	bool GetSpecialItemData();

	// Change Rand Item Data
	bool GetChangeRandItemData();

	// Guild Town Data
	bool GetGuildTownData();
	void SetGuildTownData();
	short GetGuildPictureNum(TCHAR* strGuildName);
	void GetGuildTownWarFromIni();
	void StatusGuildTownWar(short sTownNum);

	void StatusClassWar();

	// User Save
	void SaveAllUserData();

	// Store Restock
	void StoreRestock(bool bFull);

	// Npc Game Time Spawn
	void NpcSpawnByGameTime();

	// Send message
	void SendAllMessage(TCHAR* pMsg);
};
