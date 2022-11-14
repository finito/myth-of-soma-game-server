#include "stdafx.h"
#include "SEncryption.h"
#include <time.h>

// Encryption / Decyption

#define	D_WORD_XOR1MUL ((WORD)0x009d)
#define	D_WORD_XOR3BASE	((WORD)0x086d)
#define	D_WORD_XOR3MUL ((WORD)0x087b)
#define	D_XORTABLE_LENGTH 8

SEncryption::SEncryption()
{
	for (int i = 0; i < D_XORTABLE_LENGTH; i++)
	{
		m_Key1[i] = rand() % 255;
		m_Key2[i] = rand() % 255;
	}

	m_Key1[D_XORTABLE_LENGTH] = 0;
	m_Key2[D_XORTABLE_LENGTH] = 0;
	SetKey();
}

void SEncryption::SetKey()
{
	for (int i = 0; i < D_XORTABLE_LENGTH; i++)
	{
		m_Key[i] = m_Key1[i] ^ m_Key2[i];
	}
}

void SEncryption::Encode_Decode(LPBYTE lpTarget, LPBYTE lpSource, WORD nLen)
{
	BYTE l_BYTE_Xor1 = nLen * D_WORD_XOR1MUL;
	WORD l_WORD_Xor3 = D_WORD_XOR3BASE;
	for (WORD nCir= 0; nCir < nLen; nCir++)
	{
		BYTE l_BYTE_Xor3 = HIBYTE(l_WORD_Xor3);
		lpTarget[nCir] = lpSource[nCir] ^ l_BYTE_Xor1 ^ m_Key[nCir % D_XORTABLE_LENGTH] ^ l_BYTE_Xor3;
		l_WORD_Xor3 *= D_WORD_XOR3MUL;
	}
}
