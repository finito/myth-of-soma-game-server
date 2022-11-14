#pragma once

#include "IOCPort.h"
#include "Define.h"
#include "SEncryption.h"

#define receives 0
#define sends 1
#define both 2

class CCircularBuffer;

class CIOCPSocket2
{
public:
	void InitSocket(CIOCPort* pIOCPort);
	void Close();
	BOOL AsyncSelect(long lEvent = FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE);
	BOOL SetSockOpt(int nOptionName, const void* lpOptionValue, int nOptionLen, int nLevel = SOL_SOCKET);
	BOOL ShutDown(int nHow = sends);
	BOOL PullOutCore(char *&data, int &length, bool& bEncryption);
	void ReceivedData(int length);
	int  Receive();
	int  Send(char *pBuf, long length, bool bRaw = false, int dwFlag=0);
	BOOL Connect(CIOCPort* pIocp, LPCTSTR lpszHostAddress, UINT nHostPort);
	BOOL Create(UINT nSocketPort = 0,
				 int nSocketType = SOCK_STREAM,
				 long lEvent = FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CONNECT | FD_CLOSE,
				 LPCTSTR lpszSocketAddress = NULL);
	BOOL Accept(SOCKET listensocket, struct sockaddr* addr, int* len);
	int	 GetSocketID() {return m_Sid;};
	void SetSocketID(int sid) { m_Sid = sid;};
	HANDLE GetSocketHandle() {return (HANDLE)m_Socket;};
	BYTE GetState() {return m_State;};
	BYTE GetSockType() {return m_Type;};

	virtual void CloseProcess();
	virtual void Parsing(int len, TCHAR *pBuf, bool& bEncryption);
	virtual void Initialize();

	CIOCPSocket2();
	virtual ~CIOCPSocket2();

	short m_nSocketErr;
	short m_nPending;
	short m_nWouldblock;

protected:
	CIOCPort* m_pIOCPort;
	CCircularBuffer* m_pBuffer;

	SOCKET m_Socket;

	char m_pRecvBuff[SOCKET_BUFF_SIZE];
	char m_pSendBuff[MAX_PACKET_SIZE];

	HANDLE m_hSockEvent;

	OVERLAPPED	m_RecvOverlapped;
	OVERLAPPED	m_SendOverlapped;

	BYTE m_Type;
	BYTE m_State;
	int	m_Sid;
	LPCTSTR m_ConnectAddress;

	int	m_CryptionFlag;
	SEncryption SomaEncryption;
	DWORD m_Rec_val;
	DWORD m_wPacketSerial;
};
