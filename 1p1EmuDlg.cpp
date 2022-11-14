#include "stdafx.h"
#include "1p1Emu.h"
#include "1p1EmuDlg.h"
#include "Extern.h"
#include "Search.h"
#include "User.h"
#include "BufferEx.h"

// #### DATABASE LOADING HEADERS #### //
// ITEM
#include "ItemTableSet.h"
#include "PlusSpecialTableSet.h"
#include "MagicTableSet.h"
#include "SpecialTableSet.h"
#include "MakeSkillTableSet.h"
#include "ClassStoreTableSet.h"
#include "StoreSet.h"
#include "TownPortalFixedSet.h" // For Soma tb_townportal_fixed table
#include "TownPortalSet.h" // For Soma tb_townportal table
#include "ChangeOtherItemTableSet.h"
#include "TraderExchangeTableSet.h"

// MONSTER
#include "NpcTableSet.h"
#include "NpcTable.h"
#include "NpcPosSet.h"
#include "NpcChatSet.h"

// EXP TABLES HSOMA
#include "ExpTableSet.h"
#include "SkillExpTableSet.h"
#include "StatExpTableSet.h"
#include "ConExpTableSet.h"
#include "ChaExpTableSet.h"

// EXP TABLES DSOMA
#include "ExpTableDSet.h"
#include "SkillExpTableDSet.h"
#include "StatExpTableDSet.h"
#include "ConExpTableDSet.h"
#include "ChaExpTableDSet.h"

// GUILD
#include "Guild.h"

// #### END DATABASE LOADING HEADERS #### //

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// Global Variable
const int GAME_TIME = 100; // Timer ID for updating game time

CMy1p1EmuDlg* g_pMainDlg = NULL;
CSearch* g_pUserList = NULL;
CMyDB g_DB[AUTOMATA_THREAD+1];
CIOCPort CMy1p1EmuDlg::m_Iocport;
ServerArray	g_ServerInfo;
MakeSkillTableArray	g_arMakeSkillTable;
SpecialAttackTableArray	g_arSpecialAttackTable;
MagicTableArray	g_arMagicTable;
NpcChatArray g_arNpcChat;
EventViewDataArray g_arEventViewData;
SpecialItemDataArray g_arSpecialItemData;
ChangeRandItemDataArray g_arChangeRandItemData[20];
GuildTownDataArray g_arGuildTownData;

// NPC
bool g_bNpcExit	= false;
long g_TotalNPC;
long g_CurrentNPC;
long g_CurrentNPCError;
NpcArray g_arNpc;
NpcTypeNoLiveArray g_arNpcTypeNoLive;
NpcTypeTimeArray g_arNpcTypeTime;
NpcTypeClassArray g_arNpcTypeClass;
NpcTypeGuildArray g_arNpcTypeGuild;
NpcTableArray g_arNpcTable;
NpcThreadArray g_arNpcThread;
NpcDetecterArray g_arNpcTypeDetecter;

// Item
ItemTableMap g_mapItemTable;
PlusSpecialTableArray g_arPlusSpecialTable;
ClassStoreTableArray g_arClassStoreTable;
StoreArray g_arStore;
TownPortalArray	g_arTownPortal;
TownPortalFixedArray g_arTownPortalFixed;

// Map
ZoneArray g_Zones;
EventArray g_Events;

// Dex hit rate array for attack success (Low) dex attack >= -50 and < 0
int g_DexHitRateLow[51] = {49, 48, 47, 46, 45, 44, 43, 42, 41,
			   40, 39, 38, 37, 36, 35, 35, 34, 34,
			   33, 33, 32, 32, 31, 31, 30, 30, 29,
			   29, 28, 28, 27, 27, 26, 26, 25, 25,
			   25, 24, 24, 24, 23, 23, 23, 21, 21,
			   21, 21, 21, 21, 20};

// Dex hit rate array for when Monster dex attack (High) dex attack > 0 and < 50
int g_DexHitRateHigh[51] = {51, 52, 53, 54, 55, 56, 57, 58, 59,
				60, 61, 62, 63, 64, 65, 66, 67, 68,
				69, 70, 71, 72, 73, 74, 75, 76, 77,
				78, 79, 80, 81, 82, 83, 84, 85, 86,
				87, 88, 89, 90, 91, 92, 93, 94, 95,
				96, 97, 98, 99, 99};

// Level up (EXP) Tables HSOMA
ExpTableArray g_arExpTable;
SkillExpTableArray g_arSkillExpTable;
StatExpTableArray g_arStatExpTable;
ConExpTableArray g_arConExpTable;
ChaExpTableArray g_arChaExpTable;

// Level up (EXP) Tables DSOMA
ExpTableDArray g_arExpTableD;
SkillExpTableDArray g_arSkillExpTableD;
StatExpTableDArray g_arStatExpTableD;
ConExpTableDArray g_arConExpTableD;
ChaExpTableDArray g_arChaExpTableD;

// Change Other Item
ChangeOtherItemTableArray g_arChangeOtherItemTable;

// Trader Exchange Item
TraderExchangeTableArray g_arTraderExchangeTable;

// Class War
bool g_bClassWarEnd;	// True if the class war has reached time when it should end
bool g_bClassWarStart;	// True if the class war has reached time when it should start
bool g_bClassWar;		// Is the class war currently in progress.
int g_iClassWarTime;
int g_iClassWarCount;	// The amount of wins in a row of WotW
int g_iClassWarLimit;	// Limits the % of damage / defense reduced after winning X WotW
int g_iClassWarMinus;	// The amount damage / defense is reduced when another race is on opposite race map and its not a wotw map and wotw is not on
int g_iClassWarPlusAttack;
int g_iClassWarPlusDefense;
int g_iClassWar;		// The class whos owns the WotW area

// Guild Town War
bool g_bGuildTownWarEnd[MAX_GUILD_TOWN];	// True if the guild town war has reached time when it should end
bool g_bGuildTownWarStart[MAX_GUILD_TOWN]; // True if the guild town war has reached time when it should start
bool g_bGuildTownWar[MAX_GUILD_TOWN]; // Is the guild town war currently in progress.
int g_iGuildTownRange[MAX_GUILD_TOWN]; // Map Range number for guild towns
int g_iGuildTownWarStartTime; // The time the guild town war starts
int g_iGuildTownWarEndTime; // The time the guild town war ends

// Zone (Human / Devil)
ZoneClassMap g_mapZoneClass;

// Zone (Extra details for esoma client)
ZoneDetailsMap g_mapZoneDetails;

// Guild
GuildArray g_arGuild;

// Database
char g_strDatabaseConnectionString[256];

CMy1p1EmuDlg::CMy1p1EmuDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMy1p1EmuDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nYear = 0;
	m_nMonth = 0;
	m_nDay = 0;
	m_nHour = 0;
	m_nMin = 0;
	m_nWeather = 0;
}

void CMy1p1EmuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMy1p1EmuDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_CLOSE_PROCESS, OnCloseProcess)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CMy1p1EmuDlg message handlers

BOOL CMy1p1EmuDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	g_pMainDlg = this;

	CTime currentTime = CTime::GetCurrentTime();
	CString fileName;
	fileName.Format("LogOutput/%d-%d-%d.txt", currentTime.GetYear(), currentTime.GetMonth(), currentTime.GetDay(), currentTime.GetHour(), currentTime.GetMinute());

	HANDLE hFile = CreateFile(fileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		CreateDirectory("LogOutput", NULL);
		hFile = CreateFile(fileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			AfxMessageBox("Failed to open server log file\n");
			EndDialog(IDCANCEL);
			return FALSE;
		}
	}
	m_LogFile.m_hFile = hFile;

	m_Ini.SetPath("soma.ini");
	GetTimeFromIni();
	GetClassWarFromIni();

	InitEnvironment();

	// Initialize database
	char dsn[30];
	strcpy_s(dsn, sizeof(dsn), m_Ini.GetProfileString("DATABASE", "DSN", "soma"));
	char uid[30];
	strcpy_s(uid, sizeof(uid), m_Ini.GetProfileString("DATABASE", "UID", "soma"));
	char pwd[30];
	strcpy_s(pwd, sizeof(pwd), m_Ini.GetProfileString("DATABASE", "PWD", "soma"));
	sprintf_s(g_strDatabaseConnectionString, sizeof(g_strDatabaseConnectionString), _T("ODBC;DSN=%s;UID=%s;PWD=%s"), dsn, uid, pwd);

	for (int i = 0; i < AUTOMATA_THREAD + 1; i++)
	{
		g_DB[i].Init(10);
		g_DB[i].DBConnect(g_strDatabaseConnectionString);
	}

	//----------------------------------------------------------------------
	//	Sets a random number starting point.
	//----------------------------------------------------------------------
	srand(static_cast<unsigned int>(time(NULL)));
	for (int i = 0; i < 10; i++) myrand(1, 10000);	// don't delete

	// Communication Part Initialize ...
	m_Com.Init();

	// Server Information DB Open & Load ...
	if (!GetServerInfoData())
	{
		AfxMessageBox("Failed loading Server info table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	// Load Zone & Event...
	if (!LoadZoneData())
	{
		EndDialog(IDCANCEL);
		return FALSE;
	}

	// Load Item Table
	if (!GetItemTable())
	{
		AfxMessageBox("Failed loading BasicItem table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	// Load tb_plus_special Table
	if (!GetPlusSpecialTable())
	{
		AfxMessageBox("Failed loading tb_plus_special table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	// Load Magic Table
	if (!GetMagicTable())
	{
		AfxMessageBox("Failed loading Magic table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	// Load Special_Attack Table
	if (!GetSpecialAttackTable())
	{
		AfxMessageBox("Failed loading Special_Attack table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	if (!GetMakeSkillTable())
	{
		AfxMessageBox("Failed loading MakeSkill table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	if (!GetNpcTableData())
	{
		EndDialog(IDCANCEL);
		return FALSE;
	}

	if (!CreateNpcThread())
	{
		EndDialog(IDCANCEL);
		return FALSE;
	}

	//----------------------------------------------------------------------
	//	Load NPC Chat Table
	//----------------------------------------------------------------------
	if (!GetNpcChatTable())
	{
		AfxMessageBox("### Fail To Read NPC_CHAT!! ###");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	if (!GetStoreTable())
	{
		AfxMessageBox("Failed loading store table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	if (!GetClassStoreTable())
	{
		AfxMessageBox("Failed loading tb_ClassPointStore table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	if (!GetTownPortal())
	{
		AfxMessageBox("Failed loading tb_townportal table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	if (!GetTownPortalFixed())
	{
		AfxMessageBox("Failed loading tb_townportal_fixed table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	if (!GetChangeOtherItem())
	{
		AfxMessageBox("Failed loading tb_changeotheritem table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	if (!GetTraderExchangeTable())
	{
		AfxMessageBox("Failed loading trader_exchange table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	//----------------------------------------------------------------------
	//	Load Level Up Tables HSOMA
	//----------------------------------------------------------------------
	if (!GetExpTable())
	{
		AfxMessageBox("Failed loading tb_exp table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	if (!GetSkillExpTable())
	{
		AfxMessageBox("Failed loading tb_inc_exp table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	if (!GetStatExpTable())
	{
		AfxMessageBox("Failed loading tb_inc table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	if (!GetConExpTable())
	{
		AfxMessageBox("Failed loading tb_inc_con table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	if (!GetChaExpTable())
	{
		AfxMessageBox("Failed loading tb_inc_cha table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	//----------------------------------------------------------------------
	//	Load Level Up Tables DSOMA
	//----------------------------------------------------------------------
	if (!GetExpTableD())
	{
		AfxMessageBox("Failed loading d_tb_exp table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	if (!GetSkillExpTableD())
	{
		AfxMessageBox("Failed loading d_tb_inc_exp table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	if (!GetStatExpTableD())
	{
		AfxMessageBox("Failed loading d_tb_inc table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	if (!GetConExpTableD())
	{
		AfxMessageBox("Failed loading d_tb_inc_con table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	if (!GetChaExpTableD())
	{
		AfxMessageBox("Failed loading d_tb_inc_cha table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	// Sets the class of each zone (Human / Devil)
	if (!GetZoneClass())
	{
		AfxMessageBox("Failed loading zone class data\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	// Sets the extra details of each zone
	GetZoneDetails();

	// Guild Table
	if (!GetGuildData())
	{
		AfxMessageBox("Failed loading guild table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	// Guild User Table
	if (!GetGuildMemberData())
	{
		AfxMessageBox("Failed loading guild_user table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	// Guild Req Table
	if (!GetGuildReqData())
	{
		AfxMessageBox("Failed loading guild_req table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	// Event View Table
	if (!GetEventViewData())
	{
		AfxMessageBox("Failed loading event_view table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	// Special Item Table
	if (!GetSpecialItemData())
	{
		AfxMessageBox("Failed loading tb_special_item table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	// Change Rand Item Data
	if (!GetChangeRandItemData())
	{
		AfxMessageBox("Failed loading change_rand_item table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	// Guild Town Data
	if (!GetGuildTownData())
	{
		AfxMessageBox("Failed loading guild_town table\n");
		EndDialog(IDCANCEL);
		return FALSE;
	}

	SetGuildTownData();
	for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
	{
		g_bGuildTownWarStart[i] = false;
		g_bGuildTownWarEnd[i] = false;
		g_bGuildTownWar[i] = false;
	}
	g_iGuildTownRange[0] = MAP_AREA_HSOMA_GV;
	g_iGuildTownRange[1] = MAP_AREA_DSOMA_GV;

	g_bClassWarStart = false;
	g_bClassWarEnd = false;
	g_bClassWar = false;

	GetGuildTownWarFromIni();

	SetTimer(GAME_TIME, 10000, NULL);

	ResumeAI();

	// DEBUG ONLY m_Com.DebugSetThrowItem();

	m_Iocport.Init(MAX_USER, 10, 8);

	for (int i = 0; i < MAX_USER; i++)
	{
		m_Iocport.m_SockArrayInActive[i] = new USER;
	}

	if (!m_Iocport.Listen(12000))
	{
		AfxMessageBox("FAIL TO CREATE LISTEN STATE");
		AfxPostQuitMessage(0);
		return FALSE;
	}

	::ResumeThread(m_Iocport.m_hAcceptThread);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMy1p1EmuDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMy1p1EmuDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMy1p1EmuDlg::OnCancel()
{
	g_bShutDown = 1;
	::SuspendThread(m_Iocport.m_hAcceptThread);
	SaveAllUserData();

	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	PostMessage(WM_CLOSE_PROCESS);
}

LONG CMy1p1EmuDlg::OnCloseProcess(UINT wParam, LONG lParam)
{
	EndDialog(IDOK);
	return 0L;
}

void CMy1p1EmuDlg::OnDestroy()
{
	// Kill timers
	KillTimer(GAME_TIME);
}

BOOL CMy1p1EmuDlg::DestroyWindow()
{
	// Close log file
	m_LogFile.Close();

	// ServerInfo array Delete ...
	for (int i = 0; i < g_ServerInfo.GetSize(); i++)
	{
		delete g_ServerInfo[i];
	}
	g_ServerInfo.RemoveAll();

	g_bNpcExit = true; // Exit NPC Threads

	for (int i = 0; i < g_arNpcThread.GetSize(); i++)
	{
		WaitForSingleObject(g_arNpcThread.GetAt(i)->m_pThread->m_hThread, INFINITE);
	}

	// Item Table Array Delete ...
	CItemTable *item_data;
	int number = 0;
	POSITION pos = g_mapItemTable.GetStartPosition();
	while (pos)
	{
		g_mapItemTable.GetNextAssoc(pos, number, item_data);
		delete item_data;
	}
	g_mapItemTable.RemoveAll();

	// Map(Zone) Array Delete...
	for (int i = 0; i < g_Zones.GetSize(); i++)
	{
		delete g_Zones[i];
	}
	g_Zones.RemoveAll();

	// Event Array Delete
	for (int i = 0; i < g_Events.GetSize(); i++)
	{
		delete g_Events[i];
	}
	g_Events.RemoveAll();

	// PlusSpecial Array Delete
	for (int i = 0; i < g_arPlusSpecialTable.GetSize(); i++)
	{
		delete g_arPlusSpecialTable[i];
	}
	g_arPlusSpecialTable.RemoveAll();

	// NpcTable Array Delete
	for (int i = 0; i < g_arNpcTable.GetSize(); i++)
	{
		delete g_arNpcTable[i];
	}
	g_arNpcTable.RemoveAll();


	// NpcTypeNoLive Array Delete
	for (int i = 0; i < g_arNpcTypeNoLive.GetSize(); i++)
	{
		delete g_arNpcTypeNoLive[i];
	}
	g_arNpcTypeNoLive.RemoveAll();

	// NpcTypeTime Array Delete
	for (int i = 0; i < g_arNpcTypeTime.GetSize(); i++)
	{
		delete g_arNpcTypeTime[i];
	}
	g_arNpcTypeTime.RemoveAll();

	// NpcTypeClass Array Delete
	for (int i = 0; i < g_arNpcTypeClass.GetSize(); i++)
	{
		delete g_arNpcTypeClass[i];
	}
	g_arNpcTypeClass.RemoveAll();

	// NpcTypeGuild Array Delete
	for (int i = 0; i < g_arNpcTypeGuild.GetSize(); i++)
	{
		delete g_arNpcTypeGuild[i];
	}
	g_arNpcTypeGuild.RemoveAll();

	// NpcThread Array Delete
	for (int i = 0; i < g_arNpcThread.GetSize(); i++)
	{
		delete g_arNpcThread[i];
		g_arNpcThread[i] = NULL;
	}
	g_arNpcThread.RemoveAll();

	// NpcTypeDetecter Array Delete
	g_arNpcTypeDetecter.RemoveAll();

	// Make Skill Array Delete
	for (int i = 0; i < g_arMakeSkillTable.GetSize(); i++)
	{
		delete g_arMakeSkillTable[i];
	}
	g_arMakeSkillTable.RemoveAll();

	// Special Attack Array Delete
	for (int i = 0; i < g_arSpecialAttackTable.GetSize(); i++)
	{
		delete g_arSpecialAttackTable[i];
	}
	g_arSpecialAttackTable.RemoveAll();

	// Magic Table Array Delete ...
	for (int i = 0; i < g_arMagicTable.GetSize(); i++)
	{
		delete g_arMagicTable[i];
	}
	g_arMagicTable.RemoveAll();

	// Npc Chat Array Delete ...
	for (int i = 0; i < g_arNpcChat.GetSize(); i++)
	{
		delete g_arNpcChat[i];
	}
	g_arNpcChat.RemoveAll();

	// ClassStore Table Array Delete ...
	for (int i = 0; i < g_arClassStoreTable.GetSize(); i++)
	{
		delete g_arClassStoreTable[i];
	}
	g_arClassStoreTable.RemoveAll();

	// TownPortal Table Array Delete ...
	for (int i = 0; i < g_arTownPortal.GetSize(); i++)
	{
		delete g_arTownPortal[i];
	}
	g_arTownPortal.RemoveAll();

	// TownPortalFixed Table Array Delete ...
	for (int i = 0; i < g_arTownPortalFixed.GetSize(); i++)
	{
		delete g_arTownPortalFixed[i];
	}
	g_arTownPortalFixed.RemoveAll();

	// HSOMA EXP Table Array Delete ...
	for (int i = 0; i < g_arExpTable.GetSize(); i++)
	{
		delete g_arExpTable[i];
	}
	g_arExpTable.RemoveAll();

	// HSOMA Skill EXP Table Array Delete ...
	for (int i = 0; i < g_arSkillExpTable.GetSize(); i++)
	{
		delete g_arSkillExpTable[i];
	}
	g_arSkillExpTable.RemoveAll();

	// HSOMA Stat EXP Table Array Delete ...
	for (int i = 0; i < g_arStatExpTable.GetSize(); i++)
	{
		delete g_arStatExpTable[i];
	}
	g_arStatExpTable.RemoveAll();

	// HSOMA Con EXP Table Array Delete ...
	for (int i = 0; i < g_arConExpTable.GetSize(); i++)
	{
		delete g_arConExpTable[i];
	}
	g_arConExpTable.RemoveAll();

	// HSOMA Cha EXP Table Array Delete ...
	for (int i = 0; i < g_arChaExpTable.GetSize(); i++)
	{
		delete g_arChaExpTable[i];
	}
	g_arChaExpTable.RemoveAll();

	// DSOMA EXP Table Array Delete ...
	for (int i = 0; i < g_arExpTableD.GetSize(); i++)
	{
		delete g_arExpTableD[i];
	}
	g_arExpTableD.RemoveAll();

	// DSOMA Skill EXP Table Array Delete ...
	for (int i = 0; i < g_arSkillExpTableD.GetSize(); i++)
	{
		delete g_arSkillExpTableD[i];
	}
	g_arSkillExpTable.RemoveAll();

	// DSOMA Stat EXP Table Array Delete ...
	for (int i = 0; i < g_arStatExpTableD.GetSize(); i++)
	{
		delete g_arStatExpTableD[i];
	}
	g_arStatExpTableD.RemoveAll();

	// DSOMA Con EXP Table Array Delete ...
	for (int i = 0; i < g_arConExpTableD.GetSize(); i++)
	{
		delete g_arConExpTableD[i];
	}
	g_arConExpTableD.RemoveAll();

	// DSOMA Cha EXP Table Array Delete ...
	for (int i = 0; i < g_arChaExpTableD.GetSize(); i++)
	{
		delete g_arChaExpTableD[i];
	}
	g_arChaExpTableD.RemoveAll();

	// Change Other Item Table Array Delete ...
	for (int i = 0; i < g_arChangeOtherItemTable.GetSize(); i++)
	{
		delete g_arChangeOtherItemTable[i];
	}
	g_arChangeOtherItemTable.RemoveAll();

	// Trader Exchange Item Table Array Delete ...
	for (int i = 0; i < g_arTraderExchangeTable.GetSize(); i++)
	{
		delete g_arTraderExchangeTable[i];
	}
	g_arTraderExchangeTable.RemoveAll();

	// Store Table Array Delete ...
	for (int i = 0; i < g_arStore.GetSize(); i++)
	{
		delete g_arStore[i];
	}
	g_arStore.RemoveAll();

	// Guild Table Array Delete
	for (int i = 0; i < g_arGuild.GetSize(); i++)
	{
		delete g_arGuild[i];
	}
	g_arGuild.RemoveAll();

	// Event View Data Array Delete ...
	for (int i = 0; i < g_arEventViewData.GetSize(); i++)
	{
		delete g_arEventViewData[i];
	}
	g_arEventViewData.RemoveAll();

	// Special Item Data Array Delete ...
	for (int i = 0; i < g_arSpecialItemData.GetSize(); i++)
	{
		delete g_arSpecialItemData[i];
	}
	g_arSpecialItemData.RemoveAll();


	// Change Rand Item Data Array Delete ...
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < g_arChangeRandItemData[i].GetSize(); j++)
		{
			delete g_arChangeRandItemData[i][j];
		}
		g_arChangeRandItemData[i].RemoveAll();
	}

	// Guild Town Data Array Delete ...
	for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
	{
		delete g_arGuildTownData[i];
	}
	g_arGuildTownData.RemoveAll();

	if (g_pUserList)
	{
		delete g_pUserList;
	}
	g_pUserList = NULL;

	return CDialog::DestroyWindow();
}

void CMy1p1EmuDlg::InitEnvironment()
{
	g_pUserList = new CSearch(MAX_USER);
}

void CMy1p1EmuDlg::UserFree(int uid)
{

	USER* pUser = g_pUserList->GetUserUid(uid);
	if (pUser)
	{
		g_pUserList->FreeUser(uid);
	}
}

void CMy1p1EmuDlg::LogToFile(CString entry)
{
	CTime currentTime = CTime::GetCurrentTime();
	CString log;
	log.Format("[%d-%d-%d %d:%d:%d]%s\r\n", currentTime.GetYear(), currentTime.GetMonth(), currentTime.GetDay(), currentTime.GetHour(), currentTime.GetMinute(), currentTime.GetSecond(), entry);
	m_LogFile.Write((LPTSTR)(LPCTSTR)log, log.GetLength());
}

// ################################################################# //
// ############### NPC LOADING #################################### //
// ################################################################# //

bool CMy1p1EmuDlg::GetNpcTableData()
{
	CNpcTableSet NpcTableSet;

	try
	{
		if (g_arNpcTable.GetSize())
		{
			return FALSE;
		}

		if (NpcTableSet.IsOpen())
		{
			NpcTableSet.Close();
		}

		if (!NpcTableSet.Open())
		{
			AfxMessageBox(_T("MONSTER DB Open Fail!"));
			return FALSE;
		}

		if (NpcTableSet.IsBOF())
		{
			AfxMessageBox(_T("MONSTER DB Empty!"));
			return FALSE;
		}

		while (!NpcTableSet.IsEOF())
		{
			CNpcTable* Npc = new CNpcTable;

			Npc->m_sSid = atoi((LPCTSTR)NpcTableSet.m_sSid);
			Npc->m_sPid = atoi((LPCTSTR)NpcTableSet.m_sPid);
			Npc->m_tType = atoi((LPCTSTR)NpcTableSet.m_tType);
			Npc->m_sTypeAI = atoi((LPCTSTR)NpcTableSet.m_sTypeAI);
			Npc->m_sMinDamage = atoi((LPCTSTR)NpcTableSet.m_sMinDamage);
			Npc->m_sClass = atoi((LPCTSTR)NpcTableSet.m_sClass);
			_tcscpy(Npc->m_strName, NpcTableSet.m_strName);	// MONSTER(NPC) Name
			Npc->m_sBlood = atoi((LPCTSTR)NpcTableSet.m_sBlood);
			Npc->m_sLevel = atoi((LPCTSTR)NpcTableSet.m_sLevel);
			Npc->m_iMaxExp = atoi((LPCTSTR)NpcTableSet.m_iMaxExp);
			Npc->m_sStr = atoi((LPCTSTR)NpcTableSet.m_sStr);
			Npc->m_sWStr = atoi((LPCTSTR)NpcTableSet.m_sWStr);
			Npc->m_sAStr = atoi((LPCTSTR)NpcTableSet.m_sAStr);
			Npc->m_sDex_at = atoi((LPCTSTR)NpcTableSet.m_sDex_at);
			Npc->m_sDex_df = atoi((LPCTSTR)NpcTableSet.m_sDex_df);
			Npc->m_sIntel = atoi((LPCTSTR)NpcTableSet.m_sIntel);
			Npc->m_sCharm = atoi((LPCTSTR)NpcTableSet.m_sCharm);
			Npc->m_sWis = atoi((LPCTSTR)NpcTableSet.m_sWis);
			Npc->m_sCon = atoi((LPCTSTR)NpcTableSet.m_sCon);
			Npc->m_sMaxHp = atoi((LPCTSTR)NpcTableSet.m_sMaxHp);
			Npc->m_sMaxMp = atoi((LPCTSTR)NpcTableSet.m_sMaxMp);
			Npc->m_iStatus = atoi((LPCTSTR)NpcTableSet.m_iStatus);
			Npc->m_iMoral = atoi((LPCTSTR)NpcTableSet.m_iMoral);
			Npc->m_at_type = atoi((LPCTSTR)NpcTableSet.m_at_type);
			Npc->m_can_escape = atoi((LPCTSTR)NpcTableSet.m_can_escape);
			Npc->m_can_find_enemy = atoi((LPCTSTR)NpcTableSet.m_can_find_enemy);
			Npc->m_can_find_our = atoi((LPCTSTR)NpcTableSet.m_can_find_our);
			Npc->m_have_item_num = atoi((LPCTSTR)NpcTableSet.m_have_item_num);
			Npc->m_haved_item = atoi((LPCTSTR)NpcTableSet.m_haved_item);
			Npc->m_have_magic_num = atoi((LPCTSTR)NpcTableSet.m_have_magic_num);
			Npc->m_haved_magic = atoi((LPCTSTR)NpcTableSet.m_haved_magic);
			Npc->m_have_skill_num = atoi((LPCTSTR)NpcTableSet.m_have_skill_num);
			Npc->m_haved_skill = atoi((LPCTSTR)NpcTableSet.m_haved_skill);
			Npc->m_search_range = atoi((LPCTSTR)NpcTableSet.m_search_range);
			Npc->m_movable_range = atoi((LPCTSTR)NpcTableSet.m_movable_range);
			Npc->m_move_speed = atoi((LPCTSTR)NpcTableSet.m_move_speed);
			Npc->m_standing_time = atoi((LPCTSTR)NpcTableSet.m_standing_time);
			Npc->m_regen_time = atoi((LPCTSTR)NpcTableSet.m_regen_time);
			Npc->m_bmagicexp = atoi((LPCTSTR)NpcTableSet.m_bmagicexp);
			Npc->m_wmagicexp = atoi((LPCTSTR)NpcTableSet.m_wmagicexp);
			Npc->m_dmagicexp = atoi((LPCTSTR)NpcTableSet.m_dmagicexp);
			Npc->m_sRangeRate = atoi((LPCTSTR)NpcTableSet.m_sRangeRate);
			Npc->m_sBackRate = atoi((LPCTSTR)NpcTableSet.m_sBackRate);
			Npc->m_sHowTarget = atoi((LPCTSTR)NpcTableSet.m_sHowTarget);
			Npc->m_sMoneyRate = atoi((LPCTSTR)NpcTableSet.m_sMoneyRate);
			Npc->m_sMagicNum01 = atoi((LPCTSTR)NpcTableSet.m_sMagicNum01);
			Npc->m_sMagicRate01 = atoi((LPCTSTR)NpcTableSet.m_sMagicRate01);
			Npc->m_sMagicNum02 = atoi((LPCTSTR)NpcTableSet.m_sMagicNum02);
			Npc->m_sMagicRate02 = atoi((LPCTSTR)NpcTableSet.m_sMagicRate02);
			Npc->m_sMagicNum03 = atoi((LPCTSTR)NpcTableSet.m_sMagicNum03);
			Npc->m_sMagicRate03 = atoi((LPCTSTR)NpcTableSet.m_sMagicRate03);
			Npc->m_sDistance = atoi((LPCTSTR)NpcTableSet.m_sDistance);

			g_arNpcTable.Add(Npc);

			NpcTableSet.MoveNext();
		}

		NpcTableSet.Close();
	}
	catch (CMemoryException * e)
	{
		e->ReportError();
		e->Delete();

		return FALSE;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();

		return FALSE;
	}

	return TRUE;
}

bool CMy1p1EmuDlg::CreateNpcThread()
{
	BOOL	bMoveNext	= TRUE;
	int		nSerial		= 0;
	int		nNpcCount	= 0;
	int		i			= 0;

	g_TotalNPC = 0;

	CNpcPosSet	NpcPosSet;
	CNpcTable*	pNpcTable = NULL;

	g_arNpc.RemoveAll();

	try
	{
		if (NpcPosSet.IsOpen())
		{
			NpcPosSet.Close();
		}

		if (!NpcPosSet.Open())
		{
			AfxMessageBox(_T("MONSTER_POS DB Open Fail!"));
			return FALSE;
		}

		if (NpcPosSet.IsBOF())
		{
			AfxMessageBox(_T("MONSTER_POS DB Empty!"));
			return FALSE;
		}

		while (!NpcPosSet.IsEOF())
		{
			CNpc* pNpc = new CNpc;

			pNpc->m_sNid = nSerial++;
			pNpc->m_sSid = atoi((LPCTSTR)NpcPosSet.m_sTableNum); // MONSTER(NPC) Serial ID
			pNpc->m_sMid = atoi((LPCTSTR)NpcPosSet.m_sSid);	// MONSTERSET Serial ID

			pNpcTable = NULL;
			for (i = 0; i < g_arNpcTable.GetSize(); i++)
			{
				if (pNpc->m_sSid == g_arNpcTable[i]->m_sSid)
				{
					pNpcTable = g_arNpcTable[i];
					break;
				}
			}

			if (!pNpcTable)
			{
				if (pNpc)
				{
					delete pNpc;
				}

				CString str;
				str.Format("Fail no NPC(monsterset => %d)", atoi((LPCTSTR)NpcPosSet.m_sTableNum));
				AfxMessageBox(str);
				return FALSE;
			}

			if (bMoveNext)
			{
				bMoveNext = FALSE;
				nNpcCount = atoi((LPCTSTR)NpcPosSet.m_sNum);
			}

			pNpc->m_sPid = pNpcTable->m_sPid;
			pNpc->m_tNpcType = pNpcTable->m_tType;
			pNpc->m_sTypeAI = pNpcTable->m_sTypeAI;
			pNpc->m_sMinDamage = pNpcTable->m_sMinDamage;
			pNpc->m_sClass = pNpcTable->m_sClass;
			memset(pNpc->m_strName, 0, sizeof(pNpc->m_strName));
			if (pNpcTable->m_strName[0] !=  '!')
			{
				_tcscpy(pNpc->m_strName, pNpcTable->m_strName);	// MONSTER(NPC) Name
			}
			pNpc->m_sBlood = pNpcTable->m_sBlood;
			pNpc->m_sLevel = pNpcTable->m_sLevel;
			pNpc->m_iMaxExp = pNpcTable->m_iMaxExp;
			pNpc->m_sStr = pNpcTable->m_sStr;
			pNpc->m_sWStr = pNpcTable->m_sWStr;
			pNpc->m_sAStr = pNpcTable->m_sAStr;
			pNpc->m_sDex_at = pNpcTable->m_sDex_at;
			pNpc->m_sDex_df = pNpcTable->m_sDex_df;
			pNpc->m_sIntel = pNpcTable->m_sIntel;
			pNpc->m_sCharm = pNpcTable->m_sCharm;
			pNpc->m_sWis = pNpcTable->m_sWis;
			pNpc->m_sCon = pNpcTable->m_sCon;
			pNpc->m_sHP = pNpc->m_sMaxHp = pNpcTable->m_sMaxHp;
			pNpc->m_sMP = pNpc->m_sMaxMp = pNpcTable->m_sMaxMp;
			pNpc->m_iStatus = pNpcTable->m_iStatus;
			pNpc->m_iMoral = pNpcTable->m_iMoral;
			pNpc->m_tNpcAttType = pNpcTable->m_at_type;
			pNpc->m_can_escape = pNpcTable->m_can_escape;
			pNpc->m_can_find_enemy = pNpcTable->m_can_find_enemy;
			pNpc->m_can_find_our = pNpcTable->m_can_find_our;
			pNpc->m_have_item_num = pNpcTable->m_have_item_num;
			pNpc->m_haved_item = pNpcTable->m_haved_item;
			pNpc->m_have_magic_num = pNpcTable->m_have_magic_num;
			pNpc->m_haved_magic = pNpcTable->m_haved_magic;
			pNpc->m_have_skill_num = pNpcTable->m_have_skill_num;
			pNpc->m_haved_skill = pNpcTable->m_haved_skill;
			if (pNpcTable->m_search_range > 15)
			{
				pNpc->m_search_range = 15;
			}
			else
			{
				pNpc->m_search_range = pNpcTable->m_search_range;
			}
			pNpc->m_movable_range = pNpcTable->m_movable_range;
			pNpc->m_sSpeed = pNpcTable->m_move_speed;
			pNpc->m_standing_time = pNpcTable->m_standing_time;
			pNpc->m_regen_time = pNpcTable->m_regen_time;
			pNpc->m_bmagicexp = pNpcTable->m_bmagicexp;
			pNpc->m_wmagicexp = pNpcTable->m_wmagicexp;
			pNpc->m_dmagicexp = pNpcTable->m_dmagicexp;
			pNpc->m_sRangeRate = pNpcTable->m_sRangeRate;
			pNpc->m_sBackRate = pNpcTable->m_sBackRate;
			pNpc->m_sHowTarget = pNpcTable->m_sHowTarget;
			pNpc->m_sMoneyRate = pNpcTable->m_sMoneyRate;
			pNpc->m_sMagicNum[0] = pNpcTable->m_sMagicNum01;
			pNpc->m_sMagicRate[0] = pNpcTable->m_sMagicRate01;
			pNpc->m_sMagicNum[1] = pNpcTable->m_sMagicNum02;
			pNpc->m_sMagicRate[1] = pNpcTable->m_sMagicRate02;
			pNpc->m_sMagicNum[2] = pNpcTable->m_sMagicNum03;
			pNpc->m_sMagicRate[2] = pNpcTable->m_sMagicRate03;
			pNpc->m_sDistance = pNpcTable->m_sDistance;


			//////// MONSTER POS ////////////////////////////////////////
			pNpc->m_sCurZ = pNpc->m_sZone = atoi((LPCTSTR)NpcPosSet.m_sZone);
			pNpc->m_sMinX = atoi((LPCTSTR)NpcPosSet.m_sMinX);
			pNpc->m_sMinY = atoi((LPCTSTR)NpcPosSet.m_sMinY);
			pNpc->m_sMaxX = atoi((LPCTSTR)NpcPosSet.m_sMaxX);
			pNpc->m_sMaxY = atoi((LPCTSTR)NpcPosSet.m_sMaxY);
			pNpc->m_sNum = atoi((LPCTSTR)NpcPosSet.m_sNum);
			pNpc->m_sTableNum = atoi((LPCTSTR)NpcPosSet.m_sTableNum);
			pNpc->m_sRegenType = atoi((LPCTSTR)NpcPosSet.m_sRegenType);
			pNpc->m_sRegenTime = atoi((LPCTSTR)NpcPosSet.m_sRegenTime);
			pNpc->m_sRegenTime = pNpc->m_sRegenTime * 1000;
			pNpc->m_sRegenEvent = atoi((LPCTSTR)NpcPosSet.m_sRegenEvent);
			pNpc->m_sGroup = atoi((LPCTSTR)NpcPosSet.m_sGroup);
			pNpc->m_sGuild = atoi((LPCTSTR)NpcPosSet.m_sGuild);
			pNpc->m_sGuildOpt = atoi((LPCTSTR)NpcPosSet.m_sGuildOpt);

			// NPC Regen different types currently NoLive (ForcedSpawn) and Time (GameTimedSpawn)
			switch (pNpc->m_sRegenType)
			{
			case NPCREGENTYPE_FORCE:
				{
					NPC_TYPE_NOLIVE* NoLive = new NPC_TYPE_NOLIVE;
					NoLive->sNId = pNpc->m_sNid;
					NoLive->sMId = pNpc->m_sMid;
					NoLive->sGroup = pNpc->m_sGroup;
					g_arNpcTypeNoLive.Add(NoLive);
				}
				break;
			case NPCREGENTYPE_TIME:
				{
					NPC_TYPE_TIME* Time = new NPC_TYPE_TIME;
					Time->sNId = pNpc->m_sNid;
					Time->sMId = pNpc->m_sMid;
					Time->sRegenTime = pNpc->m_sRegenTime;
					g_arNpcTypeTime.Add(Time);
				}
				break;
			default:
				break;
			}

			// NPC for wotw maps
			if (pNpc->m_sMid >= NPC_CLASS_BAND_HUMAN && pNpc->m_sMid < 30000)
			{
				NPC_TYPE_CLASS* Class = new NPC_TYPE_CLASS;
				Class->sNId = pNpc->m_sNid;
				Class->sMId = pNpc->m_sMid;
				if (pNpc->m_sMid >= NPC_CLASS_BAND_DEVIL)
				{
					Class->sClass = 10;
				}
				else
				{
					Class->sClass = 0;
				}
				g_arNpcTypeClass.Add(Class);
			}

			// NPC with a guild
			if (pNpc->m_sGuild != -1)
			{
				NPC_TYPE_GUILD* Guild = new NPC_TYPE_GUILD;
				Guild->sNId = pNpc->m_sNid;
				Guild->sMId = pNpc->m_sMid;
				Guild->sGuild = pNpc->m_sGuild;
				Guild->sGuildOpt = pNpc->m_sGuildOpt;
				g_arNpcTypeGuild.Add(Guild);
			}

			pNpc->m_sChat[0] = atoi((LPCTSTR)NpcPosSet.m_sChat01);
			pNpc->m_sChat[1] = atoi((LPCTSTR)NpcPosSet.m_sChat02);
			pNpc->m_sChat[2] = atoi((LPCTSTR)NpcPosSet.m_sChat03);
			pNpc->m_sMaxItemNum = atoi((LPCTSTR)NpcPosSet.m_sMaxItemNum);
			int i = 0;
			pNpc->m_sarrItem[i] = atoi((LPCTSTR)NpcPosSet.m_sItem01);
			pNpc->m_sarrItemRand[i++] = atoi((LPCTSTR)NpcPosSet.m_sItem01Rand);
			pNpc->m_sarrItem[i] = atoi((LPCTSTR)NpcPosSet.m_sItem02);
			pNpc->m_sarrItemRand[i++] = atoi((LPCTSTR)NpcPosSet.m_sItem02Rand);
			pNpc->m_sarrItem[i] = atoi((LPCTSTR)NpcPosSet.m_sItem03);
			pNpc->m_sarrItemRand[i++] = atoi((LPCTSTR)NpcPosSet.m_sItem03Rand);
			pNpc->m_sarrItem[i] = atoi((LPCTSTR)NpcPosSet.m_sItem04);
			pNpc->m_sarrItemRand[i++] = atoi((LPCTSTR)NpcPosSet.m_sItem04Rand);
			pNpc->m_sarrItem[i] = atoi((LPCTSTR)NpcPosSet.m_sItem05);
			pNpc->m_sarrItemRand[i++] = atoi((LPCTSTR)NpcPosSet.m_sItem05Rand);
			pNpc->m_sarrItem[i] = atoi((LPCTSTR)NpcPosSet.m_sItem06);
			pNpc->m_sarrItemRand[i++] = atoi((LPCTSTR)NpcPosSet.m_sItem06Rand);
			pNpc->m_sarrItem[i] = atoi((LPCTSTR)NpcPosSet.m_sItem07);
			pNpc->m_sarrItemRand[i++] = atoi((LPCTSTR)NpcPosSet.m_sItem07Rand);
			pNpc->m_sarrItem[i] = atoi((LPCTSTR)NpcPosSet.m_sItem08);
			pNpc->m_sarrItemRand[i++] = atoi((LPCTSTR)NpcPosSet.m_sItem08Rand);
			pNpc->m_sarrItem[i] = atoi((LPCTSTR)NpcPosSet.m_sItem09);
			pNpc->m_sarrItemRand[i++] = atoi((LPCTSTR)NpcPosSet.m_sItem09Rand);
			pNpc->m_sarrItem[i] = atoi((LPCTSTR)NpcPosSet.m_sItem10);
			pNpc->m_sarrItemRand[i++] = atoi((LPCTSTR)NpcPosSet.m_sItem10Rand);
			pNpc->m_sarrItem[i] = atoi((LPCTSTR)NpcPosSet.m_sItem11);
			pNpc->m_sarrItemRand[i++] = atoi((LPCTSTR)NpcPosSet.m_sItem11Rand);
			pNpc->m_sarrItem[i] = atoi((LPCTSTR)NpcPosSet.m_sItem12);
			pNpc->m_sarrItemRand[i++] = atoi((LPCTSTR)NpcPosSet.m_sItem12Rand);
			pNpc->m_sarrItem[i] = atoi((LPCTSTR)NpcPosSet.m_sItem13);
			pNpc->m_sarrItemRand[i++] = atoi((LPCTSTR)NpcPosSet.m_sItem13Rand);
			pNpc->m_sarrItem[i] = atoi((LPCTSTR)NpcPosSet.m_sItem14);
			pNpc->m_sarrItemRand[i++] = atoi((LPCTSTR)NpcPosSet.m_sItem14Rand);
			pNpc->m_sarrItem[i] = atoi((LPCTSTR)NpcPosSet.m_sItem15);
			pNpc->m_sarrItemRand[i++] = atoi((LPCTSTR)NpcPosSet.m_sItem15Rand);
			pNpc->m_sEventNum = atoi((LPCTSTR)NpcPosSet.m_sEventNum);
			pNpc->m_sMoneyType = atoi((LPCTSTR)NpcPosSet.m_sMoneyType);
			pNpc->m_sMoneyMin = atoi((LPCTSTR)NpcPosSet.m_sMoneyMin);
			pNpc->m_sMoneyMax = atoi((LPCTSTR)NpcPosSet.m_sMoneyMax);

			pNpc->m_sCurX = pNpc->m_sMinX;
			pNpc->m_sCurY = pNpc->m_sMinY;

			pNpc->m_sClientSpeed = 100 - (pNpc->m_sSpeed - 500) * 10/50;
			pNpc->m_dwStepDelay = GetTickCount();
			pNpc->m_ZoneIndex		= -1;

			if (pNpc->m_sClientSpeed <= 20)
			{
				pNpc->m_sClientSpeed = 20;
			}

			for (i = 0; i < g_Zones.GetSize(); i++)
			{
				MAP* pAddMap = g_Zones[i];
				if (g_Zones[i]->m_Zone == pNpc->m_sCurZ)
				{
					pNpc->m_ZoneIndex = pNpc->m_TableZoneIndex = i;
					break;
				}
			}

			if (pNpc->m_ZoneIndex == -1)
			{
				if (pNpc)
				{
					delete pNpc;
				}

				AfxMessageBox("Invalid NPC zone Index!!");
				return FALSE;
			}

			if (pNpc->IsDetecter())
			{
				g_arNpcTypeDetecter.Add(pNpc->m_sNid);
			}

			if (!pNpc->Init())
			{
				if (pNpc)
				{
					delete pNpc;
				}

				CString str;
				str.Format("Unable to initialize NPC(monsterset => %d)!", atoi((LPCTSTR)NpcPosSet.m_sSid));
				AfxMessageBox(str);
				return FALSE;
			}

			g_arNpc.Add(pNpc);

			g_TotalNPC = nSerial;
			if (--nNpcCount > 0)
			{
				continue;
			}

			bMoveNext = TRUE;
			nNpcCount = 0;

			NpcPosSet.MoveNext();
		}
	}
	catch (CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}

	TRACE("LOADED %d NPCS\n", g_TotalNPC);
	int step = 0;
	CNpcThread* pNpcThread = NULL;
	for (i = 0; i < g_arNpc.GetSize(); i++)
	{
		if (step == 0)
		{
			pNpcThread = NULL;
			pNpcThread = new CNpcThread;
		}
		pNpcThread->m_pNpc[step] = g_arNpc[i];

		++step;

		if (step == NPC_NUM)
		{
			pNpcThread->m_pCom = &m_Com;
			pNpcThread->m_pThread = AfxBeginThread(NpcThreadProc, &(pNpcThread->m_ThreadInfo), THREAD_PRIORITY_BELOW_NORMAL, 0, CREATE_SUSPENDED);
			g_arNpcThread.Add(pNpcThread);
			step = 0;
		}
	}

	if (step != 0)
	{
		pNpcThread->m_pCom = &m_Com;
		pNpcThread->m_pThread = AfxBeginThread(NpcThreadProc, &(pNpcThread->m_ThreadInfo), THREAD_PRIORITY_BELOW_NORMAL, 0, CREATE_SUSPENDED);
		g_arNpcThread.Add(pNpcThread);
	}

	return TRUE;
}

void CMy1p1EmuDlg::ResumeAI()
{
	for (int i = 0; i < g_arNpcThread.GetSize(); i++)
	{
		g_arNpcThread[i]->m_ThreadInfo.hWndMsg = this->GetSafeHwnd();

		for (int j = 0; j < NPC_NUM; j++)
		{
			g_arNpcThread[i]->m_ThreadInfo.pNpc[j] = g_arNpcThread[i]->m_pNpc[j];
		}

		g_arNpcThread[i]->m_ThreadInfo.pCom = &m_Com;

		::ResumeThread(g_arNpcThread[i]->m_pThread->m_hThread);
	}
}

// ################################################################# //
// ############### ITEM LOADING #################################### //
// ################################################################# //

bool CMy1p1EmuDlg::GetItemTable()
{
	CItemTableSet ItemTableSet;

	try
	{
		if (ItemTableSet.IsOpen())
		{
			ItemTableSet.Close();
		}

		ItemTableSet.m_strSort = "sNum ASC";

		if (!ItemTableSet.Open())
		{
			AfxMessageBox(_T("Item DB Open Fail!"));
			return false;
		}

		if (ItemTableSet.IsBOF())
		{
			AfxMessageBox(_T("Item DB Empty!"));
			return false;
		}

		while (!ItemTableSet.IsEOF())
		{
			CItemTable* pNewItem = new CItemTable;

			pNewItem->m_sNum = ItemTableSet.m_sNum;
			pNewItem->m_sPicNum1 = ItemTableSet.m_sPicNum1;
			pNewItem->m_sPicNum2 = ItemTableSet.m_sPicNum2;
			pNewItem->m_sPicNum3 = ItemTableSet.m_sPicNum3;
			pNewItem->m_sPicNum4 = ItemTableSet.m_sPicNum4;
			pNewItem->m_strName	= ItemTableSet.m_strName;
			pNewItem->m_bType = ItemTableSet.m_bType;
			pNewItem->m_bArm = ItemTableSet.m_bArm;
			pNewItem->m_bGender	= ItemTableSet.m_bGender;
			pNewItem->m_sAb1 = ItemTableSet.m_sAb1;
			pNewItem->m_sAb2 = ItemTableSet.m_sAb2;
			pNewItem->m_sWgt = ItemTableSet.m_sWgt;
			pNewItem->m_iCost = ItemTableSet.m_iCost;
			pNewItem->m_sDur = ItemTableSet.m_sDur;
			pNewItem->m_sNeedStr = ItemTableSet.m_sNeedStr;
			pNewItem->m_sNeedInt = ItemTableSet.m_sNeedInt;
			pNewItem->m_sMinExp	= ItemTableSet.m_sMinExp;
			pNewItem->m_sStr = ItemTableSet.m_sStr;
			pNewItem->m_sDex = ItemTableSet.m_sDex;
			pNewItem->m_sInt = ItemTableSet.m_sInt;
			pNewItem->m_sAt = ItemTableSet.m_sAt;
			pNewItem->m_sDf = ItemTableSet.m_sDf;
			pNewItem->m_sHP = ItemTableSet.m_sHP;
			pNewItem->m_sMP	= ItemTableSet.m_sMP;
			pNewItem->m_sMagicNo = ItemTableSet.m_sMagicNo;
			pNewItem->m_sMagicOp = ItemTableSet.m_sMagicOpt;
			pNewItem->m_sTime = ItemTableSet.m_sTime;
			pNewItem->m_sSpecial = ItemTableSet.m_sSpecial;
			pNewItem->m_sSpOpt[0] = ItemTableSet.m_sSpOpt1;
			pNewItem->m_sSpOpt[1] = ItemTableSet.m_sSpOpt2;
			pNewItem->m_sSpOpt[2] = ItemTableSet.m_sSpOpt3;

			g_mapItemTable[pNewItem->m_sNum] = pNewItem;

			ItemTableSet.MoveNext();
		}
	}
	catch (CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetPlusSpecialTable()
{
	CPlusSpecialTableSet PlusSpecialTableSet;

	try
	{
		if (PlusSpecialTableSet.IsOpen())
		{
			PlusSpecialTableSet.Close();
		}

		if (!PlusSpecialTableSet.Open())
		{
			AfxMessageBox(_T("PlusSpecial DB Open Fail!"));
			return false;
		}

		if (PlusSpecialTableSet.IsBOF())
		{
			AfxMessageBox(_T("PlusSpecial DB Empty!"));
			return false;
		}

		while (!PlusSpecialTableSet.IsEOF())
		{
			CPlusSpecialTable* pNewPlusSpecial = new CPlusSpecialTable;

			pNewPlusSpecial->m_sId = atoi((LPCTSTR)PlusSpecialTableSet.m_sId);
			pNewPlusSpecial->m_sSpecial = atoi((LPCTSTR)PlusSpecialTableSet.m_sSpecial);
			pNewPlusSpecial->m_sSpOpt[0] = atoi((LPCTSTR)PlusSpecialTableSet.m_sSpOpt1);
			pNewPlusSpecial->m_sSpOpt[1] = atoi((LPCTSTR)PlusSpecialTableSet.m_sSpOpt2);
			pNewPlusSpecial->m_sSpOpt[2] = atoi((LPCTSTR)PlusSpecialTableSet.m_sSpOpt3);

			g_arPlusSpecialTable.Add(pNewPlusSpecial);
			PlusSpecialTableSet.MoveNext();
		}
	}
	catch (CMemoryException * e)
	{
		e->ReportError();
		e->Delete();

		return false;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();

		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetMagicTable()
{
	CMagicTableSet MagicTableSet;

	try
	{
		if (MagicTableSet.IsOpen())
		{
			MagicTableSet.Close();
		}

		if (!MagicTableSet.Open())
		{
			AfxMessageBox(_T("Magic DB Open Fail!"));
			return false;
		}

		if (!MagicTableSet.Requery())
		{
			AfxMessageBox(_T("Magic DB Requery Fail!"));
			return false;
		}

		if (MagicTableSet.IsBOF())
		{
			AfxMessageBox(_T("Magic DB Empty!"));
			return false;
		}

		while (!MagicTableSet.IsEOF())
		{
			CMagicTable* pNewMagic = new CMagicTable;

			pNewMagic->m_sMid = atoi((LPCTSTR)MagicTableSet.m_sMid);
			pNewMagic->m_tClass = atoi((LPCTSTR)MagicTableSet.m_tClass);
			pNewMagic->m_strName = MagicTableSet.m_strName;
			pNewMagic->m_tType01 = atoi((LPCTSTR)MagicTableSet.m_tType01);
			pNewMagic->m_tType02 = atoi((LPCTSTR)MagicTableSet.m_tType02);
			pNewMagic->m_sStartTime = atoi((LPCTSTR)MagicTableSet.m_sStartTime);
			pNewMagic->m_tTarget = atoi((LPCTSTR)MagicTableSet.m_tTarget);
			pNewMagic->m_sMpdec = atoi((LPCTSTR)MagicTableSet.m_sMpdec);
			pNewMagic->m_tEValue = atoi((LPCTSTR)MagicTableSet.m_tEValue);
			pNewMagic->m_sEDist = atoi((LPCTSTR)MagicTableSet.m_sEDist);
			pNewMagic->m_sRange = atoi((LPCTSTR)MagicTableSet.m_sRange);
			pNewMagic->m_sDamage = atoi((LPCTSTR)MagicTableSet.m_sDamage);
			pNewMagic->m_iTime = atoi((LPCTSTR)MagicTableSet.m_iTime);
			pNewMagic->m_iTerm = atoi((LPCTSTR)MagicTableSet.m_iTerm);
			pNewMagic->m_sMinLevel = atoi((LPCTSTR)MagicTableSet.m_sMinLevel);
			pNewMagic->m_sMinMxp = atoi((LPCTSTR)MagicTableSet.m_sMinMxp);
			pNewMagic->m_sMinInt = atoi((LPCTSTR)MagicTableSet.m_sMinInt);
			pNewMagic->m_sMinMoral = atoi((LPCTSTR)MagicTableSet.m_sMinMoral);
			pNewMagic->m_sMaxMoral = atoi((LPCTSTR)MagicTableSet.m_sMaxMoral);
			pNewMagic->m_strInform = MagicTableSet.m_strInform;
			pNewMagic->m_strGetInfo = MagicTableSet.m_strGetInfo;
			pNewMagic->m_sRate = atoi((LPCTSTR)MagicTableSet.m_sRate);
			pNewMagic->m_sPlusExpRate = atoi((LPCTSTR)MagicTableSet.m_sPlusExpRate);
			pNewMagic->m_sPlusType = atoi((LPCTSTR)MagicTableSet.m_sPlusType);
			g_arMagicTable.Add(pNewMagic);

			MagicTableSet.MoveNext();
		}
	}
	catch(CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	catch(CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetSpecialAttackTable()
{
	CSpecialTableSet SpecialTableSet;

	try
	{
		if (SpecialTableSet.IsOpen())
		{
			SpecialTableSet.Close();
		}

		if (!SpecialTableSet.Open())
		{
			AfxMessageBox(_T("Special DB Open Fail!"));
			return false;
		}

		if (SpecialTableSet.IsBOF())
		{
			AfxMessageBox(_T("Special DB Empty!"));
			return false;
		}

		while (!SpecialTableSet.IsEOF())
		{
			CSpecialTable* pNewSpecial = new CSpecialTable;

			pNewSpecial->m_sMid = atoi((LPCTSTR)SpecialTableSet.m_sMid);
			pNewSpecial->m_tClass = atoi((LPCTSTR)SpecialTableSet.m_tClass);
			pNewSpecial->m_strName = SpecialTableSet.m_strName;
			pNewSpecial->m_tType = atoi((LPCTSTR)SpecialTableSet.m_tType);
			pNewSpecial->m_tTarget = atoi((LPCTSTR)SpecialTableSet.m_tTarget);
			pNewSpecial->m_sHpdec = atoi((LPCTSTR)SpecialTableSet.m_sHpdec);
			pNewSpecial->m_sMpdec = atoi((LPCTSTR)SpecialTableSet.m_sMpdec);
			pNewSpecial->m_sStmdec = atoi((LPCTSTR)SpecialTableSet.m_sStmdec);
			pNewSpecial->m_tEValue = atoi((LPCTSTR)SpecialTableSet.m_tEValue);
			pNewSpecial->m_sEDist = atoi((LPCTSTR)SpecialTableSet.m_sEDist);
			pNewSpecial->m_sDamage = atoi((LPCTSTR)SpecialTableSet.m_sDamage);
			pNewSpecial->m_iTime = atoi((LPCTSTR)SpecialTableSet.m_iTime);
			pNewSpecial->m_iTerm = atoi((LPCTSTR)SpecialTableSet.m_iTerm);
			pNewSpecial->m_iDecTerm = atoi((LPCTSTR)SpecialTableSet.m_iDecTerm);
			pNewSpecial->m_sTHp = atoi((LPCTSTR)SpecialTableSet.m_sTHp);
			pNewSpecial->m_sTMp = atoi((LPCTSTR)SpecialTableSet.m_sTMp);
			pNewSpecial->m_sMinExp = atoi((LPCTSTR)SpecialTableSet.m_sMinExp);
			pNewSpecial->m_strInform = SpecialTableSet.m_strInform;
			pNewSpecial->m_strGetInfo = SpecialTableSet.m_strGetInfo;
			pNewSpecial->m_sRate = atoi((LPCTSTR)SpecialTableSet.m_sRate);
			pNewSpecial->m_sPlusExpRate = atoi((LPCTSTR)SpecialTableSet.m_sPlusExpRate);
			pNewSpecial->m_sPlusType = atoi((LPCTSTR)SpecialTableSet.m_sPlusType);

			g_arSpecialAttackTable.Add(pNewSpecial);
			SpecialTableSet.MoveNext();
		}
	}
	catch (CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetMakeSkillTable()
{
	CMakeSkillTableSet MakeSkillTableSet;

	try
	{
		if (MakeSkillTableSet.IsOpen())
		{
			MakeSkillTableSet.Close();
		}

		if (!MakeSkillTableSet.Open())
		{
			AfxMessageBox(_T("MakeSkill DB Open Fail!"));
			return false;
		}

		if (MakeSkillTableSet.IsBOF())
		{
			AfxMessageBox(_T("MakeSkill DB Empty!"));
			return false;
		}

		while (!MakeSkillTableSet.IsEOF())
		{
			CMakeSkillTable* pNewMakeSkill = new CMakeSkillTable;

			pNewMakeSkill->m_sId = MakeSkillTableSet.m_sId;
			pNewMakeSkill->m_sType = MakeSkillTableSet.m_sType;
			pNewMakeSkill->m_strName = MakeSkillTableSet.m_strName;
			pNewMakeSkill->m_sMinExp = MakeSkillTableSet.m_sMinExp;
			pNewMakeSkill->m_strInform = MakeSkillTableSet.m_strInform;
			pNewMakeSkill->m_strGetInfo = MakeSkillTableSet.m_strGetInfo;
			g_arMakeSkillTable.Add(pNewMakeSkill);
			MakeSkillTableSet.MoveNext();
		}
	}
	catch (CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetNpcChatTable()
{
	CNpcChatSet	ChatNpcSet;

	g_arNpcChat.RemoveAll();

	try
	{
		if (ChatNpcSet.IsOpen())
		{
			ChatNpcSet.Close();
		}

		if (!ChatNpcSet.Open())
		{
			AfxMessageBox(_T("NpcChat DB Open Fail!"));
			return false;
		}

		if (ChatNpcSet.IsBOF())
		{
			AfxMessageBox(_T("NpcChat DB Empty!"));
			return false;
		}

		while (!ChatNpcSet.IsEOF())
		{
			CNpcChat* pNewChat = new CNpcChat;

			pNewChat->m_sCid = atoi((LPCTSTR)ChatNpcSet.m_sCid);
			pNewChat->m_strTalk = ChatNpcSet.m_strTalk;
			g_arNpcChat.Add(pNewChat);
			ChatNpcSet.MoveNext();
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetClassStoreTable()
{
	CClassStoreTableSet	ClassStoreTableSet;

	try
	{
		if (ClassStoreTableSet.IsOpen())
		{
			ClassStoreTableSet.Close();
		}

		if (!ClassStoreTableSet.Open())
		{
			AfxMessageBox(_T("ClassStore DB Open Fail!"));
			return false;
		}

		if (ClassStoreTableSet.IsBOF())
		{
			AfxMessageBox(_T("ClassStore DB Empty!"));
			return false;
		}

		while (!ClassStoreTableSet.IsEOF())
		{
			CClassStoreTable* pNewClassStore = new CClassStoreTable;
			pNewClassStore->m_sId = atoi((LPCTSTR)ClassStoreTableSet.m_sId);
			pNewClassStore->m_sType = atoi((LPCTSTR)ClassStoreTableSet.m_sType);
			pNewClassStore->m_sItemNum = atoi((LPCTSTR)ClassStoreTableSet.m_sItemNum);
			pNewClassStore->m_strName = ClassStoreTableSet.m_strName;
			pNewClassStore->m_sNum = atoi((LPCTSTR)ClassStoreTableSet.m_sNum);
			pNewClassStore->m_iPoint = atoi((LPCTSTR)ClassStoreTableSet.m_iPoint);

			g_arClassStoreTable.Add(pNewClassStore);
			ClassStoreTableSet.MoveNext();
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetStoreTable()
{
	CStoreSet StoreSet;

	try
	{
		StoreSet.m_strSort = _T("sId");

		if (StoreSet.IsOpen())
		{
			StoreSet.Close();
		}

		if (!StoreSet.Open())
		{
			AfxMessageBox(_T("Store DB Open Fail!"));
			return false;
		}

		if (StoreSet.IsBOF())
		{
			AfxMessageBox(_T("Store DB Empty!"));
			return false;
		}

		while (!StoreSet.IsEOF())
		{
			CStore*	pNewStore = new CStore;
			pNewStore->m_sStoreID = StoreSet.m_sStoreID;
			pNewStore->m_tStoreType = StoreSet.m_sType;

			int index = 0;
			char* pItems = (char*)StoreSet.m_Items.GetData();
			for (int j = 0; j < StoreSet.m_sItemNum; j++)
			{
				short sItemNo = GetShort(pItems, index);
				short sQuantity = GetShort(pItems, index);
				short sRegen = GetShort(pItems, index);
				short sRegenRate = GetShort(pItems, index);
				short sMRegen = GetShort(pItems, index);
				short sMRegenRate = GetShort(pItems, index);

				StoreItem* s = new StoreItem;
				s->sItemNo = sItemNo;
				if (sQuantity != -1)
				{
					s->sQuantity = 0;
				}
				else
				{
					s->sQuantity = sQuantity;
				}
				s->sOriginalQuantity = sQuantity;
				s->sRegen = sRegen;
				s->sRegenRate = sRegenRate;
				s->sOriginalRegen = sRegen;
				s->sMRegen = sMRegen;
				s->sMRegenRate = sMRegenRate;
				s->sOriginalMRegen = sMRegen;
                pNewStore->m_arItems.Add(s);
			}
			g_arStore.Add(pNewStore);
			StoreSet.MoveNext();
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetTownPortal()
{
	CTownPortal TownPortalSet;

	try
	{
		if (TownPortalSet.IsOpen())
		{
			TownPortalSet.Close();
		}

		if (!TownPortalSet.Open())
		{
			AfxMessageBox(_T("Town Portal DB Open Fail!"));
			return false;
		}

		if (TownPortalSet.IsBOF())
		{
			AfxMessageBox(_T("Town Portal DB Empty!"));
			return false;
		}

		while (!TownPortalSet.IsEOF())
		{
			TOWNPORTAL *pPortal= new TOWNPORTAL;

			pPortal->nItemNum = TownPortalSet.m_sitemnum;
			pPortal->nMoveZone = TownPortalSet.m_smovezone;
			pPortal->nX = TownPortalSet.m_sx;
			pPortal->nY = TownPortalSet.m_sy;

			g_arTownPortal.Add(pPortal);

			TownPortalSet.MoveNext();
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetTownPortalFixed()
{
	CTownPortalFixed TownPortalFixedSet;

	try
	{
		if (TownPortalFixedSet.IsOpen())
		{
			TownPortalFixedSet.Close();
		}

		if (!TownPortalFixedSet.Open())
		{
			AfxMessageBox(_T("Town Portal Fixed DB Open Fail!"));
			return false;
		}

		if (TownPortalFixedSet.IsBOF())
		{
			AfxMessageBox(_T("Town Portal Fixed DB Empty!"));
			return false;
		}

		while (!TownPortalFixedSet.IsEOF())
		{
			TOWNPORTALFIXED *pPortalFixed = new TOWNPORTALFIXED;

			pPortalFixed->nZone = TownPortalFixedSet.m_szone;
			pPortalFixed->nMoveZone = TownPortalFixedSet.m_smovezone;
			pPortalFixed->nX = TownPortalFixedSet.m_sx;
			pPortalFixed->nY = TownPortalFixedSet.m_sy;

			g_arTownPortalFixed.Add(pPortalFixed);

			TownPortalFixedSet.MoveNext();
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetChangeOtherItem()
{
	CChangeOtherItemTableSet ChangeOtherItemSet;

	try
	{
		if (ChangeOtherItemSet.IsOpen())
		{
			ChangeOtherItemSet.Close();
		}

		if (!ChangeOtherItemSet.Open())
		{
			AfxMessageBox(_T("Change Other Item DB Open Fail!"));
			return false;
		}

		if (ChangeOtherItemSet.IsBOF())
		{
			AfxMessageBox(_T("Change Other Item DB Empty!"));
			return false;
		}

		while (!ChangeOtherItemSet.IsEOF())
		{
			CChangeOtherItemTable* pChangeOtherItem = new CChangeOtherItemTable;

			pChangeOtherItem->m_sId = ChangeOtherItemSet.m_sId;
			pChangeOtherItem->m_sSpecialNum = ChangeOtherItemSet.m_sSpecialNum;
			pChangeOtherItem->m_sItemNum = ChangeOtherItemSet.m_sItemNum;
			pChangeOtherItem->m_strItemName = ChangeOtherItemSet.m_strItemName;

			g_arChangeOtherItemTable.Add(pChangeOtherItem);
			ChangeOtherItemSet.MoveNext();
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete
		return false;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetTraderExchangeTable()
{
	CTraderExchangeTableSet TraderExchangeSet;

	try
	{
		if (TraderExchangeSet.IsOpen())
		{
			TraderExchangeSet.Close();
		}

		if (!TraderExchangeSet.Open())
		{
			AfxMessageBox(_T("Trader Exchange DB Open Fail!"));
			return false;
		}

		if (TraderExchangeSet.IsBOF())
		{
			AfxMessageBox(_T("Change Other Item DB Empty!"));
			return false;
		}

		while (!TraderExchangeSet.IsEOF())
		{
			CTraderExchangeTable* pTraderExchange = new CTraderExchangeTable;
			pTraderExchange->m_sId = TraderExchangeSet.m_sId;
			pTraderExchange->m_sType = TraderExchangeSet.m_sType;
			pTraderExchange->m_sItem = TraderExchangeSet.m_sItem;
			pTraderExchange->m_sMoney = TraderExchangeSet.m_sMoney;
			for (int i = 0; i < 5; i++)
			{
				pTraderExchange->m_sNeedItem[i] = TraderExchangeSet.m_sNeedItem[i];
				pTraderExchange->m_sNeedItemNum[i] =TraderExchangeSet.m_sNeedItemNum[i];
			}
			g_arTraderExchangeTable.Add(pTraderExchange);
			TraderExchangeSet.MoveNext();
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetExpTable()
{
	CExpTableSet ExpTableSet;

	try
	{
		if (ExpTableSet.IsOpen())
		{
			ExpTableSet.Close();
		}

		if (!ExpTableSet.Open())
		{
			AfxMessageBox(_T("Exp DB Open Fail!"));
			return false;
		}

		if (ExpTableSet.IsBOF())
		{
			AfxMessageBox(_T("Exp DB Empty!"));
			return false;
		}

		while (!ExpTableSet.IsEOF())
		{
			CExpTable* pNewExp = new CExpTable;

			pNewExp->m_sid = ExpTableSet.m_sid;
			pNewExp->m_slevel = ExpTableSet.m_slevel;
			pNewExp->m_maxexp = ExpTableSet.m_maxexp;
			pNewExp->m_rdexp = ExpTableSet.m_rdexp;
			pNewExp->m_wdexp = ExpTableSet.m_wdexp;

			g_arExpTable.Add(pNewExp);
			ExpTableSet.MoveNext();
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetSkillExpTable()
{
	CSkillExpTableSet SkillExpTableSet;

	try
	{
		if (SkillExpTableSet.IsOpen())
		{
			SkillExpTableSet.Close();
		}

		if (!SkillExpTableSet.Open())
		{
			AfxMessageBox(_T("SkillExp DB Open Fail!"));
			return false;
		}

		if (SkillExpTableSet.IsBOF())
		{
			AfxMessageBox(_T("SkillExp DB Empty!"));
			return false;
		}

		while (!SkillExpTableSet.IsEOF())
		{
			CSkillExpTable* pNewSkillExp = new CSkillExpTable;

			pNewSkillExp->m_sid = SkillExpTableSet.m_sid;
			pNewSkillExp->m_slevel = SkillExpTableSet.m_slevel;
			pNewSkillExp->m_axe = SkillExpTableSet.m_axe;
			pNewSkillExp->m_bow = SkillExpTableSet.m_bow;
			pNewSkillExp->m_knuckle = SkillExpTableSet.m_knuckle;
			pNewSkillExp->m_spear = SkillExpTableSet.m_spear;
			pNewSkillExp->m_staff = SkillExpTableSet.m_staff;
			pNewSkillExp->m_sword = SkillExpTableSet.m_sword;
			pNewSkillExp->m_darkmg = SkillExpTableSet.m_darkmg;
			pNewSkillExp->m_whitemg = SkillExpTableSet.m_whitemg;
			pNewSkillExp->m_bluemg = SkillExpTableSet.m_bluemg;
			pNewSkillExp->m_wpmake = SkillExpTableSet.m_wpmake;
			pNewSkillExp->m_ammake = SkillExpTableSet.m_ammake;
			pNewSkillExp->m_acmake = SkillExpTableSet.m_acmake;
			pNewSkillExp->m_pomake = SkillExpTableSet.m_pomake;
			pNewSkillExp->m_ckmake = SkillExpTableSet.m_ckmake;
			pNewSkillExp->m_rdaxe = SkillExpTableSet.m_rdaxe;
			pNewSkillExp->m_wdaxe = SkillExpTableSet.m_wdaxe;
			pNewSkillExp->m_rdbow = SkillExpTableSet.m_rdbow;
			pNewSkillExp->m_wdbow = SkillExpTableSet.m_wdbow;
			pNewSkillExp->m_rdknuckle = SkillExpTableSet.m_rdknuckle;
			pNewSkillExp->m_wdknuckle = SkillExpTableSet.m_wdknuckle;
			pNewSkillExp->m_rdspear = SkillExpTableSet.m_rdspear;
			pNewSkillExp->m_wdspear = SkillExpTableSet.m_wdspear;
			pNewSkillExp->m_rdstaff = SkillExpTableSet.m_rdstaff;
			pNewSkillExp->m_wdstaff = SkillExpTableSet.m_wdstaff;
			pNewSkillExp->m_rdsword = SkillExpTableSet.m_rdsword;
			pNewSkillExp->m_wdsword = SkillExpTableSet.m_wdsword;
			pNewSkillExp->m_rddarkmg = SkillExpTableSet.m_rddarkmg;
			pNewSkillExp->m_wddarkmg = SkillExpTableSet.m_wddarkmg;
			pNewSkillExp->m_rdwhitemg = SkillExpTableSet.m_rdwhitemg;
			pNewSkillExp->m_wdwhitemg = SkillExpTableSet.m_wdwhitemg;
			pNewSkillExp->m_rdbluemg = SkillExpTableSet.m_rdbluemg;
			pNewSkillExp->m_wdbluemg = SkillExpTableSet.m_wdbluemg;

			g_arSkillExpTable.Add(pNewSkillExp);
			SkillExpTableSet.MoveNext();
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetStatExpTable()
{
	CStatExpTableSet StatExpTableSet;

	try
	{
		if (StatExpTableSet.IsOpen())
		{
			StatExpTableSet.Close();
		}

		if (!StatExpTableSet.Open())
		{
			AfxMessageBox(_T("StatExp DB Open Fail!"));
			return false;
		}

		if (StatExpTableSet.IsBOF())
		{
			AfxMessageBox(_T("StatExp DB Empty!"));
			return false;
		}

		while (!StatExpTableSet.IsEOF())
		{
			CStatExpTable* pNewStatExp = new CStatExpTable;

			pNewStatExp->m_sid = StatExpTableSet.m_sid;
			pNewStatExp->m_slevel = StatExpTableSet.m_slevel;
			pNewStatExp->m_str = StatExpTableSet.m_str;
			pNewStatExp->m_dex = StatExpTableSet.m_dex;
			pNewStatExp->m_int = StatExpTableSet.m_int;
			pNewStatExp->m_wis = StatExpTableSet.m_wis;
			pNewStatExp->m_rdstr = StatExpTableSet.m_rdstr;
			pNewStatExp->m_wdstr = StatExpTableSet.m_wdstr;
			pNewStatExp->m_rddex = StatExpTableSet.m_rddex;
			pNewStatExp->m_wddex = StatExpTableSet.m_wddex;
			pNewStatExp->m_rdint = StatExpTableSet.m_rdint;
			pNewStatExp->m_wdint = StatExpTableSet.m_wdint;
			pNewStatExp->m_rdwis = StatExpTableSet.m_rdwis;
			pNewStatExp->m_wdwis = StatExpTableSet.m_wdwis;
			g_arStatExpTable.Add(pNewStatExp);
			StatExpTableSet.MoveNext();
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetConExpTable()
{
	CConExpTableSet	ConExpTableSet;

	try
	{
		if (ConExpTableSet.IsOpen())
		{
			ConExpTableSet.Close();
		}

		if (!ConExpTableSet.Open())
		{
			AfxMessageBox(_T("ConExp DB Open Fail!"));
			return false;
		}

		if (ConExpTableSet.IsBOF())
		{
			AfxMessageBox(_T("ConExp DB Empty!"));
			return false;
		}

		while (!ConExpTableSet.IsEOF())
		{
			CConExpTable* pNewConExp = new CConExpTable;

			pNewConExp->m_sid = ConExpTableSet.m_sid;
			pNewConExp->m_slevel = ConExpTableSet.m_slevel;
			pNewConExp->m_con = ConExpTableSet.m_con;
			pNewConExp->m_rdcon = ConExpTableSet.m_rdcon;
			pNewConExp->m_wdcon = ConExpTableSet.m_wdcon;
			g_arConExpTable.Add(pNewConExp);
			ConExpTableSet.MoveNext();
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetChaExpTable()
{
	CChaExpTableSet	ChaExpTableSet;

	try
	{
		if (ChaExpTableSet.IsOpen())
		{
			ChaExpTableSet.Close();
		}

		if (!ChaExpTableSet.Open())
		{
			AfxMessageBox(_T("ChaExp DB Open Fail!"));
			return false;
		}

		if (ChaExpTableSet.IsBOF())
		{
			AfxMessageBox(_T("ChaExp DB Empty!"));
			return false;
		}

		while (!ChaExpTableSet.IsEOF())
		{
			CChaExpTable* pNewChaExp = new CChaExpTable;

			pNewChaExp->m_sid = ChaExpTableSet.m_sid;
			pNewChaExp->m_slevel = ChaExpTableSet.m_slevel;
			pNewChaExp->m_cha = ChaExpTableSet.m_cha;
			pNewChaExp->m_rdcha = ChaExpTableSet.m_rdcha;
			pNewChaExp->m_wdcha = ChaExpTableSet.m_wdcha;
			g_arChaExpTable.Add(pNewChaExp);
			ChaExpTableSet.MoveNext();
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetExpTableD()
{
	CExpTableDSet ExpTableSet;

	try
	{
		if (ExpTableSet.IsOpen())
		{
			ExpTableSet.Close();
		}

		if (!ExpTableSet.Open())
		{
			AfxMessageBox(_T("Exp DB Open Fail!"));
			return false;
		}

		if (ExpTableSet.IsBOF())
		{
			AfxMessageBox(_T("Exp DB Empty!"));
			return false;
		}

		while (!ExpTableSet.IsEOF())
		{
			CExpTableD* pNewExp = new CExpTableD;

			pNewExp->m_sid = ExpTableSet.m_sid;
			pNewExp->m_slevel = ExpTableSet.m_slevel;
			pNewExp->m_maxexp = ExpTableSet.m_maxexp;
			pNewExp->m_rdexp = ExpTableSet.m_rdexp;
			pNewExp->m_wdexp = ExpTableSet.m_wdexp;

			g_arExpTableD.Add(pNewExp);
			ExpTableSet.MoveNext();
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetSkillExpTableD()
{
	CSkillExpTableDSet SkillExpTableSet;

	try
	{
		if (SkillExpTableSet.IsOpen())
		{
			SkillExpTableSet.Close();
		}

		if (!SkillExpTableSet.Open())
		{
			AfxMessageBox(_T("SkillExp DB Open Fail!"));
			return false;
		}

		if (SkillExpTableSet.IsBOF())
		{
			AfxMessageBox(_T("SkillExp DB Empty!"));
			return false;
		}

		while (!SkillExpTableSet.IsEOF())
		{
			CSkillExpTableD* pNewSkillExp = new CSkillExpTableD;

			pNewSkillExp->m_sid = SkillExpTableSet.m_sid;
			pNewSkillExp->m_slevel = SkillExpTableSet.m_slevel;
			pNewSkillExp->m_axe = SkillExpTableSet.m_axe;
			pNewSkillExp->m_bow = SkillExpTableSet.m_bow;
			pNewSkillExp->m_knuckle = SkillExpTableSet.m_knuckle;
			pNewSkillExp->m_spear = SkillExpTableSet.m_spear;
			pNewSkillExp->m_staff = SkillExpTableSet.m_staff;
			pNewSkillExp->m_sword = SkillExpTableSet.m_sword;
			pNewSkillExp->m_darkmg = SkillExpTableSet.m_darkmg;
			pNewSkillExp->m_whitemg = SkillExpTableSet.m_whitemg;
			pNewSkillExp->m_bluemg = SkillExpTableSet.m_bluemg;
			pNewSkillExp->m_wpmake = SkillExpTableSet.m_wpmake;
			pNewSkillExp->m_ammake = SkillExpTableSet.m_ammake;
			pNewSkillExp->m_acmake = SkillExpTableSet.m_acmake;
			pNewSkillExp->m_pomake = SkillExpTableSet.m_pomake;
			pNewSkillExp->m_ckmake = SkillExpTableSet.m_ckmake;
			pNewSkillExp->m_rdaxe = SkillExpTableSet.m_rdaxe;
			pNewSkillExp->m_wdaxe = SkillExpTableSet.m_wdaxe;
			pNewSkillExp->m_rdbow = SkillExpTableSet.m_rdbow;
			pNewSkillExp->m_wdbow = SkillExpTableSet.m_wdbow;
			pNewSkillExp->m_rdknuckle = SkillExpTableSet.m_rdknuckle;
			pNewSkillExp->m_wdknuckle = SkillExpTableSet.m_wdknuckle;
			pNewSkillExp->m_rdspear = SkillExpTableSet.m_rdspear;
			pNewSkillExp->m_wdspear = SkillExpTableSet.m_wdspear;
			pNewSkillExp->m_rdstaff = SkillExpTableSet.m_rdstaff;
			pNewSkillExp->m_wdstaff = SkillExpTableSet.m_wdstaff;
			pNewSkillExp->m_rdsword = SkillExpTableSet.m_rdsword;
			pNewSkillExp->m_wdsword = SkillExpTableSet.m_wdsword;
			pNewSkillExp->m_rddarkmg = SkillExpTableSet.m_rddarkmg;
			pNewSkillExp->m_wddarkmg = SkillExpTableSet.m_wddarkmg;
			pNewSkillExp->m_rdwhitemg = SkillExpTableSet.m_rdwhitemg;
			pNewSkillExp->m_wdwhitemg = SkillExpTableSet.m_wdwhitemg;
			pNewSkillExp->m_rdbluemg = SkillExpTableSet.m_rdbluemg;
			pNewSkillExp->m_wdbluemg = SkillExpTableSet.m_wdbluemg;

			g_arSkillExpTableD.Add(pNewSkillExp);
			SkillExpTableSet.MoveNext();
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return FALSE;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetStatExpTableD()
{
	CStatExpTableDSet StatExpTableSet;

	try
	{
		if (StatExpTableSet.IsOpen())
		{
			StatExpTableSet.Close();
		}

		if (!StatExpTableSet.Open())
		{
			AfxMessageBox(_T("StatExp DB Open Fail!"));
			return false;
		}

		if (StatExpTableSet.IsBOF())
		{
			AfxMessageBox(_T("StatExp DB Empty!"));
			return false;
		}

		while (!StatExpTableSet.IsEOF())
		{
			CStatExpTableD* pNewStatExp = new CStatExpTableD;

			pNewStatExp->m_sid = StatExpTableSet.m_sid;
			pNewStatExp->m_slevel = StatExpTableSet.m_slevel;
			pNewStatExp->m_str = StatExpTableSet.m_str;
			pNewStatExp->m_dex = StatExpTableSet.m_dex;
			pNewStatExp->m_int = StatExpTableSet.m_int;
			pNewStatExp->m_wis = StatExpTableSet.m_wis;
			pNewStatExp->m_rdstr = StatExpTableSet.m_rdstr;
			pNewStatExp->m_wdstr = StatExpTableSet.m_wdstr;
			pNewStatExp->m_rddex = StatExpTableSet.m_rddex;
			pNewStatExp->m_wddex = StatExpTableSet.m_wddex;
			pNewStatExp->m_rdint = StatExpTableSet.m_rdint;
			pNewStatExp->m_wdint = StatExpTableSet.m_wdint;
			pNewStatExp->m_rdwis = StatExpTableSet.m_rdwis;
			pNewStatExp->m_wdwis = StatExpTableSet.m_wdwis;
			g_arStatExpTableD.Add(pNewStatExp);
			StatExpTableSet.MoveNext();
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetConExpTableD()
{
	CConExpTableDSet ConExpTableSet;

	try
	{
		if (ConExpTableSet.IsOpen())
		{
			ConExpTableSet.Close();
		}

		if (!ConExpTableSet.Open())
		{
			AfxMessageBox(_T("ConExp DB Open Fail!"));
			return false;
		}

		if (ConExpTableSet.IsBOF())
		{
			AfxMessageBox(_T("ConExp DB Empty!"));
			return false;
		}

		while (!ConExpTableSet.IsEOF())
		{
			CConExpTableD* pNewConExp = new CConExpTableD;

			pNewConExp->m_sid = ConExpTableSet.m_sid;
			pNewConExp->m_slevel = ConExpTableSet.m_slevel;
			pNewConExp->m_con = ConExpTableSet.m_con;
			pNewConExp->m_rdcon = ConExpTableSet.m_rdcon;
			pNewConExp->m_wdcon = ConExpTableSet.m_wdcon;
			g_arConExpTableD.Add(pNewConExp);
			ConExpTableSet.MoveNext();
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetChaExpTableD()
{
	CChaExpTableDSet ChaExpTableSet;

	try
	{
		if (ChaExpTableSet.IsOpen())
		{
			ChaExpTableSet.Close();
		}

		if (!ChaExpTableSet.Open())
		{
			AfxMessageBox(_T("ChaExp DB Open Fail!"));
			return false;
		}

		if (ChaExpTableSet.IsBOF())
		{
			AfxMessageBox(_T("ChaExp DB Empty!"));
			return false;
		}

		while (!ChaExpTableSet.IsEOF())
		{
			CChaExpTableD* pNewChaExp = new CChaExpTableD;

			pNewChaExp->m_sid = ChaExpTableSet.m_sid;
			pNewChaExp->m_slevel = ChaExpTableSet.m_slevel;
			pNewChaExp->m_cha = ChaExpTableSet.m_cha;
			pNewChaExp->m_rdcha = ChaExpTableSet.m_rdcha;
			pNewChaExp->m_wdcha = ChaExpTableSet.m_wdcha;
			g_arChaExpTableD.Add(pNewChaExp);
			ChaExpTableSet.MoveNext();
		}
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}
	catch (CDBException* e)
	{
		e->ReportError();
		e->Delete();
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetZoneClass()
{
	g_mapZoneClass[1] = 0;
	g_mapZoneClass[2] = 0;
	g_mapZoneClass[3] = 0;
	g_mapZoneClass[4] = 0;
	g_mapZoneClass[5] = 0;
	g_mapZoneClass[6] = 0;
	g_mapZoneClass[7] = 0;
	g_mapZoneClass[8] = 0;
	g_mapZoneClass[9] = 10;
	g_mapZoneClass[10] = 10;
	g_mapZoneClass[11] = 10;
	g_mapZoneClass[12] = 0;
	g_mapZoneClass[13] = 10;
	g_mapZoneClass[14] = g_iClassWar;
	g_mapZoneClass[15] = g_iClassWar;
	g_mapZoneClass[16] = g_iClassWar;
	g_mapZoneClass[17] = g_iClassWar;
	g_mapZoneClass[18] = 10;
	g_mapZoneClass[19] = 10;
	g_mapZoneClass[20] = 10;
	g_mapZoneClass[21] = 10;
	g_mapZoneClass[22] = 0;
	return true;
}

void CMy1p1EmuDlg::GetZoneDetails()
{
	g_mapZoneDetails[1].unknown = 1;
	g_mapZoneDetails[1].nightType = 1;
	g_mapZoneDetails[1].hasMusic = 1;

	g_mapZoneDetails[2].unknown = 0;
	g_mapZoneDetails[2].nightType = 0;
	g_mapZoneDetails[2].hasMusic = 1;

	g_mapZoneDetails[3].unknown = 0;
	g_mapZoneDetails[3].nightType = 0;
	g_mapZoneDetails[3].hasMusic = 1;

	g_mapZoneDetails[4].unknown = 1;
	g_mapZoneDetails[4].nightType = 1;
	g_mapZoneDetails[4].hasMusic = 1;

	g_mapZoneDetails[5].unknown = 1;
	g_mapZoneDetails[5].nightType = 1;
	g_mapZoneDetails[5].hasMusic = 1;

	g_mapZoneDetails[6].unknown = 0;
	g_mapZoneDetails[6].nightType = 0;
	g_mapZoneDetails[6].hasMusic = 1;

	g_mapZoneDetails[7].unknown = 0;
	g_mapZoneDetails[7].nightType = 0;
	g_mapZoneDetails[7].hasMusic = 1;

	g_mapZoneDetails[8].unknown = 0;
	g_mapZoneDetails[8].nightType = 2;
	g_mapZoneDetails[8].hasMusic = 1;

	g_mapZoneDetails[9].unknown = 0;
	g_mapZoneDetails[9].nightType = 2;
	g_mapZoneDetails[9].hasMusic = 1;

	g_mapZoneDetails[10].unknown = 0;
	g_mapZoneDetails[10].nightType = 2;
	g_mapZoneDetails[10].hasMusic = 1;

	g_mapZoneDetails[11].unknown = 0;
	g_mapZoneDetails[11].nightType = 2;
	g_mapZoneDetails[11].hasMusic = 1;

	g_mapZoneDetails[12].unknown = 0;
	g_mapZoneDetails[12].nightType = 0;
	g_mapZoneDetails[12].hasMusic = 1;

	g_mapZoneDetails[13].unknown = 0;
	g_mapZoneDetails[13].nightType = 2;
	g_mapZoneDetails[13].hasMusic = 1;

	g_mapZoneDetails[14].unknown = 0;
	g_mapZoneDetails[14].nightType = 1;
	g_mapZoneDetails[14].hasMusic = 1;

	g_mapZoneDetails[15].unknown = 0;
	g_mapZoneDetails[15].nightType = 2;
	g_mapZoneDetails[15].hasMusic = 1;

	g_mapZoneDetails[16].unknown = 0;
	g_mapZoneDetails[16].nightType = 2;
	g_mapZoneDetails[16].hasMusic = 1;

	g_mapZoneDetails[17].unknown = 0;
	g_mapZoneDetails[17].nightType = 1;
	g_mapZoneDetails[17].hasMusic = 1;

	g_mapZoneDetails[18].unknown = 0;
	g_mapZoneDetails[18].nightType = 1;
	g_mapZoneDetails[18].hasMusic = 1;

	g_mapZoneDetails[19].unknown = 0;
	g_mapZoneDetails[19].nightType = 2;
	g_mapZoneDetails[19].hasMusic = 1;

	g_mapZoneDetails[20].unknown = 0;
	g_mapZoneDetails[20].nightType = 2;
	g_mapZoneDetails[20].hasMusic = 1;

	g_mapZoneDetails[21].unknown = 0;
	g_mapZoneDetails[21].nightType = 0;
	g_mapZoneDetails[21].hasMusic = 1;

	g_mapZoneDetails[22].unknown = 0;
	g_mapZoneDetails[22].nightType = 1;
	g_mapZoneDetails[22].hasMusic = 0;
}

////////////////////////////////////////////////////////////////////////////////////
//	Server Information DB Open & Load ...Zones
//
bool CMy1p1EmuDlg::GetServerInfoData()
{
	SQLHSTMT hstmt = NULL;
	SQLRETURN retcode;
	TCHAR szSQL[1024];
	::ZeroMemory(szSQL, sizeof(szSQL));

	SQLINTEGER sInd = 0;
	SQLSMALLINT sSid = 0;
	SQLSMALLINT sPort = 0;
	SQLSMALLINT	sZone = 0
	SQLSMALLINT sDBIndex = -1;
	SQLCHAR	strAddr[16];
	::ZeroMemory(strAddr, sizeof(strAddr));

	_sntprintf(szSQL, sizeof(szSQL), TEXT("SELECT * FROM SERVERINFO"));

	int db_index = 0;
	CDatabase* pDB = g_DB[AUTOMATA_THREAD].GetDB(db_index);
	if (!pDB)
	{
		return false;
	}

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);

	if (retcode != SQL_SUCCESS)
	{
		TRACE("Fail To Load ServerInfo Data !!\n");
		return false;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	int dbindex = 1;
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		while (1)
		{
			retcode = SQLFetch(hstmt);
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				sDBIndex = dbindex++;
				SERVERINFO* pNewData = new SERVERINFO;

				int c = 1;
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sSid, sizeof(SQLSMALLINT), &sInd);
				SQLGetData(hstmt, c++, SQL_C_CHAR,	strAddr,sizeof(strAddr), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sPort, sizeof(SQLSMALLINT), &sInd);

				while (true)
				{
					retcode = SQLGetData(hstmt, c++, SQL_C_SSHORT, &sZone, sizeof(SQLSMALLINT), &sInd);

					if (retcode != SQL_SUCCESS)
					{
						break;
					}

					int iNum = pNewData->m_zone.GetSize();
					if (iNum >= 1)
					{
						if (sZone == *(pNewData->m_zone[iNum - 1]))
						{
							break;
						}

						if (sZone == -1)
						{
							continue;
						}

						if (c > 100)
						{
							return FALSE;
						}
					}

					int* pZone = new int;
					*pZone = (int)sZone;
					pNewData->m_zone.Add(pZone);

					sZone = -1;
				};

				pNewData->m_sid	= sSid;
				pNewData->m_sDBIndex= sDBIndex;
				pNewData->m_szAddr = strAddr;
				pNewData->m_nPort = sPort;

				g_ServerInfo.Add(pNewData);
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
		return false;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
	return true;
}

bool CMy1p1EmuDlg::LoadZoneData()
{
	CString szZone = _T("");
	CString szTemp = _T("");
	int nZoneSize = g_ServerInfo[0]->m_zone.GetSize(); // Only 1 server
	BOOL bRet = true;

	for (int i = 0; i < nZoneSize; i++)
	{
		// Map
		MAP* pNewMap = new MAP;
		int nZoneNumber = *(g_ServerInfo[0]->m_zone[i]);

		if (!pNewMap->LoadMapByNum(nZoneNumber))
		{
			bRet = false;
			delete pNewMap;
			continue;
		}
		g_Zones.Add(pNewMap);

		// Event
		MAP* pNewEvent = new EVENT;
		if (!pNewEvent->LoadEvent(nZoneNumber))
		{
			bRet = false;
			delete pNewEvent;
			continue;
		}
		g_Events.Add(pNewEvent);

		szTemp.Format("%d ", nZoneNumber);
		szZone += szTemp;
	}

	if (bRet == false)
	{
		szTemp.Format("[ %d ] Map, Event Information Loding Fail!!!!", nZoneNumber);
		AfxMessageBox(szTemp);
		return false;
	}

	return true;
}

bool CMy1p1EmuDlg::GetGuildData()
{
	SQLHSTMT hstmt = NULL;
	SQLRETURN retcode;
	TCHAR szSQL[1024];
	::ZeroMemory(szSQL, sizeof(szSQL));

	g_arGuild.SetSize(MAX_GUILD);
	for (int i = 0; i < g_arGuild.GetSize(); i++)
	{
		Guild* pNewGuild = new Guild;
		g_arGuild[i] = pNewGuild;
	}

	SQLINTEGER	sInd = 0;
	SQLSMALLINT sGuildNum = 0;
	SQLSMALLINT sGuildClass = 0;
	SQLCHAR strGuildName[GUILD_NAME_LENGTH+1];
	::ZeroMemory(strGuildName, sizeof(strGuildName));
	SQLSMALLINT sState = 0;
	SQLSMALLINT sStorageOpenType = 0;
	SQLCHAR strNoGuildName[GUILD_NAME_LENGTH+1];
	::ZeroMemory(strNoGuildName, sizeof(strNoGuildName));
	SQLSMALLINT sTaxRate = 0;
	SQLINTEGER iGuildBarr = 0;
	SQLCHAR strInfo[Guild::GUILD_INFO_SIZE+1];
	::ZeroMemory(strInfo, sizeof(strInfo));
	SQLCHAR strCallName[Guild::GUILD_RANKS][Guild::GUILD_CALL_LENGTH+1];
	for (int i = 0; i < Guild::GUILD_RANKS; i++)
	{
		::ZeroMemory(strCallName[i], sizeof(strCallName[i]));
	}
	SQLSMALLINT sSymbolVersion = 0;
	SQLCHAR strSymbol[GUILD_SYMBOL_SIZE+1];
	::ZeroMemory(strSymbol, sizeof(strSymbol));

	_sntprintf(szSQL, sizeof(szSQL), TEXT("SELECT * FROM GUILD"));

	int db_index = 0;
	CDatabase* pDB = g_DB[AUTOMATA_THREAD].GetDB(db_index);
	if (!pDB)
	{
		return false;
	}

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);

	if (retcode != SQL_SUCCESS)
	{
		TRACE("Fail To Load Guild Data !!\n");
		return false;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		while (true)
		{
			retcode = SQLFetch(hstmt);
			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				int c = 1;
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sGuildNum, sizeof(sGuildNum), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sGuildClass, sizeof(sGuildClass), &sInd);
				SQLGetData(hstmt, c++, SQL_C_CHAR, &strGuildName, sizeof(strGuildName), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sState, sizeof(sState), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sStorageOpenType, sizeof(sStorageOpenType), &sInd);
				SQLGetData(hstmt, c++, SQL_C_CHAR, &strNoGuildName, sizeof(strNoGuildName), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sTaxRate, sizeof(sTaxRate), &sInd);
				SQLGetData(hstmt, c++, SQL_C_ULONG, &iGuildBarr, sizeof(iGuildBarr), &sInd);
				SQLGetData(hstmt, c++, SQL_C_CHAR, &strInfo, sizeof(strInfo), &sInd);
				for (int k = 0; k < Guild::GUILD_RANKS; k++)
				{
					SQLGetData(hstmt, c++, SQL_C_CHAR, &strCallName[k], sizeof(strCallName[k]), &sInd);
				}
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sSymbolVersion, sizeof(sSymbolVersion), &sInd);
				SQLGetData(hstmt, c++, SQL_C_BINARY, &strSymbol, sizeof(strSymbol), &sInd);

				if (sGuildNum < 0 || sGuildNum >= MAX_GUILD)
				{
					TRACE(_T("Guild Number loaded from Guild is less than 0 or exceeds max"));
					continue;
				}

				TRACE("LOADING GUILD NUM: %d Name: %s\n", sGuildNum, strGuildName);

				Guild* pGuild = g_arGuild[sGuildNum];
				pGuild->m_sNum = sGuildNum;
				pGuild->m_sClass = sGuildClass;
				pGuild->m_sState = sState;
				pGuild->m_sStorageOpenType = sStorageOpenType;
				pGuild->m_TaxRate = sTaxRate;
				strcpy(pGuild->m_strGuildName, reinterpret_cast<char*>(strGuildName));
				strcpy(pGuild->m_strInfo, reinterpret_cast<char*>(strInfo));
				for (int k = 0; k < Guild::GUILD_RANKS; k++)
				{
					strcpy(pGuild->m_strCallName[k], reinterpret_cast<char*>(strCallName[k]));
				}

				pGuild->m_dwBarr = iGuildBarr;

				pGuild->m_sSymbolVersion = sSymbolVersion;
				strcpy(pGuild->m_strSymbol, (char*)strSymbol);

				::ZeroMemory(strGuildName, sizeof(strGuildName));
				::ZeroMemory(strNoGuildName, sizeof(strNoGuildName));
				::ZeroMemory(strInfo, sizeof(strInfo));
				for (int i = 0; i < Guild::GUILD_RANKS; i++)
				{
					::ZeroMemory(strCallName[i], sizeof(strCallName[i]));
				}
				::ZeroMemory(strSymbol, sizeof(strSymbol));
			}
			else break;
		}
	}
	else
	{
		DisplayErrorMsg(hstmt);
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
		return false;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
	return true;
}

bool CMy1p1EmuDlg::GetGuildMemberData()
{
	int i;
	SQLHSTMT hstmt;
	SQLRETURN retcode;
	TCHAR szSQL[2048];
	::ZeroMemory(szSQL, sizeof(szSQL));
	wsprintf(szSQL,TEXT("SELECT * FROM GUILD_USER"));

	SQLSMALLINT	sGuild = 0
	SQLSMALLINT sRank = 0;
	SQLCHAR	strUserId[NAME_LENGTH+1];
	::ZeroMemory(strUserId, sizeof(strUserId));

	int db_index = 0;
	CDatabase* pDB = g_DB[AUTOMATA_THREAD].GetDB(db_index);
	if (!pDB)
	{
		return false;
	}

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("Fail To Load Guild User Data !!\n");
		return false;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		while (true)
		{
			retcode = SQLFetch(hstmt);

			if (retcode ==SQL_SUCCESS || retcode ==SQL_SUCCESS_WITH_INFO)
			{
				int c = 1;
				SQLINTEGER sInd;
				SQLGetData(hstmt, c++, SQL_C_CHAR, &strUserId, sizeof(strUserId), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sGuild, sizeof(sGuild), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sRank, sizeof(sRank), &sInd);

				if (sGuild < 0 || sGuild >= MAX_GUILD)
				{
					continue;
				}

				if (g_arGuild[sGuild])
				{
					g_arGuild[sGuild]->AddMember((TCHAR *)strUserId, sRank);
				}
			}
			else
			{
				break;
			}
		}
	}
	else if (retcode == SQL_NO_DATA)
	{
		g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
		return false;
	}
	else
	{
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
		return false;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
	return true;
}

bool CMy1p1EmuDlg::GetGuildReqData()
{
	SQLHSTMT		hstmt;
	SQLRETURN		retcode;
	TCHAR			szSQL[2048];
	::ZeroMemory(szSQL, sizeof(szSQL));
	wsprintf(szSQL,TEXT("SELECT * FROM GUILD_REQ"));

	SQLSMALLINT	sGuild = 0;
	SQLCHAR	strUserId[NAME_LENGTH + 1];
	::ZeroMemory(strUserId, sizeof(strUserId));

	int db_index = 0;
	CDatabase* pDB = g_DB[AUTOMATA_THREAD].GetDB(db_index);
	if (!pDB)
	{
		return false;
	}

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("Fail To Load Guild User Data !!\n");
		return false;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		while (true)
		{
			retcode = SQLFetch(hstmt);

			if (retcode ==SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				int c = 1;
				SQLINTEGER sInd;
				SQLGetData(hstmt, c++, SQL_C_CHAR, &strUserId, sizeof(strUserId), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sGuild, sizeof(sGuild), &sInd);

				if (sGuild < 0 || sGuild >= MAX_GUILD)
				{
					continue;
				}

				if (g_arGuild[sGuild])
				{
					g_arGuild[sGuild]->AddApplicant((TCHAR *)strUserId);
				}
			}
			else break;
		}
	}
	else if (retcode == SQL_NO_DATA)
	{
		g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
		return false;
	}
	else
	{
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
		return false;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
	return true;
}

bool CMy1p1EmuDlg::GetEventViewData()
{
	SQLHSTMT hstmt;
	SQLRETURN retcode;
	TCHAR szSQL[2048];
	::ZeroMemory(szSQL, sizeof(szSQL));
	wsprintf(szSQL,TEXT("SELECT * FROM EVENT_VIEW"));

	SQLSMALLINT	sId = 0;
	SQLSMALLINT sStartEvent = 0;
	SQLSMALLINT sEndEvent = 0;
	SQLSMALLINT sClass = 0;
	SQLSMALLINT sLevel = 0;
	SQLSMALLINT sCheckEvent = 0;
	SQLCHAR	strEventName[50];
	SQLCHAR	strEventInfo[256];
	SQLCHAR	strEventAll[256];
	::ZeroMemory(strEventName, sizeof(strEventName));
	::ZeroMemory(strEventInfo, sizeof(strEventInfo));
	::ZeroMemory(strEventAll, sizeof(strEventAll));

	int db_index = 0;
	CDatabase* pDB = g_DB[AUTOMATA_THREAD].GetDB(db_index);
	if (!pDB)
	{
		return false;
	}

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("Fail To Load Event View Data !!\n");
		return false;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		while (true)
		{
			retcode = SQLFetch(hstmt);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				int c = 1;
				SQLINTEGER sInd;
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sId, sizeof(sId), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sStartEvent, sizeof(sStartEvent), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sEndEvent, sizeof(sEndEvent), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sClass, sizeof(sClass), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sLevel, sizeof(sLevel), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sCheckEvent, sizeof(sCheckEvent), &sInd);
				SQLGetData(hstmt, c++, SQL_C_CHAR, &strEventName, sizeof(strEventName), &sInd);
				SQLGetData(hstmt, c++, SQL_C_CHAR, &strEventInfo, sizeof(strEventInfo), &sInd);
				SQLGetData(hstmt, c++, SQL_C_CHAR, &strEventAll, sizeof(strEventAll), &sInd);

				EventViewData* e = new EventViewData;
				e->sId = sId;
				e->sStartEvent = sStartEvent;
				e->sEndEvent = sEndEvent;
				e->sClass = sClass;
				e->sLevel = sLevel;
				e->sCheckEvent = sCheckEvent;
				e->strEventName = strEventName;
				e->strEventInfo = strEventInfo;
				e->strEventAll = strEventAll;

				g_arEventViewData.Add(e);
			}
			else break;
		}
	}
	else if (retcode == SQL_NO_DATA)
	{
		g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
		return false;
	}
	else
	{
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
		return false;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
	return true;
}

bool CMy1p1EmuDlg::GetSpecialItemData()
{
	SQLHSTMT hstmt;
	SQLRETURN retcode;
	TCHAR szSQL[2048];
	::ZeroMemory(szSQL, sizeof(szSQL));
	wsprintf(szSQL,TEXT("SELECT * FROM tb_special_item"));

	SQLSMALLINT	sId = 0;
	SQLSMALLINT sItemNum = 0;
	SQLSMALLINT sMinAb1 = 0;
	SQLSMALLINT sMaxAb1 = 0;
	SQLSMALLINT sMinAb2 = 0;
	SQLSMALLINT sMaxAb2 = 0;
	SQLSMALLINT sMinDur = 0;
	SQLSMALLINT sMaxDur = 0;
	SQLSMALLINT sMinTime = 0;
	SQLSMALLINT sMaxTime = 0;
	SQLSMALLINT sMinWg = 0;
	SQLSMALLINT	sMaxWg = 0;

	int db_index = 0;
	CDatabase* pDB = g_DB[AUTOMATA_THREAD].GetDB(db_index);
	if (!pDB)
	{
		return false;
	}

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("Fail To Load Special Item Data !!\n");
		return false;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		while (true)
		{
			retcode = SQLFetch(hstmt);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				int c = 1;
				SQLINTEGER sInd;
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sId, sizeof(sId), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sItemNum, sizeof(sItemNum), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sMinAb1, sizeof(sMinAb1), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sMaxAb1, sizeof(sMaxAb1), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sMinAb2, sizeof(sMinAb2), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sMaxAb2, sizeof(sMaxAb2), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sMinDur, sizeof(sMinDur), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sMaxDur, sizeof(sMaxDur), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sMinTime, sizeof(sMinTime), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sMaxTime, sizeof(sMaxTime), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sMinWg, sizeof(sMinWg), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sMaxWg, sizeof(sMaxWg), &sInd);

				SpecialItemData* s = new SpecialItemData;
				s->sId = sId;
				s->sItemNum = sItemNum;
				s->sMinAb1 = sMinAb1;
				s->sMaxAb1 = sMaxAb1;
				s->sMinAb2 = sMinAb2;
				s->sMaxAb2 = sMaxAb2;
				s->sMinDur = sMinDur;
				s->sMaxDur = sMaxDur;
				s->sMinTime = sMinTime;
				s->sMaxTime = sMaxTime;
				s->sMinWg = sMinWg;
				s->sMaxWg = sMaxWg;
				g_arSpecialItemData.Add(s);
			}
			else break;
		}
	}
	else if (retcode == SQL_NO_DATA)
	{
		g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
		return false;
	}
	else
	{
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
		return false;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
	return true;
}

bool CMy1p1EmuDlg::GetChangeRandItemData()
{
	SQLHSTMT hstmt;
	SQLRETURN retcode;
	TCHAR szSQL[2048];
	::ZeroMemory(szSQL, sizeof(szSQL));
	wsprintf(szSQL,TEXT("SELECT * FROM change_rand_item"));

	SQLSMALLINT	sId = 0;
	SQLSMALLINT sGroup = 0;
	SQLSMALLINT sItemNum = 0;

	int db_index = 0;
	CDatabase* pDB = g_DB[AUTOMATA_THREAD].GetDB(db_index);
	if (!pDB)
	{
		return false;
	}

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("Fail To Load Change Rand Item Data !!\n");
		return false;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		while (true)
		{
			retcode = SQLFetch(hstmt);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				int c = 1;
				SQLINTEGER sInd;
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sId, sizeof(sId), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sGroup, sizeof(sGroup), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sItemNum, sizeof(sItemNum), &sInd);

				ChangeRandItemData* d = new ChangeRandItemData;
				d->sId = sId;
				d->sGroup = sGroup;
				d->sItemNum = sItemNum;
				g_arChangeRandItemData[d->sGroup].Add(d);
			}
			else break;
		}
	}
	else if (retcode == SQL_NO_DATA)
	{
		g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
		return false;
	}
	else
	{
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
		return false;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
	return true;
}

bool CMy1p1EmuDlg::GetGuildTownData()
{
	SQLHSTMT hstmt;
	SQLRETURN retcode;
	TCHAR szSQL[2048];
	::ZeroMemory(szSQL, sizeof(szSQL));
	wsprintf(szSQL,TEXT("SELECT sType, sTownNum, sGuildNum, strGuildName, sLevel, sTaxRate, sAlly1, sAlly2, sAlly3 FROM Guild_Town where sType = 1"));

	SQLSMALLINT	sType = 0;
	SQLSMALLINT sTownNum = 1
	SQLSMALLINT sGuildNum = -1;
	SQLSMALLINT sLevel = 1;
	SQLSMALLINT sTaxRate = -1;
    SQLSMALLINT sAlly1 = -1;
	SQLSMALLINT sAlly2 = -1;
	SQLSMALLINT sAlly3 = -1;

	SQLCHAR	strGuildName[GUILD_NAME_LENGTH];
	::ZeroMemory(strGuildName, sizeof(strGuildName));

	int db_index = 0;
	CDatabase* pDB = g_DB[AUTOMATA_THREAD].GetDB(db_index);
	if (!pDB)
	{
		return false;
	}

	retcode = SQLAllocHandle((SQLSMALLINT)SQL_HANDLE_STMT, pDB->m_hdbc, &hstmt);
	if (retcode != SQL_SUCCESS)
	{
		TRACE("Fail To Load Guild Town Data !!\n");
		return false;
	}

	retcode = SQLExecDirect(hstmt, (unsigned char*)szSQL, SQL_NTS);
	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
	{
		while (true)
		{
			retcode = SQLFetch(hstmt);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
				int c = 1;
				SQLINTEGER sInd;
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sType, sizeof(sType), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sTownNum, sizeof(sTownNum), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sGuildNum, sizeof(sGuildNum), &sInd);
				SQLGetData(hstmt, c++, SQL_C_CHAR, &strGuildName, sizeof(strGuildName), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sLevel, sizeof(sLevel), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sTaxRate, sizeof(sTaxRate), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sAlly1, sizeof(sAlly1), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sAlly2, sizeof(sAlly2), &sInd);
				SQLGetData(hstmt, c++, SQL_C_SSHORT, &sAlly3, sizeof(sAlly3), &sInd);

				GuildTownData* t = new GuildTownData;
				t->sType = sType;
				t->sTownNum = sTownNum;
				t->sGuildNum = sGuildNum;
				strcpy(t->strGuildName, reinterpret_cast<char*>(strGuildName));
				t->sLevel = sLevel;
				t->sTaxRate = sTaxRate;
				t->sAlly[0] = sAlly1;
				t->sAlly[1] = sAlly2;
				t->sAlly[2] = sAlly3;
				g_arGuildTownData.Add(t);
			}
			else break;
		}
	}
	else if (retcode==SQL_NO_DATA)
	{
		g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
		return false;
	}
	else
	{
		retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
		g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
		return false;
	}

	retcode = SQLFreeHandle((SQLSMALLINT)SQL_HANDLE_STMT, hstmt);
	g_DB[AUTOMATA_THREAD].ReleaseDB(db_index);
	return true;
}

void CMy1p1EmuDlg::SetGuildTownData()
{
	for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
	{
		GuildTownData* GuildTown = g_arGuildTownData[i];
		if (!GuildTown || GuildTown->sGuildNum == -1)
		{
			continue;
		}

		for (int j = 0; j < g_arNpcTypeGuild.GetSize(); j++)
		{
			NPC_TYPE_GUILD* GuildNPC = g_arNpcTypeGuild[j];
			if (GuildNPC->sGuild == GuildTown->sTownNum && GuildNPC->sGuildOpt == 1) // Gets the guild town for the town
			{
				CNpc* pNpc = g_arNpc[GuildNPC->sNId];
				if (!pNpc)
				{
					continue;
				}

				pNpc->m_sTownGuildNum = GuildTown->sGuildNum;
				pNpc->m_sTownGuildPicId = GetGuildPictureNum(GuildTown->strGuildName);
				strcpy(pNpc->m_strTownGuildName, GuildTown->strGuildName);
			}
		}
	}
}

short CMy1p1EmuDlg::GetGuildPictureNum(TCHAR* strGuildName)
{
	short sPicId = 0;

	EnterCriticalSection(&(m_Com.m_critGuild));

	CString tempName;
	tempName.Format("%s", strGuildName);
	for (int i = 0; i < g_arGuild.GetSize(); i++)
	{
		if (!tempName.CompareNoCase(g_arGuild[i]->m_strGuildName))
		{
			sPicId = g_arGuild[i]->m_sSymbolVersion;
			break;
		}
	}

	LeaveCriticalSection(&(m_Com.m_critGuild));

	return sPicId;
}

void CMy1p1EmuDlg::OnTimer(UINT nIDEvent)
{
	switch (nIDEvent)
	{
	case GAME_TIME:
		UpdateGameTime();
		break;
	default:
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CMy1p1EmuDlg::UpdateGameTime()
{
	m_nMin++;

	if (m_nMin == 60)
	{
		m_nHour++;
		m_nMin = 0;

		if (m_nHour == 24)
		{
			StoreRestock(true);
		}
		else
		{
			StoreRestock(false);
		}

		StatusGuildTownWar(1);
		StatusGuildTownWar(2);
		StatusClassWar();
		NpcSpawnByGameTime();

		if (m_nHour == 24)
		{
			m_nDay++;
			m_nHour = 0;
		}

		if (m_nDay == 31)
		{
			m_nMonth++;
			m_nDay = 1;
		}

		if (m_nMonth == 13)
		{
			m_nYear++;
			m_nMonth = 1;
		}

		UpdateWeather();
		SetTimeToIni();
		m_Com.SendTimeToAll();
	}
}

void CMy1p1EmuDlg::GetTimeFromIni()
{
	m_nYear = m_Ini.GetProfileInt("TIME", "YEAR", 1);
	m_nMonth = m_Ini.GetProfileInt("TIME", "MONTH", 1);
	m_nDay = m_Ini.GetProfileInt("TIME", "DAY", 1);
	m_nHour = m_Ini.GetProfileInt("TIME", "HOUR", 1);
}

void CMy1p1EmuDlg::SetTimeToIni()
{
	m_Ini.SetProfileInt("TIME", "YEAR", m_nYear);
	m_Ini.SetProfileInt("TIME", "MONTH", m_nMonth);
	m_Ini.SetProfileInt("TIME", "DAY", m_nDay);
	m_Ini.SetProfileInt("TIME", "HOUR", m_nHour);
}

void CMy1p1EmuDlg::UpdateWeather()
{
	int weather = WEATHER_FINE;
	int result = myrand(0, 100);
	if (result < 2)
	{
		weather = WEATHER_RAIN;
	}

	m_nWeather = weather;
}

void CMy1p1EmuDlg::GetClassWarFromIni()
{
	g_iClassWar = m_Ini.GetProfileInt("CLASSWAR", "CLASS", 0);
	g_iClassWarCount = m_Ini.GetProfileInt("CLASSWAR", "COUNT", 0);
	g_iClassWarTime = m_Ini.GetProfileInt("CLASSWAR", "TIME", 0);
	g_iClassWarLimit = m_Ini.GetProfileInt("CLASSWAR", "LIMIT", 60);
	g_iClassWarMinus = m_Ini.GetProfileInt("CLASSWAR", "MINUS", 85);
	g_iClassWarPlusAttack = m_Ini.GetProfileInt("CLASSWAR", "PLUSAT", 0);
	g_iClassWarPlusDefense = m_Ini.GetProfileInt("CLASSWAR", "PLUSDF", 0);
}

void CMy1p1EmuDlg::StatusClassWar()
{
}

void CMy1p1EmuDlg::GetGuildTownWarFromIni()
{
	g_iGuildTownWarStartTime = m_Ini.GetProfileInt("GUILDTOWNWAR", "STARTTIME", 21);
	g_iGuildTownWarEndTime = m_Ini.GetProfileInt("GUILDTOWNWAR", "ENDTIME", 23);
}

void CMy1p1EmuDlg::StatusGuildTownWar(short sTownNum)
{
	// Find the guild town data
	GuildTownData* GuildTown = NULL;
	for (int i = 0; i < g_arGuildTownData.GetSize(); i++)
	{
		if (!g_arGuildTownData[i])
		{
			continue;
		}

		if (g_arGuildTownData[i]->sTownNum == sTownNum)
		{
			GuildTown = g_arGuildTownData[i];
			break;
		}
	}

	if (!GuildTown)
	{
		return;
	}}

	int day;
	if (sTownNum == 1)
	{
		day = 4;
	}
	else if (sTownNum == 2)
	{
		day = 5;
	}

	// Get System time
	SYSTEMTIME systemTime = {0};
	GetSystemTime(&systemTime);

	if ((systemTime.wDay % 6) != day)
	{
		return;
	}

	if (systemTime.wHour < g_iGuildTownWarStartTime)
	{
		CString str;
		if (sTownNum == 1)
		{
			str.Format(IDS_GUILD_TOWN_WAR_TYT_SOON, GuildTown->strGuildName, systemTime.wMonth, systemTime.wDay, g_iGuildTownWarStartTime);
		}
		else if (sTownNum == 2)
		{
			str.Format(IDS_GUILD_TOWN_WAR_IC_SOON, GuildTown->strGuildName, systemTime.wMonth, systemTime.wDay, g_iGuildTownWarStartTime);
		}
		SendAllMessage((LPTSTR)(LPCTSTR)str);
	}
	else if (systemTime.wHour > g_iGuildTownWarStartTime && systemTime.wHour < g_iGuildTownWarEndTime)
	{
		CString str;
		str.Format(IDS_GUILD_TOWN_WAR_IN_PROGRESS, GuildTown->strGuildName);
		SendAllMessage((LPTSTR)(LPCTSTR)str);
	}

	if (systemTime.wHour == g_iGuildTownWarStartTime)
	{
		if (!g_bGuildTownWar[sTownNum-1])
		{
			g_bGuildTownWar[sTownNum-1] = true;
			g_bGuildTownWarStart[sTownNum-1] = true;
			g_bGuildTownWarEnd[sTownNum-1] = false;
		}
	}
	else if (systemTime.wHour == g_iGuildTownWarEndTime)
	{
		if (g_bGuildTownWar[sTownNum-1])
		{
			g_bGuildTownWar[sTownNum-1] = false;
			g_bGuildTownWarStart[sTownNum-1] = false;
			g_bGuildTownWarEnd[sTownNum-1] = true;
		}
	}
}

void CMy1p1EmuDlg::SendAllMessage(TCHAR* pMsg)
{
	CBufferEx TempBuf;
	TempBuf.Add(PKT_CHAT);
	TempBuf.Add(SYSTEM_CHAT);
	TempBuf.AddString(pMsg);

	if (TempBuf.GetLength() <= 0 || TempBuf.GetLength() >= MAX_PACKET_SIZE)
	{
		return;
	}

	SEND_DATA* pNewData = new SEND_DATA;
	if (!pNewData)
	{
		return;
	}

	pNewData->flag = SEND_ALL;
	pNewData->len = TempBuf.GetLength();

	CopyMemory(pNewData->pBuf, TempBuf, TempBuf.GetLength());

	m_Com.Send(pNewData);
	if (pNewData)
	{
		delete pNewData;
	}
}

void CMy1p1EmuDlg::SaveAllUserData()
{
	for (int i = 0; i < MAX_USER; i++)
	{
		USER* pUser = (USER*)m_Iocport.m_SockArray[i];
		if (!pUser)
		{
			continue;
		}

		pUser->Close();
		Sleep(100);
	}
}

void CMy1p1EmuDlg::StoreRestock(bool bFull)
{
	// Restocks quantity of items within stores.
	// There are two types of restock: Full and Minor.
	// Full restocks to 100% (Quantity set to OriginalQuantity)
	// Minor restocks to (Quantity set to Minor Regen Rate(%) of OriginalQuantity)
	for (int i = 0; i < g_arStore.GetSize(); i++)
	{
		for (int j = 0; j < g_arStore[i]->m_arItems.GetSize(); j++)
		{
			StoreItem* s = g_arStore[i]->m_arItems[j];
			if (!s)
			{
				continue;
			}

			if (s->sOriginalQuantity == -1)
			{
				continue;
			}

			if (bFull) // Restocks the items to original quantity
			{
				s->sRegen--;
				if (s->sRegen > 0)
				{
					continue;
				}

				s->sQuantity = s->sOriginalQuantity;
				s->sRegen = s->sOriginalRegen;
			}
			else // Restocks the items to MRegenRate(%) of original quantity
			{
				if (s->sQuantity > 0)
				{
					continue; // We don't restock if there are still items
				}

				s->sMRegen--;
				if (s->sMRegen > 0)
				{
					continue;
				}

				s->sQuantity = (s->sMRegenRate * s->sOriginalQuantity) / 100;
				s->sMRegen = s->sOriginalMRegen;
			}
		}
	}
}

void CMy1p1EmuDlg::NpcSpawnByGameTime()
{
	for (int i = 0; i < g_arNpcTypeTime.GetSize(); i++)
	{
		NPC_TYPE_TIME* n = g_arNpcTypeTime[i];
		if (!n)
		{
			continue;
		}

		if (n->sRegenTime != m_nHour)
		{
			continue;
		}

		if (n->sNId < 0 || n->sNId >= g_arNpc.GetSize())
		{
			continue;
		}

		CNpc* pNpc = g_arNpc[n->sNId];
		if (!pNpc)
		{
			continue;
		}

		if (pNpc->m_NpcState == NPC_DEAD || pNpc->m_NpcState == NPC_LIVE)
		{
			pNpc->m_Delay = 5000;
			pNpc->m_NpcState = NPC_LIVE;
		}
	}
}
