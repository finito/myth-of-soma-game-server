#include "stdafx.h"
#include "MAP.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

MapInfo::MapInfo()
{
	m_bMove	= MAP_NON_MOVEABLE;
	m_lUser	= 0;
	m_nAreaNumber = 0;
}

MapInfo::~MapInfo()
{
}

MoveInfo::MoveInfo()
{
	m_bMovable = 0;
	m_nAreaNumber = 0;
}

MoveInfo::~MoveInfo()
{
}

MoveCell::MoveCell()
{
	m_vDim.cx = 0;
	m_vDim.cy = 0;
	m_pMoveInfo = NULL;
}

MoveCell::~MoveCell()
{
	RemoveData();
}

void MoveCell::RemoveData()
{
	if (m_pMoveInfo)
	{
		for (int i = 0; i < m_vDim.cx; i++)
		{
			delete[] m_pMoveInfo[i];
		}
		delete[] m_pMoveInfo;
		m_pMoveInfo = NULL;
	}
}

void MoveCell::Initialize(int cx, int cy)
{
	RemoveData();

	m_vDim.cx = cx;
	m_vDim.cy = cy;

	m_pMoveInfo = new MoveInfo *[m_vDim.cx];
	for (int i = 0; i < m_vDim.cx; i++)
	{
		m_pMoveInfo[i] = new MoveInfo[m_vDim.cy];
	}
}

void MoveCell::Load(CArchive& ar)
{
	BYTE BitMask[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

	int cx, cy, roll;
	BYTE *t_buf;
	CPoint cell_limit;
	int load_count, read_count, tmp_int;
	int x, y, i;

	ar >> cx >> cy;

	Initialize(cx, cy);

	cell_limit.x = (cx+1)/2;
	cell_limit.y = (cy+1)/2;
	tmp_int = cell_limit.x*cell_limit.y+(cell_limit.x-1)*(cell_limit.y-1);
	load_count = tmp_int / 8;
	if (tmp_int % 8) load_count++;
	t_buf = new BYTE[load_count];
	roll = 0;
	read_count = 0;
	ar.Read(t_buf, load_count);
	for (y = 0; y < m_vDim.cy-1; y++)
	{
		for (x = 0; x < m_vDim.cx-1; x++)
		{
			if ((x+y)%2==0)
			{
				m_pMoveInfo[x][y].m_bMovable = (t_buf[read_count] & BitMask[roll]) >> roll;
				roll++;
				if (roll == 8)
				{
					roll = 0;
					read_count++;
				}
			}
		}
	}
	delete[] t_buf;

	short data_x, data_y, data;
	ar >> load_count;
	for (i = 0; i < load_count; i++)
	{
		ar >> data_x;
		ar >> data_y;
		ar >> data;
	}

	ar >> load_count;
	for (i = 0; i < load_count; i++)
	{
		ar >> data_x;
		ar >> data_y;
		ar >> data;
	}

	cell_limit.x = (cx+1)/2;
	cell_limit.y = (cy+1)/2;
	load_count = cell_limit.x*cell_limit.y+(cell_limit.x-1)*(cell_limit.y-1);
	t_buf = new BYTE[load_count];
	read_count = 0;
	ar.Read(t_buf, load_count);
	for (y = 0; y < m_vDim.cy-1; y++)
	{
		for (x = 0; x < m_vDim.cx-1; x++)
		{
			if ((x+y)%2==0)
			{
				m_pMoveInfo[x][y].m_nAreaNumber = t_buf[read_count++];
			}
		}
	}
	delete[] t_buf;
}

void MoveCell::LoadSize(CArchive &ar)
{
	int cx, cy;
	CPoint cell_limit;

	ar >> cx >> cy;

	RemoveData();

	m_vDim.cx = cx;
	m_vDim.cy = cy;
}

void MoveCell::ConvertMap(MapInfo **tmap)
{
	int tempx, tempy;
	int temph = m_vDim.cy / 2 - 1;

	for (int y = 0; y < m_vDim.cy - 1; y++)
	{
		for (int x = 0; x < m_vDim.cx - 1; x++)
		{
			if ((x+y)%2 == 0)
			{
				tempx = temph - (y / 2)+ (x / 2);

				if (x % 2) tempy = (y / 2)+ ((x / 2)+ 1);
				else tempy = (y / 2)+ (x / 2);

				tmap[tempx][tempy].m_bMove = m_pMoveInfo[x][y].m_bMovable;
				tmap[tempx][tempy].m_nAreaNumber = m_pMoveInfo[x][y].m_nAreaNumber;
				tmap[tempx][tempy].iIndex = -1;
				tmap[tempx][tempy].m_lUser = 0;
				tmap[tempx][tempy].m_FieldUse = 0;
			}
		}
	}
}

MAP::MAP()
{
	m_Zone = 1;
	m_pMap = NULL;
}

MAP::~MAP()
{
	RemoveMapData();
}

void MAP::RemoveMapData()
{
	int i;

	if (m_pMap)
	{
		for (i = 0; i < m_sizeMap.cx; i++)
		{
			delete[] m_pMap[i];
		}

		delete[] m_pMap;
	}
}

BOOL MAP::LoadMapByNum(int num)
{
	char filename[128];
	wsprintf(filename, ".\\MAP\\QvMapE%d.imf", num);

	if (!LoadImf(filename)) return FALSE;

	m_Zone = num;

	return TRUE;
}

BOOL MAP::LoadMapSizeByNum(int num)
{
	char filename[128];
	wsprintf(filename, ".\\MAP\\QvMapE%d.imf", num);

	if (!LoadImfSize(filename)) return FALSE;

	m_Zone = num;

	return TRUE;
}

BOOL MAP::LoadImf(char *szFileName)
{
	CFile	newfile;
	int		nVersionInfo;

	if (newfile.Open(szFileName, CFile::modeRead))
	{
		CArchive newar(&newfile, CArchive::load);
		newar >> nVersionInfo;
		m_vMoveCell.Load(newar);
		newar.Close();
		newfile.Close();

		InitTargetMap(m_vMoveCell.m_vDim.cx, m_vMoveCell.m_vDim.cy);
		m_vMoveCell.ConvertMap(m_pMap);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

BOOL MAP::LoadImfSize(char *szFileName)
{
	CFile	newfile;
	int		nVersionInfo;

	if (newfile.Open(szFileName, CFile::modeRead))
	{
		CArchive newar(&newfile, CArchive::load);
		newar >> nVersionInfo;
		m_vMoveCell.LoadSize(newar);
		newar.Close();
		newfile.Close();
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

void MAP::InitTargetMap(int cx, int cy)
{
	int wh = (cx+1)/2 + (cy+1)/2 - 1;
	m_pMap = new MapInfo*[wh];

	for (int i = 0; i < wh; i++)
	{
		m_pMap[i] = new MapInfo[wh];
	}

	for (int i = 0; i < wh; i++)
	{
		for (int j = 0; j < wh; j++)
		{
			m_pMap[j][i].m_bMove = MAP_MOVEABLE;
			m_pMap[j][i].m_lUser = 0;
			m_pMap[j][i].m_nAreaNumber = 0;
			m_pMap[j][i].iIndex = -1;
			m_pMap[j][i].m_FieldUse = 0;
		}
	}

	TRACE("Size Of MapInfo - %d\n", sizeof(MapInfo));
	TRACE("Size Of MAP - %d\n", sizeof(MapInfo)* wh * wh);

	m_sizeMap.cx = m_sizeMap.cy = wh;
}
