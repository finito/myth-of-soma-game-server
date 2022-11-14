#pragma once

class SEncryption
{
public:
	SEncryption();

	void SetKey();
	void Encode_Decode(LPBYTE lpTarget, LPBYTE lpSource, WORD nLen);

	BYTE m_Key[8];
	BYTE m_Key1[8];
	BYTE m_Key2[8];
};
