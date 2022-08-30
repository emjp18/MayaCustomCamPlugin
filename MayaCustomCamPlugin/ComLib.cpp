#include "pch.h"
#include "ComLib.h"

void ComLib::send(const void* msg, const size_t length, bool increaseOff)
{
	m_nextSize = length;
	sendCntrlBfr();
	handleCBLogic();
	memcpy((char*)m_data + m_offset, msg, length);
	if (increaseOff)
	{
		m_offset += (int)length;
	}
	sendCntrlBfr();
}

void ComLib::sendCntrlBfr()
{
	memcpy((char*)m_controlBufferData + sizeof(int), &m_offset, sizeof(int));
}

int ComLib::recvCntrlBfr()
{
	memcpy(m_cntrlMsg, (char*)m_controlBufferData + 0, sizeof(int));
	int consumerOffset = *((int*)m_cntrlMsg);
	return consumerOffset;
}

void ComLib::sendReverseBfr()
{
	if (m_offset + (m_nextSize) >= m_bfrSize)
	{

		m_offset = 0;
		cout << "RESTARTING" << endl;
		m_reverse = true;
		sendCntrlBfr();
		int reverse = 1;
		memcpy((char*)m_reverseData + 0, &reverse, sizeof(int));
	}
}

void ComLib::recvReverseBfr()
{
	if (m_reverse)
	{
		memcpy(m_reverseMsg, m_reverseData, sizeof(int));
		int reverse = *((int*)m_reverseMsg);
		if (reverse == 0)
		{
			m_reverse = false;
		}
	}
}

bool ComLib::shouldLockConsumer()
{
	if (m_reverse)
	{
		return false;
	}
	if (recvCntrlBfr() + (m_nextSize) > m_offset)
	{

		return true;
	}
	return false;
}

bool ComLib::shouldReleaseConsumer()
{
	if (m_reverse)
	{
		return true;
	}
	if (recvCntrlBfr() + (m_nextSize) <= m_offset)
	{

		return true;
	}
	return false;
}

void ComLib::lockConsumer()
{
	DWORD code = WaitForSingleObject(m_mutex, INFINITE);
	if (code == WAIT_ABANDONED)
	{
		cout << "ABANDONED" << endl;
	}
	else if (code == WAIT_OBJECT_0)
	{
		cout << "LCOKED" << endl;
		m_isLocked = true;
	}
	else
	{
		cout << "Someting else" << endl;
	}
}

void ComLib::releaseConsumer()
{
	if (ReleaseMutex(m_mutex))
	{
		m_isLocked = false;
		cout << "RELEASING CONSUMER" << endl;
	}
}

void ComLib::handleCBLogic()
{
	sendReverseBfr();
	recvReverseBfr();

	if (m_reverse)
	{
		if (recvCntrlBfr() <= m_offset + m_nextSize)
		{

			while (1)
			{
				if (recvCntrlBfr() > m_offset + m_nextSize || !m_reverse)
				{
					cout << "Continuing To Write" << endl;
					break;
				}
			}

		}

	}
	/*if (shouldLockConsumer())
	{
		if (!m_isLocked)
		{
			lockConsumer();

		}

	}
	if (shouldReleaseConsumer())
	{
		if (m_isLocked)
		{
			releaseConsumer();

		}

	}*/
}

void ComLib::sendCamera(MAYA_CAM::CAMERA& cam)
{
	MAYA_CAM::HEADER_TYPE type;
	type.type = MAYA_CAM::TYPE::CAMERA;
	send(&type, sizeof(MAYA_CAM::HEADER_TYPE));
	send(&cam, sizeof(MAYA_CAM::CAMERA));
	type.type = MAYA_CAM::TYPE::DEFAULT;
	send(&type, sizeof(MAYA_CAM::HEADER_TYPE), false);
}

void ComLib::sendCameraTransform(MAYA_CAM::CAMERA_TRANSFORM& transform)
{
	MAYA_CAM::HEADER_TYPE type;
	type.type = MAYA_CAM::TYPE::CAMERA_TRANSFORM;
	send(&type, sizeof(MAYA_CAM::HEADER_TYPE));
	send(&transform, sizeof(MAYA_CAM::CAMERA_TRANSFORM));
	type.type = MAYA_CAM::TYPE::DEFAULT;
	send(&type, sizeof(MAYA_CAM::HEADER_TYPE), false);
}

void ComLib::init()
{
	m_cntrlMsg = new char[8];
	m_reverseMsg = new char[4];
	g_clientMsg = new char[100];
	m_mainFM = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		(DWORD)0,
		(DWORD)m_bfrSize,
		(LPCWSTR)"myFileMap");



	m_data = MapViewOfFile(m_mainFM, FILE_MAP_ALL_ACCESS, 0, 0, m_bfrSize);


	m_cntrlBfrFM = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		(DWORD)0,
		(DWORD)sizeof(int) * 2,
		(LPCWSTR)"myFileMap2");




	m_controlBufferData = MapViewOfFile(m_cntrlBfrFM, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(int) * 2);



	m_reverseFM = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		(DWORD)0,
		(DWORD)sizeof(int),
		(LPCWSTR)"myFileMap3");


	

	m_reverseData = MapViewOfFile(m_reverseFM, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(int));



	m_mutex = CreateMutex(nullptr, false, L"myMutex");
}

ComLib::~ComLib()
{
	UnmapViewOfFile((LPCVOID)m_data);
	CloseHandle(m_mainFM);

	UnmapViewOfFile((LPCVOID)m_controlBufferData);
	CloseHandle(m_cntrlBfrFM);

	UnmapViewOfFile((LPCVOID)m_reverseData);

	CloseHandle(m_reverseFM);

	delete[] m_cntrlMsg;
	delete[] m_reverseMsg;
	delete[] g_clientMsg;
}
