#pragma once
#include "Shared.h"
class ComLib
{
private:
	size_t m_nextSize = 0;
	size_t m_bfrSize = (1 << 20)*10;
	bool m_reverseOne = false;
	bool m_reverse = false;
	bool m_shouldLockConsumer = true;
	bool m_isLocked = false;
	HANDLE m_mainFM;
	HANDLE m_cntrlBfrFM;
	HANDLE m_reverseFM;
	HANDLE m_mutex;
	HANDLE m_semaphore;
	void* m_data = nullptr;
	void* m_controlBufferData = nullptr;
	void* m_reverseData = nullptr;
	char* m_cntrlMsg = nullptr;
	char* m_reverseMsg = nullptr;
	int m_offset = 0;
	int m_ConsumerOffset = 0;
	DWORD m_ms = INFINITE;
	void send(const void* msg, const size_t length, bool increaseOff = true);
	void sendCntrlBfr();
	int recvCntrlBfr();
	void sendReverseBfr();
	void recvReverseBfr();
	bool shouldLockConsumer();
	bool shouldReleaseConsumer();
	void lockConsumer();
	void releaseConsumer();
	void handleCBLogic();
public:
	char* g_clientMsg = nullptr;
	void sendCamera(MAYA_CAM::CAMERA& cam);
	void sendCameraTransform(MAYA_CAM::CAMERA_TRANSFORM& transform);
	void init();
	~ComLib();
};

