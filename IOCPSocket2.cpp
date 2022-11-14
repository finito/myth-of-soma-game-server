#include "stdafx.h"
#include "IOCPSocket2.h"
#include "CircularBuffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CIOCPSocket2::CIOCPSocket2()
{
	m_pBuffer = new CCircularBuffer(SOCKET_BUFF_SIZE);
	m_Socket = INVALID_SOCKET;

	m_pIOCPort = NULL;
	m_Type = TYPE_ACCEPT;

	// Cryption
	m_CryptionFlag = 0;
}

CIOCPSocket2::~CIOCPSocket2()
{
	delete m_pBuffer;
}

BOOL CIOCPSocket2::Create(UINT nSocketPort, int nSocketType, long lEvent, LPCTSTR lpszSocketAddress)
{
	m_Socket = socket(AF_INET, nSocketType/*SOCK_STREAM*/, 0);
	if (m_Socket == INVALID_SOCKET){
		int ret = WSAGetLastError();
		TRACE("Socket Create Fail! - %d\n", ret);
		return FALSE;
	}

	m_hSockEvent = WSACreateEvent();
	if (m_hSockEvent == WSA_INVALID_EVENT){
		int ret = WSAGetLastError();
		TRACE("Event Create Fail! - %d\n", ret);
		return FALSE;
	}

	return TRUE;
}

BOOL CIOCPSocket2::Connect(CIOCPort* pIocp, LPCTSTR lpszHostAddress, UINT nHostPort)
{
	struct sockaddr_in addr;
	memset((void *)&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(lpszHostAddress);
	addr.sin_port = htons(nHostPort);

	int result = connect(m_Socket,(struct sockaddr *)&addr,sizeof(addr));
	if (result == SOCKET_ERROR)
	{
		int err = WSAGetLastError();
//		TRACE("CONNECT FAIL : %d\n", err);
		closesocket(m_Socket);
		return FALSE;
	}

	ASSERT(pIocp);

	InitSocket(pIocp);

	m_Sid = m_pIOCPort->GetClientSid();
	if (m_Sid < 0)
	{
		return FALSE;
	}

	m_pIOCPort->m_ClientSockArray[m_Sid] = this;

	if (!m_pIOCPort->Associate(this, m_pIOCPort->m_hClientIOCPort))
	{
		TRACE("Socket Connecting Fail - Associate\n");
		return FALSE;
	}

	m_ConnectAddress = lpszHostAddress;
	m_State = STATE_CONNECTED;
	m_Type = TYPE_CONNECT;

	Receive();

	return TRUE;
}

int CIOCPSocket2::Send(char* pBuf, long length, bool bRaw, int dwFlag)
{
	if (length > MAX_PACKET_SIZE)
	{
		return 0;
	}

	BYTE pTBuf[MAX_PACKET_SIZE];
	BYTE pTIBuf[MAX_PACKET_SIZE];
	BYTE pTOutBuf[MAX_PACKET_SIZE];
	memset(pTBuf, 0x00, MAX_PACKET_SIZE);
	memset(pTIBuf, 0x00, MAX_PACKET_SIZE);
	memset(pTOutBuf, 0x00, MAX_PACKET_SIZE);
	int index = 0;

	if (!bRaw)
	{
		if (m_CryptionFlag)
		{
			memcpy(pTIBuf, pBuf, length);
			SomaEncryption.Encode_Decode((BYTE*)pTOutBuf, (BYTE*)pTIBuf, static_cast<WORD>(length));

			pTBuf[index++] = (BYTE)PACKET_START1;
			pTBuf[index++] = (BYTE)PACKET_START2;
			memcpy(pTBuf+index, &length, 2);
			index += 2;
			memcpy(pTBuf+index, pTOutBuf, length);
			index += length;
			pTBuf[index++] = (BYTE)PACKET_END1;
			pTBuf[index++] = (BYTE)PACKET_END2;
		}
		else
		{
			pTBuf[index++] = (BYTE)PACKET_START1;
			pTBuf[index++] = (BYTE)PACKET_START2;
			memcpy(pTBuf+index, &length, 2);
			index += 2;
			memcpy(pTBuf+index, pBuf, length);
			index += length;
			pTBuf[index++] = (BYTE)PACKET_END1;
			pTBuf[index++] = (BYTE)PACKET_END2;
		}
	}
	else
	{
		memcpy(pTBuf, pBuf, length);
		index += length;
	}

	WSABUF out;
	out.buf = (char*)pTBuf;
	out.len = index;

	DWORD sent = 0;

	OVERLAPPED* pOvl = &m_SendOverlapped;
	pOvl->Offset = OVL_SEND;
	pOvl->OffsetHigh = out.len;

	int ret_value = WSASend(m_Socket, &out, 1, &sent, dwFlag, pOvl, NULL);

	if (ret_value == SOCKET_ERROR)
	{
		int last_err;
		last_err = WSAGetLastError();

		if (last_err == WSA_IO_PENDING)
		{
			TRACE("SEND : IO_PENDING[SID=%d]\n", m_Sid);
			m_nPending++;
			if (m_nPending > 3)
				goto close_routine;
			sent = length;
		}
		else if (last_err == WSAEWOULDBLOCK)
		{
			TRACE("SEND : WOULDBLOCK[SID=%d]\n", m_Sid);

			m_nWouldblock++;
			if (m_nWouldblock > 3)
				goto close_routine;
			return 0;
		}
		else
		{
			TRACE("SEND : ERROR [SID=%d] - %d\n", m_Sid, last_err);
			m_nSocketErr++;
			goto close_routine;
		}
	}
	else if (!ret_value)
	{
		m_nPending = 0;
		m_nWouldblock = 0;
		m_nSocketErr = 0;
	}

	return sent;

close_routine:
	pOvl = &m_RecvOverlapped;
	pOvl->Offset = OVL_CLOSE;

	HANDLE	hComport = NULL;

	if (m_Type == TYPE_ACCEPT)
	{
		hComport = m_pIOCPort->m_hServerIOCPort;
	}
	else
	{
		hComport = m_pIOCPort->m_hClientIOCPort;
	}

	PostQueuedCompletionStatus(hComport, (DWORD)0, (DWORD)m_Sid, pOvl);

	return -1;
}

int CIOCPSocket2::Receive()
{
	memset(m_pRecvBuff, NULL, MAX_PACKET_SIZE);

	WSABUF in;
	in.len = MAX_PACKET_SIZE;
	in.buf = m_pRecvBuff;
	DWORD insize;
	DWORD dwFlag = 0;
	OVERLAPPED* pOvl = &m_RecvOverlapped;
	pOvl->Offset = OVL_RECEIVE;
	int RetValue = WSARecv(m_Socket, &in, 1, &insize, &dwFlag, pOvl, NULL);

 	if (RetValue == SOCKET_ERROR)
	{
		int last_err;
		last_err = WSAGetLastError();

		if (last_err == WSA_IO_PENDING)
		{
//			TRACE("RECV : IO_PENDING[SID=%d]\n", m_Sid);
//			m_nPending++;
//			if (m_nPending > 3)
//				goto close_routine;
			return 0;
		}
		else if (last_err == WSAEWOULDBLOCK)
		{
			TRACE("RECV : WOULDBLOCK[SID=%d]\n", m_Sid);

			m_nWouldblock++;
			if (m_nWouldblock > 3)
			{
				goto close_routine;
			}
			return 0;
		}
		else
		{
			TRACE("RECV : ERROR [SID=%d] - %d\n", m_Sid, last_err);

			m_nSocketErr++;
			if (m_nSocketErr == 2)
			{
				goto close_routine;
			}
			return -1;
		}
	}

	return (int)insize;

close_routine:
	pOvl = &m_RecvOverlapped;
	pOvl->Offset = OVL_CLOSE;

	HANDLE	hComport = NULL;

	if (m_Type == TYPE_ACCEPT)
	{
		hComport = m_pIOCPort->m_hServerIOCPort;
	}
	else
	{
		hComport = m_pIOCPort->m_hClientIOCPort;
	}

	PostQueuedCompletionStatus(hComport, (DWORD)0, (DWORD)m_Sid, pOvl);

	return -1;
}

void CIOCPSocket2::ReceivedData(int length)
{
	if (length <= 0 || !strlen(m_pRecvBuff))
	{
		return;
	}

	m_pBuffer->PutData(m_pRecvBuff, length);

	char* pData = NULL;
	int len = 0;
	bool bEncryption = false;
	while (PullOutCore(pData, len, bEncryption))
	{
		if (pData)
		{
			Parsing(len, pData, bEncryption);
			delete[] pData;
			pData = NULL;
		}
	}
}

BOOL CIOCPSocket2::PullOutCore(char*& data, int& length, bool& bEncryption)
{
	int len = m_pBuffer->GetValidCount();

	if (len == 0 || len < 0)
	{
		return FALSE;
	}

	BYTE* pTmp = new BYTE[len];

	m_pBuffer->GetData((char*)pTmp, len);

	if (m_CryptionFlag == 1)
	{
		bEncryption = true;
	}

	BOOL foundCore = FALSE;

	int	sPos = 0;
	int ePos = 0;

	for (int i = 0; i < len && !foundCore; i++)
	{
		if (i + 2 >= len)
		{
			break;
		}

		if (pTmp[i] == PACKET_START1 && pTmp[i + 1] == PACKET_START2)
		{
			sPos = i + 2;

			MYSHORT slen;
			slen.b[0] = pTmp[sPos];
			slen.b[1] = pTmp[sPos + 1];

			length = (int)slen.i;

			if (length < 0 || length > len)
			{
				goto cancelRoutine;
			}

			if ((BYTE)pTmp[sPos + 2] == ENCRYPTION_PKT)
			{
				bEncryption = true;
			}

			ePos = sPos+length + 2;

			if ((ePos + 2) > len)goto cancelRoutine;

			if (pTmp[ePos] == PACKET_END1 && pTmp[ePos+1] == PACKET_END2)
			{
				if (m_CryptionFlag)
				{
					BYTE* pBuff = new BYTE[length+1];
					SomaEncryption.Encode_Decode((BYTE *)pBuff, (BYTE *)(pTmp+sPos+2), length);

					data = new char[length+1];
					CopyMemory((void *)data, (const void *)(pBuff), length);
					data[length] = 0;
					foundCore = TRUE;
					int head = m_pBuffer->GetHeadPos(), tail = m_pBuffer->GetTailPos();
					delete[] pBuff;
				}
				else
				{					//
					data = new char[length+1];
					CopyMemory((void *)data, (const void *)(pTmp+sPos+2), length);
					data[length] = 0;
					foundCore = TRUE;
					int head = m_pBuffer->GetHeadPos(), tail = m_pBuffer->GetTailPos();
				}
				break;
			}
			else
			{
				m_pBuffer->HeadIncrease(3);
				break;
			}
		}
	}

	if (foundCore)
	{
		m_pBuffer->HeadIncrease(6+length); //6: header 2+ end 2+ length 2
	}

	delete[] pTmp;
	return foundCore;

cancelRoutine:
	delete[] pTmp;
	return foundCore;
}

BOOL CIOCPSocket2::AsyncSelect(long lEvent)
{
	int retEventResult, err;

	retEventResult = WSAEventSelect(m_Socket, m_hSockEvent, lEvent);
	err = WSAGetLastError();

	return (!retEventResult);
}

BOOL CIOCPSocket2::SetSockOpt(int nOptionName, const void* lpOptionValue, int nOptionLen, int nLevel)
{
	int retValue;
	retValue = setsockopt(m_Socket, nLevel, nOptionName, (char *)lpOptionValue, nOptionLen);
	return (!retValue);
}

BOOL CIOCPSocket2::ShutDown(int nHow)
{
	int retValue;
	retValue = shutdown(m_Socket, nHow);

	return (!retValue);
}

void CIOCPSocket2::Close()
{
	if (!m_pIOCPort)
	{
		return;
	}

	HANDLE	hComport = NULL;
	OVERLAPPED* pOvl;
	pOvl = &m_RecvOverlapped;
	pOvl->Offset = OVL_CLOSE;

	if (m_Type == TYPE_ACCEPT)
	{
		hComport = m_pIOCPort->m_hServerIOCPort;
	}
	else
	{
		hComport = m_pIOCPort->m_hClientIOCPort;
	}

	int retValue = PostQueuedCompletionStatus(hComport, (DWORD)0, (DWORD)m_Sid, pOvl);

	if (!retValue)
	{
		int errValue;
		errValue = GetLastError();
		TRACE("PostQueuedCompletionStatus Error : %d\n", errValue);
	}
}

void CIOCPSocket2::CloseProcess()
{
	m_State = STATE_DISCONNECTED;

	if (m_Socket != INVALID_SOCKET)
	{
		closesocket(m_Socket);
	}
}

void CIOCPSocket2::InitSocket(CIOCPort* pIOCPort)
{
	m_pIOCPort = pIOCPort;
	m_RecvOverlapped.hEvent = NULL;
	m_SendOverlapped.hEvent = NULL;
	m_pBuffer->SetEmpty();
	m_nSocketErr = 0;
	m_nPending = 0;
	m_nWouldblock = 0;

	Initialize();
}

BOOL CIOCPSocket2::Accept(SOCKET listensocket, struct sockaddr* addr, int* len)
{
	m_Socket = accept(listensocket, addr, len);
	if (m_Socket == INVALID_SOCKET)
	{
		int err = WSAGetLastError();
		TRACE("Socket Accepting Fail - %d\n", err);
		return FALSE;
	}

//	int flag = 1;
//	setsockopt(m_Socket, SOL_SOCKET, SO_DONTLINGER, (char *)&flag, sizeof(flag));

//	int lensize, socklen=0;

//	getsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (char*)&socklen, &lensize);
//	TRACE("getsockopt : %d\n", socklen);

//	struct linger lingerOpt;

//	lingerOpt.l_onoff = 1;
//	lingerOpt.l_linger = 0;

//	setsockopt(m_Socket, SOL_SOCKET, SO_LINGER, (char *)&lingerOpt, sizeof(lingerOpt));

	return TRUE;
}

void CIOCPSocket2::Parsing(int len, TCHAR* pBuf, bool& bEncryption)
{

}

void CIOCPSocket2::Initialize()
{
	m_wPacketSerial = 0;
	m_CryptionFlag = 0;
	m_Rec_val = 0;
}
