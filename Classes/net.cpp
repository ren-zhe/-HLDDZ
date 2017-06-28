#include "net.h"
#include<iostream>
#include<thread>
Net* Net::m_net = NULL;

bool Net::makeClient()
{
	if (serverIP.empty())
		return false;
	if (connectSocket != NULL)
		return false;
	connectSocket = new ODSocket();
	if (connectSocket == NULL)
	{
		//OutputDebugString(L"new error\n");
		return false;
	}
	if (connectSocket->Init() == -1)
	{
		//OutputDebugString(L"Init error\n");
		return false;
	}
	if (!connectSocket->Create(AF_INET, SOCK_STREAM, 0))
	{
		//OutputDebugString(L"Create error\n");
		return false;
	}
	/*if (!connectSocket->Connect(temp.c_str(), DEFAULTPORT))
	{
	log("Connect error1");
	return;
	}*/
	std::string temp = serverIP;
	//thread
	std::thread cr(&Net::clientRecv, this, temp);
	cr.detach();
	return true;
}
void Net::clientRecv(std::string temp)
{
	while (!connect)
	{
		if (Net::getInstance()->exit)
			return;
		if (connectSocket->Connect(temp.c_str(), DEFAULTPORT))
		{
			connect = true;
			//	OutputDebugString(L"connect ok\n");
		}
		//OutputDebugString(L"send999\n");
		#ifdef WIN32 
			Sleep(300);
		#else
			usleep(300);
		#endif
	}
	OutputDebugStringA("------------------connect ok---------------");
	fd_set fdRead, fdWrite, fdExcept;
	SOCKET sd = *connectSocket;
	//clientList.push_back(new Connection(sd));
	clientConnection = new Connection(sd);
	if (sd > maxfd)
		maxfd = sd;
	while (true)
	{
		if (Net::getInstance()->exit)
			return;
		if (!connectSocket)
			break;
		/*	OutputDebugString(L"circle\n");*/
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExcept);
		if (clientConnection->recvBytes < BUFFERSIZE)
			FD_SET(clientConnection->hSocket, &fdRead);
		if (clientConnection->sendBytes)
			FD_SET(clientConnection->hSocket, &fdWrite);
		FD_SET(clientConnection->hSocket, &fdExcept);
		/*ConnectionList::const_iterator it = clientList.begin();
		if ((*it)->recvBytes < BUFFERSIZE)
		{
			FD_SET((*it)->hSocket, &fdRead);
		}
		if ((*it)->sendBytes > 0)
		{
			FD_SET((*it)->hSocket, &fdWrite);
		}
		FD_SET((*it)->hSocket, &fdExcept);*/
		timeval time{ 0, 300000 };
#ifdef WIN32
		int ret = select(0, &fdRead, &fdWrite, &fdExcept, &time);
#else   
		int ret = select(maxfd + 1, &fdRead, &fdWrite, &fdExcept, &time);
#endif
		/*OutputDebugString(L"after select\n");*/
		if (ret < 0)
		{
			//OutputDebugString(L"select error\n");
			//std::cout << "select error" << WSAGetLastError() << std::endl;
			break;
		}
		else if (ret == 0)
			continue;
		CheckConn(fdRead, fdWrite, fdExcept, clientConnection);
	}
}

void Net::CheckConn(const fd_set & fdRead, const fd_set & fdWrite, const fd_set & fdExcept, Connection * conn)
{
	bool ok = true;
	if (FD_ISSET(conn->hSocket, &fdExcept))
	{
		ok = false;
		int lastErr;
		socklen_t errlen = sizeof(lastErr);
		getsockopt(conn->hSocket, SOL_SOCKET, SO_ERROR, (char*)&lastErr, &errlen);
		//OutputDebugString(L"I/O error\n");
		std::cout << "I/O error" << lastErr << std::endl;
	}
	else
	{
		if (FD_ISSET(conn->hSocket, &fdRead))
		{
			ok = TryRead(conn);
		}
		else if (FD_ISSET(conn->hSocket, &fdWrite))
		{
			ok = TryWrite(conn);
		}
	}
	if (!ok)
	{
		//closesocket(pConn->hSocket);
		delete conn;
	}
}
bool Net::TryRead(Connection * pConn)
{
	//OutputDebugString(L"read enter\n");
	int ret = recv(pConn->hSocket, pConn->recvBuffer + pConn->recvBytes, BUFFERSIZE - pConn->recvBytes, 0);
	if (ret > 0)
	{
		//OutputDebugString(L"read words;\n");
		//OutputDebugStringA(pConn->recvBuffer);
		pConn->recvBytes += ret;
		pConn->recvBuffer[pConn->recvBytes] = '\0';
		manageStr(pConn->recvBuffer, true);
		pConn->recvBytes -= ret;
		pConn->recvBuffer[pConn->recvBytes] = '\0';
		return true;
	}
	else if (ret == 0)
	{
		connect = false;
		//OutputDebugString(L"connection close by peer\n");
		std::cout << "connection close by peer" << std::endl;
		PassiveShutdown(pConn->hSocket, pConn->recvBuffer, pConn->recvBytes);
		return false;
	}
	else
	{
		return false;
	}
}
bool Net::TryWrite(Connection * pConn)
{
	//OutputDebugString(L"write enter\n");
	//m_mutex.lock();
	if (pConn == clientConnection)
		m_mutex2.lock();
	int ret = send(pConn->hSocket, pConn->sendBuffer, pConn->sendBytes, 0);
	if (pConn == clientConnection)
	{
		/*OutputDebugStringA("client send(");
		OutputDebugStringA(pConn->sendBuffer);
		OutputDebugStringA(")");*/
	}
	else
	{
		/*OutputDebugStringA("server send(");
		OutputDebugStringA(pConn->sendBuffer);
		OutputDebugStringA(")");*/
	}
	if (ret > 0)
	{
		//OutputDebugString(L"write words;\n");
		pConn->sendBytes -= ret;
		
		if (pConn->sendBytes > 0)
		{
		memmove(pConn->sendBuffer, pConn->sendBuffer + ret, pConn->sendBytes);
		}
		else
		{
		pConn->sendBuffer[0] = '\0';
		}
		if (pConn == clientConnection)
			m_mutex2.unlock();
		return true;
	}
	else if (ret == 0)
	{
		//connect = false;
		//OutputDebugString(L"connection close by peer\n");
		std::cout << "connection close by peer" << std::endl;
		PassiveShutdown(pConn->hSocket, pConn->sendBuffer, pConn->sendBytes);
		if (pConn == clientConnection)
			m_mutex2.unlock();
		return false;
	}
	else
	{
		/*int lastErr = WSAGetLastError();

		if (lastErr == WSAEWOULDBLOCK)

		{

		return true;

		}

		OutputDebugString(L"send error\n");

		std::cout << "send error" << lastErr << std::endl;*/
		if (pConn == clientConnection)
			m_mutex2.unlock();
		return false;
	}
}
bool Net::PassiveShutdown(SOCKET sd, const char * buff, int len)
{
	if (buff != NULL && len > 0)
	{
#ifdef WIN32
		u_long noBlock = 0;
		if (ioctlsocket(sd, FIONBIO, &noBlock) == SOCKET_ERROR)
		{
			OutputDebugString(L"ioctlsocket error\n");
			std::cout << "ioctlsocket error" << WSAGetLastError() << std::endl;
			return false;
		}
#else
		int flags = fcntl(sd, F_GETFL, 0);                        //获取文件的flags值。
		fcntl(sd, F_SETFL, flags | O_NONBLOCK);   //设置成非阻塞模式；
#endif
		int nSend = 0;
		while (nSend < len)
		{
			int temp = send(sd, &buff[nSend], len - nSend, 0);
			if (temp > 0)
			{
				nSend += temp;
			}
			else if (temp == SOCKET_ERROR)
			{
				//OutputDebugString(L"send error\n");
				//	std::cout << "send error" << WSAGetLastError() << std::endl;
				return false;
			}
			else
			{
				//OutputDebugString(L"Connection closed unexceptedly by peer\n");
				std::cout << "Connection closed unexceptedly by peer" << std::endl;
				break;
			}
		}
	}
	if (shutdown(sd, 1) == SOCKET_ERROR)
	{
		//OutputDebugString(L"shutdown error\n");
		//std::cout << "shutdown error" << WSAGetLastError() << std::endl;
		return false;
	}
	return true;
}
bool Net::createUdpSocket()
{
	if (udpSocket == NULL)
	{
		udpSocket = new ODSocket();
		if (udpSocket == NULL)
		{
			OutputDebugString(L"udp create error\n");
			return false;
		}
		if (udpSocket->Init() == -1)
		{
			OutputDebugString(L"udp init error\n");
			return false;
		}
		if (!udpSocket->Create(AF_INET, SOCK_DGRAM, 0))
		{
			OutputDebugString(L"create udp error\n");
			return false;
		}
		if (!udpSocket->Bind(PORTA))
		{
		OutputDebugString(L"bind error\n");
			return false;
		}
		if (!udpSocket->setBroadcast())
		{
			OutputDebugString(L"set broadcast error\n");
			return false;
		}
	}
	return true;//cun zai huozhe bucunzai
}

void Net::sendStringC(std::string temp)
{
	m_mutex2.lock();
	for (int i = 0; i < temp.size(); i++)
	{
		clientConnection->sendBuffer[clientConnection->sendBytes++] = temp[i];
	}
	clientConnection->sendBuffer[clientConnection->sendBytes] = '\0';
	m_mutex2.unlock();
}

void Net::manageStr(std::string temp, bool isClient)
{
	/*OutputDebugStringA("manage str ( ");
	OutputDebugStringA(temp.c_str());
	OutputDebugStringA(")");*/
	std::string t;
	while (!temp.empty())
	{
		/*if(temp.size() == 4)
		OutputDebugStringA("4");*/
		if (temp[0] >= 'a' && temp[0] <= 'z')
		{
			int i;
			for (i = 1; i < temp.size();)
			{
				if (temp[i] >= '0' && temp[i] <= '1' + 15)
					i++;
				else if (temp[i] == '#')
				{
					i++;
					break;
				}
				else
				{
					OutputDebugStringA("error");
					break;
				}
			}
			t = temp.substr(0, i);
			if (isClient)
			{
				m_mutex.lock();
				clientStr.push_back(t);
				m_mutex.unlock();
			}
			temp = temp.substr(i);
		}
		else
		{		
			OutputDebugStringA("string error");	
			break;
		}
	}
}
void Net::DeleteSocket()
{
	if (connectSocket)
	{
		delete connectSocket;
		connectSocket = NULL;
	}
	connect = false;
	serverIP.clear();
	clientStr.clear();
	maxfd = -1;
	exit = false;
}
