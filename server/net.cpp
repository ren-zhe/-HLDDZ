#include "net.h"
#include<iostream>
#include<thread>
Net* Net::m_net = NULL;
bool Net::makeServer()
{
	if (connectSocket != NULL)
		return false;
	if (listenSocket)
		return false;
	listenSocket = new ODSocket();
	if (!listenSocket)
	{
		//OutputDebugString(L"new error\n");
		return false;
	}
	if (listenSocket->Init() == -1)
	{
		//OutputDebugString(L"Init error\n");
		return false;
	}
	if (!listenSocket->Create(AF_INET, SOCK_STREAM, 0))
	{
		//OutputDebugString(L"Create listensocket error\n");
		return false;
	}
	int port = DEFAULTPORT;
	if (!listenSocket->Bind(port))
	{
		//OutputDebugString(L"Bind error\n");
		return false;
	}
	if (!listenSocket->Listen())
	{
		//OutputDebugString(L"Listen error\n");
		return false;
	}
	std::thread acceptThread(&Net::accept, this);
	acceptThread.join();
	return true;
}

void Net::accept()
{
	fd_set fdRead, fdWrite, fdExcept;
	SOCKET sdListen = *listenSocket;
	if (maxfd < sdListen)
		maxfd = sdListen;
	while (true)
	{
		if (Net::getInstance()->exit)
			return;
		if (!listenSocket)
		{
			//OutputDebugStringA("accept quit");
			break;
		}
		/*if (clean)
		{
		closesocket(sdListen);
		}*/
		ResetFDSet(fdRead, fdWrite, fdExcept, sdListen, clientList);
		timeval time = { 0, 300000};
#ifdef WIN32
		int ret = select(0, &fdRead, &fdWrite, &fdExcept, &time);
#else
		int ret = select(maxfd + 1, &fdRead, &fdWrite, &fdExcept, &time);
#endif
		if (ret < 0)
		{
			//OutputDebugString(L"select error\n");
			//	std::cout << "select error" << WSAGetLastError() << std::endl;
			break;
		}
		else if (ret == 0)
			continue;
		ret = CheckAccept(fdRead, fdExcept, sdListen, clientList);
		if (ret == SOCKET_ERROR)
		{
			//OutputDebugString(L"checkaccept error\n");
		break;
		}
		CheckConn(fdRead, fdWrite, fdExcept, clientList);
	}
	/*connectSocket = new ODSocket();
	if (!connectSocket)
	{
	log("new error");
	return;
	}
	if (connectSocket->Init() == -1)
	{

	log("Init error");

	return;

	}

	if (!connectSocket->Create(AF_INET, SOCK_STREAM, 0))

	{

	log("Create error");

	return;

	}

	if (!listenSocket->Accept(*connectSocket))

	{

	log("Accept error.");

	return;

	}*/

}

void Net::ResetFDSet(fd_set & fdRead, fd_set & fdWrite, fd_set & fdExcept, SOCKET sdListen, const ConnectionList & conns)
{
	FD_ZERO(&fdRead);
	FD_ZERO(&fdWrite);
	FD_ZERO(&fdExcept);
	FD_SET(sdListen, &fdRead);
	FD_SET(sdListen, &fdExcept);
	ConnectionList::const_iterator it = conns.begin();
	for (; it != conns.end(); ++it)
	{
		Connection* pConn = *it;
		if (pConn->recvBytes < BUFFERSIZE)
		{
			FD_SET(pConn->hSocket, &fdRead);
		}
		if (pConn->sendBytes > 0)
		{
			FD_SET(pConn->hSocket, &fdWrite);
		}
		FD_SET(pConn->hSocket, &fdExcept);
	}
}
int Net::CheckAccept(const fd_set & fdRead, const fd_set & fdExcept, SOCKET sdListen, ConnectionList & conns)
{
	int lastErr = 0;
	if (FD_ISSET(sdListen, &fdExcept))//sdListen 异常
	{
		socklen_t errlen = sizeof(lastErr);
		getsockopt(sdListen, SOL_SOCKET, SO_ERROR, (char*)&lastErr, &errlen);
		//OutputDebugString(L"I/O error\n");
		//log(Value(lastErr).asString().c_str());
		//std::cout << "I/O error" << lastErr << std::endl;
		return SOCKET_ERROR;
	}
	if (FD_ISSET(sdListen, &fdRead))//可以accept
	{
		printf("--------------------------line-----------");
		//OutputDebugString(L"accept ok\n");
		if (conns.size() >= FD_SETSIZE - 1)
		{
			return 0;
		}
		sockaddr_in client;
		socklen_t size = sizeof(sockaddr_in);
		SOCKET sd = ::accept(sdListen, (sockaddr*)&client, &size);
		//lastErr = WSAGetLastError();
		if (sd == INVALID_SOCKET)//&& lastErr != WSAEWOULDBLOCK
		{
			//OutputDebugString(L"accept error\n");
			//std::cout << "accept error" << lastErr << std::endl;
			return SOCKET_ERROR;
		}
		if (sd != INVALID_SOCKET)
		{
			connect = true;
			if (maxfd < sd)
				maxfd = sd;
#ifdef WIN32
			u_long noBlock = 1;
		    if (ioctlsocket(sd, FIONBIO, &noBlock) == SOCKET_ERROR)
			{
				OutputDebugString(L"ioctlsocket error\n");
				std::cout << "ioctlsocket error" << WSAGetLastError() << std::endl;
				return SOCKET_ERROR;
			}
#else
			int flags = fcntl(sd, F_GETFL, 0);                        //获取文件的flags值。
			fcntl(sd, F_SETFL, flags | O_NONBLOCK);   //设置成非阻塞模式；
#endif
			if (!discardCards)
			{
				m_mutex1.lock();
				conns.push_back(new Connection(sd));

				conns[conns.size() - 1]->sendBuffer[conns[conns.size() - 1]->sendBytes++] = 't';//向连接上的玩家发送连接编号
				conns[conns.size() - 1]->sendBuffer[conns[conns.size() - 1]->sendBytes++] = '0' + conns.size();
				conns[conns.size() - 1]->sendBuffer[conns[conns.size() - 1]->sendBytes++] = '#';
				for (int i = 0; i < conns.size() - 1; i++)
				{
					conns[i]->sendBuffer[conns[i]->sendBytes++] = 'l';//向已经连接的玩家发送新玩家连接上的通知
					conns[i]->sendBuffer[conns[i]->sendBytes++] = '0' + conns.size();
					conns[i]->sendBuffer[conns[i]->sendBytes++] = '#';
					conns[conns.size() - 1]->sendBuffer[conns[conns.size() - 1]->sendBytes++] = 'l';//向这个玩家发送已连接玩家的连接信息
					conns[conns.size() - 1]->sendBuffer[conns[conns.size() - 1]->sendBytes++] = '1' + i;
					conns[conns.size() - 1]->sendBuffer[conns[conns.size() - 1]->sendBytes++] = '#';
					for (int j = 0; j < readyPlayer.size(); j++)
					{
						conns[conns.size() - 1]->sendBuffer[conns[conns.size() - 1]->sendBytes++] = 'r';
						conns[conns.size() - 1]->sendBuffer[conns[conns.size() - 1]->sendBytes++] = '0' + readyPlayer[j];
						conns[conns.size() - 1]->sendBuffer[conns[conns.size() - 1]->sendBytes++] = '#';
					}
				}
				for (int i = 0; i < conns.size(); i++)
				{
					conns[i]->sendBuffer[conns[i]->sendBytes] = '\0';
				}
				m_mutex1.unlock();
				//if (conns.size() == 3)
				//{
				//	cardManage();
				//	discardCards = true;
				//	//exit = true;
				//}
			}
		/*	if (conns.size() == 3)
			{
				readyGame();
			}*/
		}
	}
	return 0;
}
void Net::CheckConn(const fd_set & fdRead, const fd_set & fdWrite, const fd_set & fdExcept, ConnectionList & conns)
{
	//OutputDebugString(L"chekconn\n");
	ConnectionList::iterator it = conns.begin();
	while (it != conns.end())
	{
		Connection* pConn = *it;
		bool ok = true;
		if (FD_ISSET(pConn->hSocket, &fdExcept))
		{
			ok = false;
			int lastErr;
			socklen_t errlen = sizeof(lastErr);
			getsockopt(pConn->hSocket, SOL_SOCKET, SO_ERROR, (char*)&lastErr, &errlen);
			//OutputDebugString(L"I/O error\n");
			std::cout << "I/O error" << lastErr << std::endl;
		}
		else
		{
			if (FD_ISSET(pConn->hSocket, &fdRead))
			{
				ok = TryRead(pConn);
			}
			else if (FD_ISSET(pConn->hSocket, &fdWrite))
			{
				ok = TryWrite(pConn);
			}
		}
		if (!ok)
		{
			//closesocket(pConn->hSocket);
			delete pConn;
			it = conns.erase(it);
		}
		else
			++it;
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
		/*m_mutex.lock();*/
		if (pConn == clientConnection)
		{
			//clientStr.push_back(pConn->recvBuffer);
			std::cout << pConn->recvBuffer << std::endl;
			manageStr(pConn->recvBuffer, true);
			//OutputDebugStringA(pConn->recvBuffer);
		//	OutputDebugStringA("jieshou\n");
		}
		else
		{
			std::cout << pConn->recvBuffer << std::endl;
			/*OutputDebugStringA("manage str ( ");
			OutputDebugStringA(pConn->recvBuffer);
			OutputDebugStringA(")");*/
			//serverStr.push_back(pConn->recvBuffer);
			manageStr(pConn->recvBuffer, false);
		}
	/*	m_mutex.unlock();*/
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
	else
		m_mutex1.lock();
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
		else
			m_mutex1.unlock();
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
		else
			m_mutex1.unlock();
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
		else
			m_mutex1.unlock();
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

void Net::cardManage()
{
	//创建扑克 洗牌，分牌
	makeCards();
	shuffleCards();
	giveCards();
}
void Net::makeCards()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 1; j < 14; j++)
		{
			allCards.push_back(puke(i, j));
		}
	}
	allCards.push_back(puke(4, 14));
	allCards.push_back(puke(4, 15));
}
void Net::shuffleCards()
{
	srand((unsigned int)time(NULL));
	for (int i = 0; i < allCards.size(); i++)
	{
		int t1 = rand() % allCards.size();
		int t2 = rand() % allCards.size();
		puke temp = allCards[t1];
		allCards[t1] = allCards[t2];
		allCards[t2] = temp;
	}
}
void Net::giveCards()
{
	srand((unsigned int)time(NULL));
	std::string tem;
	for (int i = 1; i <= 3; i++)
	{
		tem += 'f';
		tem += '0';
		for (int j = 0; j < 17; j++)
		{
			int num = allCards[(i - 1) * 17 + j].num;
			int type = allCards[(i - 1) * 17 + j].type;
			tem += '0' + type;
			tem += '0' + num;
			playerCards[i - 1].push_back(puke(type, num));
		}
		tem += '#';
		sendStringTo(tem, i);
		tem.clear();
	}
	for (int i = 0; i < 3; i++)
		rules.sortPukes(playerCards[i]);
	
}
void Net::giveCardsA()
{
	srand((unsigned int)time(NULL));
	std::string tem;
	int num = rand() % 3;
	for (int i = 1; i <= 3; i++)
	{	
		tem += 'f';
		tem += '0';
		tem += '1'+ num;//指定第一个玩家
		for (int j = 0; j < 17; j++)
		{
			int num = allCards[(i - 1) * 17 + j].num;
			int type = allCards[(i - 1) * 17 + j].type;
			tem += '0' + type;
			tem += '0' + num;
			playerCards[i - 1].push_back(puke(type, num));
		}
		tem += '#';
		sendStringTo(tem, i);
		/*OutputDebugStringA("chongfa:(");
		OutputDebugStringA(tem.c_str());
		OutputDebugStringA(")\n");*/
		tem.clear();
	}
}
void Net::manageServerMessage(std::string temp)
{
	/*OutputDebugStringA("server manage message (");
	OutputDebugStringA(temp.c_str());
	OutputDebugStringA(")");*/
	if (temp.empty())
	{
		OutputDebugStringA("server message empty\n");
		return;
	}
	if (temp[0] == 'q')
	{
		readyNum++;
		if (readyNum == 3)//可以开始游戏
		{
			//OutputDebugStringA("start game\n");
			m_mutex1.lock();
			//srand((unsigned int)time(NULL));
			int num = rand() % 3;
			for (int i = 0; i < clientList.size(); i++)
			{
				clientList[i]->sendBuffer[clientList[i]->sendBytes++] = 's';
				clientList[i]->sendBuffer[clientList[i]->sendBytes++] = '1' + num; 
				clientList[i]->sendBuffer[clientList[i]->sendBytes++] = '#';
				clientList[i]->sendBuffer[clientList[i]->sendBytes] = '\0';
			}
			m_mutex1.unlock();
		}
	}
	else if (temp[0] == 'r')
	{
		sendStringS(temp);
		readyPlayer.push_back(temp[1] - '0');
		if (readyPlayer.size() == 3)
		{
			cardManage();
		}
	}
	else if (temp[0] == 'j')
	{
		manageLandlord(temp);
	}
	else if (temp[0] == 'c')
	{
		managePuke(temp);
	}
	else if (temp[0] == 't')
	{
		manageTimeOut(temp);
	}
	else if (temp[0] == 'z')
	{
		manageTuoGuan(temp);
	}
}

void Net::sendStringS(std::string temp)
{
	m_mutex1.lock();
	for (int i = 0; i < clientList.size(); i++)
	{
		for (int j = 0; j < temp.size(); j++)
		{
			clientList[i]->sendBuffer[clientList[i]->sendBytes++] = temp[j];
		}
		clientList[i]->sendBuffer[clientList[i]->sendBytes] = '\0';
	}
	m_mutex1.unlock();
}
void Net::sendStringTo(std::string temp, int num)
{	
	m_mutex1.lock();
	for (int i = 0; i < temp.size(); i++)
	{
		clientList[num - 1]->sendBuffer[clientList[num - 1]->sendBytes++] = temp[i];
	}
	clientList[num - 1]->sendBuffer[clientList[num - 1]->sendBytes] = '\0';
	m_mutex1.unlock();
}
void Net::manageLandlord(std::string temp)
{
	Turn turn = static_cast<Turn>(temp[1] - '0');//可作为局部变量，当前轮到玩家,再将它设置为下一个玩家
	if(temp[2] != '4')
		sendStringS(temp);
	if (turn == firstNum)//处理初始玩家
	{
		if (circleNum < 2)//叫地主环节
		{
			if (temp[2] == '4')//  超时处理
			{
				if (circleNum == 0)//叫地主环节
				{
					//不叫
					std::string temp;
					temp += 'j';
					temp += '0' + turn;
					temp += '1';
					temp += '#';
					sendStringS(temp);
					turn = static_cast<Turn>(turn % 3 + 1);
				}
				else//抢地主环节
				{
					//不抢
					std::string notGrab;
					notGrab += 'j';
					notGrab += '0' + turn;
					notGrab += '3';
					notGrab += '#';
					sendStringS(notGrab);
					circleNum = 2;
					turn = static_cast<Turn>(grabLandlord);
					sendLandlordCards();
					cardsNum[turn - 1] += 3;
					std::string temp;
					temp += 'd';
					temp += '0' + turn;
					/*temp += '0' + firstNum;
					if (grabLandlord == firstNum % 3 + 1)
						temp += '0' + firstNum % 3 + 1;
					else
						temp += '0' + (firstNum + 1) % 3 + 1;*/
					temp += '#';
					sendStringS(temp);
					insertLandlordCards(turn);
				}
			}
			else//没超时
			{
				if (circleNum == 0)//叫地主环节
				{   //叫地主
					if (temp[2] == '0')
					{
						callLandlord = static_cast<Landlord>(turn);
						turn = static_cast<Turn>(turn % 3 + 1);
						circleNum = 1;
					}
					else if (temp[2] == '1')//不叫
					{
						turn = static_cast<Turn>(turn % 3 + 1);
					}
				}
				else//抢地主环节
				{
					if (temp[2] == '2')//抢地主
					{
						sendLandlordCards();
						circleNum = 2;
						//grabLandlord = p1;
						grabLandlord = static_cast<Landlord>(turn);
						cardsNum[turn - 1] += 3;
						std::string temp;
						temp += 'd';
						//temp += '0' + firstNum;
						temp += '0' + turn;
						temp += '#';
						sendStringS(temp);
						insertLandlordCards(turn);
					}
					else if(temp[2] = '3')//不抢
					{
						sendLandlordCards();
						circleNum = 2;
						turn = static_cast<Turn>(grabLandlord); 
						cardsNum[turn - 1] += 3;
						std::string temp;
						temp += 'd';
						temp += '0' + turn;
						/*temp += '0' + firstNum;
						if (grabLandlord == firstNum % 3 + 1)
							temp += '0' + firstNum % 3 + 1;
						else
							temp += '0' + (firstNum + 1) % 3 + 1;*/
						temp += '#';
						sendStringS(temp);
						insertLandlordCards(turn);
					}
				}
			}
		}
	}
	else if (turn == (firstNum % 3 + 1))//处理其左侧玩家
	{
		if (circleNum < 2)//叫地主阶段
		{
			if (temp[2] == '4')//超时
			{
				if (circleNum == 0)
				{
					std::string temp;
					temp += 'j';
					temp += '0' + turn;
					temp += '1';
					temp += '#';
					sendStringS(temp);
					turn = static_cast<Turn>(turn % 3 + 1);
				}
				else
				{
					if (callLandlord != turn)//没叫，抢地主
					{
						std::string temp;
						temp += 'j';
						temp += '0' + turn;
						temp += '3';
						temp += '#';
						sendStringS(temp);
						turn = static_cast<Turn>(turn % 3 + 1);
					}
					else//叫完后 抢地主
					{
						std::string notGrab;
						notGrab += 'j';
						notGrab += '0' + turn;
						notGrab += '3';
						notGrab += '#';
						sendStringS(notGrab);
						sendLandlordCards();
						circleNum = 2;
						turn = static_cast<Turn>(grabLandlord);
						cardsNum[turn - 1] += 3;
						std::string temp;
						temp += 'd';
						temp += '0' + turn;
						temp += '#';
						sendStringS(temp);
						insertLandlordCards(turn);
					}
				}
			}
			else
			{
				if (circleNum == 0)//叫地主环节
				{
					if (temp[2] == '0')//叫地主
					{
						callLandlord = static_cast<Landlord>(turn);
						turn = static_cast<Turn>(turn % 3 + 1);
						circleNum = 1;
					}
					else if (temp[2] == '1')//不叫
					{
						turn = static_cast<Turn>(turn % 3 + 1);
					}
				}
				else //抢地主环节
				{
					if (callLandlord != turn)//没叫，抢地主
					{
						if (temp[2] == '2')
						{
							grabLandlord = static_cast<Landlord>(turn);
							turn = static_cast<Turn>(turn % 3 + 1);
						}
						else if (temp[2] == '3')
						{
							turn = static_cast<Turn>(turn % 3 + 1);
						}
					}
					else//叫完后 抢地主
					{
						if (temp[2] == '2')//抢地主
						{
							sendLandlordCards();
							grabLandlord = static_cast<Landlord>(turn);
							circleNum = 2;
							cardsNum[turn - 1] += 3;
							std::string temp;
							temp += 'd';
							temp += '0' + turn;
							temp += '#';
							sendStringS(temp);
							insertLandlordCards(turn);
						}
						else if(temp[2] == '3')//不抢
						{
							sendLandlordCards();
							circleNum = 2;
							turn = static_cast<Turn>(grabLandlord);
							cardsNum[turn - 1] += 3;
							std::string temp;
							temp += 'd';
							temp += '0' + turn;
							temp += '#';
							sendStringS(temp);
							insertLandlordCards(turn);
						}
					}
				}
			}
		}
	}
	else//处理其右侧玩家
	{
		if (circleNum < 2)//叫地主阶段
		{
			if (temp[2] == '4')//超时
			{
				if (circleNum == 0)
				{
					std::string temp;
					temp += 'j';
					temp += '0' + turn;
					temp += '1';
					temp += '#';
					sendStringS(temp);
					//重开
					std::string t;
					t += 'j';
					t += '5';
					t += '#';
					sendStringS(t);
					shuffleCards();
					giveCardsA();
				}
				else
				{
					if (callLandlord == firstNum)//原始地主叫地主
					{
						std::string temp;
						temp += 'j';
						temp += '0' + turn;
						temp += '3';
						temp += '#';
						sendStringS(temp);
						turn = static_cast<Turn>(turn % 3 + 1);
						if (grabLandlord == none)//没人抢
						{
							sendLandlordCards();
							circleNum = 2;
							cardsNum[turn - 1] += 3;
							std::string temp;
							temp += 'd';
							temp += '0' + turn;
							temp += '#';
							sendStringS(temp);
							insertLandlordCards(turn);
						}
					}
					else//非原始地主叫地主
					{
						std::string notGrab;
						notGrab += 'j';
						notGrab += '0' + turn;
						notGrab += '3';
						notGrab += '#';
						sendStringS(notGrab);
						sendLandlordCards();
						circleNum = 2;
						turn = static_cast<Turn>(callLandlord);
						cardsNum[turn - 1] += 3;
						std::string temp;
						temp += 'd';
						temp += '0' + turn;
						temp += '#';
						sendStringS(temp);
						insertLandlordCards(turn);
					}
				}
			}
			else
			{
				if (circleNum == 1)
				{
					if (callLandlord == firstNum)//原始地主叫地主
					{
						if (temp[2] == '2')
						{
							grabLandlord = static_cast<Landlord>(turn);
							turn = static_cast<Turn>(turn % 3 + 1);
						}
						else if (temp[2] == '3')
						{
							turn = static_cast<Turn>(turn % 3 + 1);
							if (grabLandlord == none)//没人抢
							{
								sendLandlordCards();
								circleNum = 2;
								cardsNum[turn - 1] += 3;
								std::string temp;
								temp += 'd';
								temp += '0' + turn;
								temp += '#';
								sendStringS(temp);
								insertLandlordCards(turn);
							}
						}
					}
					else//非原始地主叫地主
					{
						if (temp[2] == '2')//抢
						{
							grabLandlord = static_cast<Landlord>(turn);
							turn = static_cast<Turn>((turn + 1) % 3 + 1);
						}
						else if (temp[2] == '3')//不抢
						{
							sendLandlordCards();
							circleNum = 2;
							turn = static_cast<Turn>(callLandlord);
							cardsNum[turn - 1] += 3;
							std::string temp;
							temp += 'd';
							temp += '0' + turn;
							temp += '#';
							sendStringS(temp);
							insertLandlordCards(turn);
						}
					}
				}
				else//叫地主
				{
					if (temp[2] == '0')//叫地主
					{
						sendLandlordCards();
						circleNum = 2;
						callLandlord = static_cast<Landlord>(turn);
						cardsNum[turn - 1] += 3;
						std::string temp;
						temp += 'd';
						temp += '0' + turn;
						temp += '#';
						sendStringS(temp);
						insertLandlordCards(turn);
					}
					else if(temp[2] == '1')//不叫
					{
						//重开
						std::string t;
						t += 'j';
						t += '5';
						t += '#';
						sendStringS(t);
						shuffleCards();
						giveCardsA();
					}
				}
			}
		}
	}
	std::string t;//指示下一个玩家是谁
	t += 'a';
	t += '0' + turn;
	t += '0' + circleNum;//标记是叫地主0，还是抢地主1,出牌2
	if (circleNum == 2)//附加出牌菜单禁用
		t += '0';
	t += '#';
	sendStringS(t);
}
void Net::manageTimeOut(std::string temp)
{
	Turn turn = static_cast<Turn>(temp[1] - '0');//可作为局部变量
	if (circleNum < 2)
	{
		if (turn == firstNum)//处理初始玩家
		{
			if (circleNum == 0)//叫地主环节
			{
				//不叫
				std::string temp;
				temp += 'j';
				temp += '0' + turn;
				temp += '1';
				temp += '#';
				sendStringS(temp);
				turn = static_cast<Turn>(turn % 3 + 1);
			}
			else//抢地主环节
			{
				//不抢
				std::string notGrab;
				notGrab += 'j';
				notGrab += '0' + turn;
				notGrab += '3';
				notGrab += '#';
				sendStringS(notGrab);
				circleNum = 2;
				turn = static_cast<Turn>(grabLandlord);
				sendLandlordCards();
				cardsNum[turn - 1] += 3;
				std::string temp;
				temp += 'd';
				temp += '0' + turn;
				/*temp += '0' + firstNum;
				if (grabLandlord == firstNum % 3 + 1)
				temp += '0' + firstNum % 3 + 1;
				else
				temp += '0' + (firstNum + 1) % 3 + 1;*/
				temp += '#';
				sendStringS(temp);
				insertLandlordCards(turn);
			}
		}
		else if (turn == (firstNum % 3 + 1))//处理其左侧玩家
		{
			if (circleNum == 0)
			{
				std::string temp;
				temp += 'j';
				temp += '0' + turn;
				temp += '1';
				temp += '#';
				sendStringS(temp);
				turn = static_cast<Turn>(turn % 3 + 1);
			}
			else
			{
				if (callLandlord != turn)//没叫，抢地主
				{
					std::string temp;
					temp += 'j';
					temp += '0' + turn;
					temp += '3';
					temp += '#';
					sendStringS(temp);
					turn = static_cast<Turn>(turn % 3 + 1);
				}
				else//叫完后 抢地主
				{
					std::string notGrab;
					notGrab += 'j';
					notGrab += '0' + turn;
					notGrab += '3';
					notGrab += '#';
					sendStringS(notGrab);
					sendLandlordCards();
					circleNum = 2;
					turn = static_cast<Turn>(grabLandlord);
					cardsNum[turn - 1] += 3;
					std::string temp;
					temp += 'd';
					temp += '0' + turn;
					temp += '#';
					sendStringS(temp);
					insertLandlordCards(turn);
				}
			}
		}
		else//处理其右侧玩家
		{
			if (circleNum == 0)
			{
				std::string temp;
				temp += 'j';
				temp += '0' + turn;
				temp += '1';
				temp += '#';
				sendStringS(temp);
				//重开
				std::string t;
				t += 'j';
				t += '5';
				t += '#';
				sendStringS(t);
				shuffleCards();
				giveCardsA();
			}
			else
			{
				if (callLandlord == firstNum)//原始地主叫地主
				{
					std::string temp;
					temp += 'j';
					temp += '0' + turn;
					temp += '3';
					temp += '#';
					sendStringS(temp);
					turn = static_cast<Turn>(turn % 3 + 1);
					if (grabLandlord == none)//没人抢
					{
						sendLandlordCards();
						circleNum = 2;
						cardsNum[turn - 1] += 3;
						std::string temp;
						temp += 'd';
						temp += '0' + turn;
						temp += '#';
						sendStringS(temp);
						insertLandlordCards(turn);
					}
				}
				else//非原始地主叫地主
				{
					std::string notGrab;
					notGrab += 'j';
					notGrab += '0' + turn;
					notGrab += '3';
					notGrab += '#';
					sendStringS(notGrab);
					sendLandlordCards();
					circleNum = 2;
					turn = static_cast<Turn>(callLandlord);
					cardsNum[turn - 1] += 3;
					std::string temp;
					temp += 'd';
					temp += '0' + turn;
					temp += '#';
					sendStringS(temp);
					insertLandlordCards(turn);
				}
			}
		}
	}
	else
	{
		//OutputDebugStringA("timeout");
		std::vector<puke> hintCards;
		if (lastCards.empty() || big == temp[1] - '0')//拥有出牌权
		{
			std::string t;
			if (temp[0] == 't')
			{	
				hintCards = getHintCards(temp[1] - '1');	
			}
			else
			{
				hintCards = getTuoGuanCards(temp[1] - '1');
			}
			t = "c";
			t += temp[1];
			t += '0';
			for (int i = 0; i < hintCards.size(); i++)
			{
				t += '0' + hintCards[i].type;
				t += '0' + hintCards[i].num;
			}
			t += '#';
			sendStringS(t);
			lastCards = hintCards;
			big = static_cast<Turn>(temp[1] - '0');
			removePuke(t);
			turn = static_cast<Turn>((temp[1] - '0') % 3 + 1);
			int num = hintCards.size();
			cardsNum[big - 1] -= num;
			if (playerCards[big - 1].size() == 0)
			{
				std::string t1;
				t1 += 'w';
				if (big == (grabLandlord == none ? callLandlord : grabLandlord))//地主先出去
				{
					t1 += '0';
				}
				else
				{
					t1 += '1';
				}
				t1 += '#';
				sendStringS(t1);
			}
		}
		else
		{
			turn = static_cast<Turn>((temp[1] - '0') % 3 + 1);
			if (temp[0] == 't')//超时处理
			{
				std::string  t = "c";
				t += temp[1];
				t += '5';
				t += '#';
				sendStringS(t);
			}
			else if (temp[0] == 'z')//托管处理
			{
				std::vector<puke> tempCards = rules.hintCards(lastCards, playerCards[temp[1] - '1']);
				if (tempCards.size() > 0)
				{
					big = static_cast<Turn>(temp[1] - '0');
					std::string t = "c";
					t += temp[1];
					t += '0';
					for (int i = 0; i < tempCards.size(); i++)
					{
						t += '0' + tempCards[i].type;
						t += '0' + tempCards[i].num;
					}
					t += '#';
					removePuke(t);
					sendStringS(t);
					lastCards = tempCards;
					cardsNum[big - 1] -= tempCards.size();
					turn = static_cast<Turn>((temp[1] - '0') % 3 + 1);
					if (cardsNum[big - 1] == 0)
					{
						std::string t1;
						t1 += 'w';
						if (big == (grabLandlord == none ? callLandlord : grabLandlord))//地主先出去
						{
							t1 += '0';
						}
						else
						{
							t1 += '1';
						}
						t1 += '#';
						sendStringS(t1);
					}
				}
				else//不出
				{
					std::string  t = "c";
					t += temp[1];
					t += '5';
					t += '#';
					sendStringS(t);
				}
			}
		}
	}
	std::string t;//指示下一个玩家是谁
	t += 'a';
	t += '0' + turn;
	t += '0' + circleNum;//标记是叫地主0，还是抢地主1,出牌2
	t += '#';
	sendStringS(t);
}
void Net::manageTuoGuan(std::string temp)
{
	if (temp[2] == '0')//托管
	{
		/*isTuoGuanState[temp[1] - '1'] = true;*/
		sendStringS(temp);
	}
	else if (temp[2] == '1')//取消托管
	{
		/*isTuoGuanState[temp[1] - '1'] = false;*/
		sendStringS(temp);
	}
	else if (temp[2] == '2')//托管中
	{
		manageTimeOut(temp);
	}
}
void Net::sendLandlordCards()
{
//	m_mutex1.lock();//死锁
	for (int i = 0; i < 3; i++)//发地主牌
	{
		std::string t;
		t += 'f';
		t += '5';
		t += '0' + allCards[51 + i].type;
		t += '0' + allCards[51 + i].num;
		t += '#';
		sendStringS(t);
	}
	//m_mutex1.unlock();
}
void Net::managePuke(std::string temp)
{
	if (temp[2] == '0')//客户端出牌
	{
		std::vector<puke> cards = makeCards(temp);
		if (rule.getCardsType(cards) == Invalid)
		{
			std::string t;
			t += 'c';
			t += temp[1];
			t += '6';
			t += '#';
			sendStringTo(t, temp[1] - '0');
		}
		else
		{
			if (lastCards.empty() || big == temp[1] - '0')//拥有出牌权
			{
				sendStringS(temp);
				lastCards = cards;
				big = static_cast<Turn>(temp[1] - '0');
				std::string t;
				t += 'a';
				t += '0' + (temp[1] - '0') % 3 + 1;
				t += '2';
				t += '#';
				sendStringS(t);	
				int num = (temp.size() - 4) / 2;
				cardsNum[big - 1] -= num;
				removePuke(temp);
				if (playerCards[big - 1].size() == 0)
				{
					std::string t1;
					t1 += 'w';
					if (big == (grabLandlord == none ? callLandlord : grabLandlord))//地主先出去
					{	
						t1 += '0';
					}
					else
					{
						t1 += '1';
					}
					t1 += '#';
					sendStringS(t1);
				}
			}
			else
			{
				if (rule.compare(cards, lastCards))//管上
				{
					sendStringS(temp);
					lastCards = cards;
					big = static_cast<Turn>(temp[1] - '0');
					std::string t;
					t += 'a';
					t += '0' + (temp[1] - '0') % 3 + 1;
					t += '2';
					t += '#';
					sendStringS(t);
					int num = (temp.size() - 4) / 2;
					cardsNum[big - 1] -= num;
					removePuke(temp);
					if (playerCards[big - 1].size() == 0)	
					{
						std::string t1;
						t1 += 'w';
						if (big == (grabLandlord == none ? callLandlord : grabLandlord))//地主先出去
						{
							t1 += '0';
						}
						else
						{
							t1 += '1';
						}
						t1 += '#';
						sendStringS(t1);
					}
				}
				else 
				{
					std::string t;
					t += 'c';
					t += temp[1];
					t += '7';
					t += '#';
					sendStringTo(t, temp[1] - '0');
				}
			}
		}
	}
	else if (temp[2] == '5')//客户端不出
	{
		sendStringS(temp);
		std::string t;
		t += 'a';
		t += '0' + (temp[1] - '0') % 3 + 1;
		t += '2';
		if (big == (temp[1] - '0') % 3 + 1)
			t += '0';
		t += '#';
		sendStringS(t);
	}
	else if (temp[2] == '4')//客户端请求提示
	{
		std::vector<puke> tempCards = rules.hintCards(lastCards, playerCards[temp[1] - '1']);
		if (tempCards.size() > 0)
		{
		   std::vector<int> position;
		   for (int i = 0; i < tempCards.size(); i++)
		   {
			   for (int j = 0; j < playerCards[temp[1] - '1'].size(); j++)
			   {
				   if (tempCards[i].type == playerCards[temp[1] - '1'][j].type && tempCards[i].num == playerCards[temp[1] - '1'][j].num)
					   position.push_back(j);
			   }
		   }
		   //for (int i = position.size() - 1; i >= 0; i--)//移除这些牌
		   //{
			  // playerCards[temp[1] - '1'].erase(playerCards[temp[1] - '1'].begin() + i);
		   //}
		   std::string t = "c";
		   t += temp[1];
		   t += '4';
		   for (int i = 0; i < position.size(); i++)
		   {
			   t += '0' + position[i];
		   }
		   t += '#';
		   sendStringTo(t, temp[1] - '0');
		}
		else//你没有大过上家的牌
		{
			std::string  t = "c";
			t += temp[1];
			t += '3';
			t += '#';
			sendStringTo(t, temp[1] - '0');
		}
	}
}
std::vector<puke> Net::makeCards(std::string temp)
{
	std::vector<puke> t;
	int i = 3;
	while (i < temp.size() - 1)
	{
		t.push_back(puke(temp[i] - '0', temp[i + 1] - '0'));
		i += 2;
	}
	return t;
}
void Net::insertLandlordCards(int num)
{
	for (int i = 51; i < 54; i++)
		playerCards[num - 1].push_back(allCards[i]);
	rules.sortPukes(playerCards[num - 1]);
}
std::vector<puke> Net::getHintCards(int num)//超时主动出牌
{
	std::vector<puke> temp;
	temp.push_back(playerCards[num][playerCards[num].size() - 1]);
	playerCards[num].erase(playerCards[num].begin() + playerCards[num].size() - 1);
	return temp;
}
void Net::removePuke(std::string temp)
{
	for (int i = 3; i < temp.size() - 1; i += 2)
	{
		for (int j = 0; j < playerCards[temp[1] - '1'].size(); j++)
		{
			if (playerCards[temp[1] - '1'][j].type == temp[i] - '0' &&
				playerCards[temp[1] - '1'][j].num == temp[i + 1] - '0')
			{
				playerCards[temp[1] - '1'].erase(playerCards[temp[1] - '1'].begin() + j);
				break;
			}
		}
	}
}
std::vector<puke> Net::getTuoGuanCards(int player)
{
	/*“飞机类牌”，“四带两个单张 / 对子”，“连对”，
	“顺子”，“三带一 / 二”，“炸弹类牌”，“对子”，“单张”*/
	std::vector<puke> temp;
	temp = getType1(player);
	if (!temp.empty())
	{
		//OutputDebugStringA("1--------------------------------------1");
		return temp;
	}
	temp = getType2(player);
	if (!temp.empty())
	{
		//OutputDebugStringA("2--------------------------------------------2");
		return temp;
	}
	temp = getType3(player);
	if (!temp.empty())
	{
		//OutputDebugStringA("3---------------------------------------3");
		return temp;
	}
	temp = getType4(player);
	if (!temp.empty())
	{
		//OutputDebugStringA("4---------------------------------------4");
		return temp;
	}
	temp = getType5(player);
	if (!temp.empty())
	{
		//OutputDebugStringA("5---------------------------------------------5");
		return temp;
	}
	temp = getType6(player);
	if (!temp.empty())
	{
		//OutputDebugStringA("6----------------------------------------------6");
		return temp;
	}
	temp = getType7(player);
	if (!temp.empty())
	{
		//OutputDebugStringA("7---------------------------------------------7");
		return temp;
	}
}
std::vector<puke> Net::getType1(int player)
{
	std::vector<puke> temp;
	std::vector<int> cardNum(15, 0);
	for (int i = 0; i < playerCards[player].size(); i++)
		cardNum[playerCards[player][i].num - 1] += 1;
	//分别表示 当前连续数量，当前起始位置， 最大起始位置， 最大连续长度
	int continueNum = 0, startNum = -1, rightStartNum = -1, rightContinueNum = 0;
	int oneNum = 0, twoNum = 0, fourNum = 0;
	for (int i = 0; i < 15; i++)
	{
		if (cardNum[i] >= 3)
		{
			if (cardNum[i] == 4)
				cardNum[i] = 3;
			if (startNum == -1)
				startNum = i;
			if (startNum == -1 || startNum + continueNum == i)
			{
				continueNum += 1;
			}
		}
		else
		{
			if (continueNum >= 2 && startNum != -1 && rightContinueNum < continueNum)
			{
				rightContinueNum = continueNum;
				rightStartNum = startNum;
				startNum = -1;
				continueNum = 0;
			}
			if (cardNum[i] == 1)
				oneNum += 1;
			if (cardNum[i] == 2)
				twoNum += 1;
			if (cardNum[i] == 4)
				fourNum += 1;
		}
	}
	if (rightStartNum != -1)
	{
		for (int i = 0; i < playerCards[player].size(); i++)
		{
			if (playerCards[player][i].num >= rightStartNum + 1 && playerCards[player][i].num < rightStartNum + 1 + rightContinueNum)
			{
				temp.push_back(playerCards[player][i]);
				cardNum[playerCards[player][i].num - 1] -= 1;
				if (cardNum[playerCards[player][i].num - 1] == 0)
					cardNum[playerCards[player][i].num - 1] = 3;
			}
		}
		if (twoNum >= rightContinueNum)//飞机带两个对子
		{
			int doubleNum1 = -1, doubleNum2 = -1;
			for (int i = 0; i < 15; i++)
			{
				if (cardNum[i] == 2)
				{
					if (doubleNum1 == -1)
						doubleNum1 = i + 1;
					else if (doubleNum2 == -1)
						doubleNum2 = i + 1;
					else
						break;
				}
			}
			if (doubleNum1 != -1 && doubleNum2 != -1)
			{
				for (int i = 0; i < playerCards[player].size(); i++)
				{
					if (playerCards[player][i].num == doubleNum1 || playerCards[player][i].num == doubleNum2)
						temp.push_back(playerCards[player][i]);
				}
			}
		}
		else if (oneNum >= rightContinueNum)//飞机带两个单张，需刨除两个王的情况
		{
			int singleNum1 = -1, singleNum2 = -1;
			for (int i = 0; i < 15; i++)
			{
				if (i < rightStartNum && i >= rightStartNum + rightContinueNum && cardNum[i] == 1 || cardNum[i] == 2)
				{
					if (singleNum1 == -1)
						singleNum1 = i + 1;
					else if (singleNum2 == -1)
						singleNum2 = i + 1;
					else
						break;
				}
			}
			if (singleNum1 != 14 || singleNum2 != 15)
			{
				for (int i = 0; i < playerCards[player].size(); i++)
				{
					if (playerCards[player][i].num == singleNum1)
					{
						singleNum1 = -1;
						temp.push_back(playerCards[player][i]);
					}
					if (playerCards[player][i].num == singleNum2)
					{
						singleNum2 = -1;
						temp.push_back(playerCards[player][i]);
					}
				}
			}
		}
	}
	return temp;
}
std::vector<puke> Net::getType2(int player)
{
	std::vector<puke> temp;
	std::vector<int> cardNum(15, 0);
	for (int i = 0; i < playerCards[player].size(); i++)
		cardNum[playerCards[player][i].num - 1] += 1;
	int oneNum = 0, twoNum = 0, fourNum = 0, fourValue = -1;
	bool have = false;
	for (int i = 0; i < 15; i++)
	{
		if (cardNum[i] == 1)
			oneNum += 1;
		if (cardNum[i] == 2)
			twoNum += 1;
		if (cardNum[i] == 4)
		{
			fourNum += 1;
			if(fourValue == -1)
				fourValue = i + 1;
		}
	}
	if (fourNum > 0)
	{
		for (int i = 0; i < playerCards[player].size(); i++)
		{
			if (playerCards[player][i].num == fourValue)
			{
				temp.push_back(playerCards[player][i]);
			}
		}
		if (twoNum >= 2)
		{
			int putedNum = 0;
			for (int i = 0; i < playerCards[player].size(); i++)
			{
				if (cardNum[playerCards[player][i].num - 1] == 2)
				{
					temp.push_back(playerCards[player][i]);
					putedNum += 1;
					if (putedNum == 4)
						break;
				}
			}
			have = true;
		}
		else if (oneNum + twoNum >= 2)
		{
			int singleNum1 = -1, singleNum2 = -1;
			for (int i = 0; i < 15; i++)
			{
				if (cardNum[i] == 1 || cardNum[i] == 2)
				{
					if (singleNum1 == -1)
						singleNum1 = i + 1;
					else if (singleNum2 == -1)
						singleNum2 = i + 1;
					else
						break;
				}
			}
			if (singleNum1 != 14 || singleNum2 != 15)
			{
				for (int i = 0; i < playerCards[player].size(); i++)
				{
					if (playerCards[player][i].num == singleNum1)
					{
						singleNum1 = -1;
						temp.push_back(playerCards[player][i]);
					}
					if (playerCards[player][i].num == singleNum2)
					{
						singleNum2 = -1;
						temp.push_back(playerCards[player][i]);
					}
				}
			}
			have = true;
		}	
	}
	if (have)
		return temp;
	else
	{
		temp.clear();
		return temp;
	}
}
std::vector<puke> Net::getType3(int player)
{
	std::vector<puke> temp;
	std::vector<int> cardNum(15, 0);
	for (int i = 0; i < playerCards[player].size(); i++)
		cardNum[playerCards[player][i].num - 1] += 1;
	int continueNum = 0, startNum = -1, rightStartNum = -1, rightContinueNum = 0;
	for (int i = 0; i < 15; i++)
	{
		if (cardNum[i] >= 2)
		{
			/*if (cardNum[i] > 2)
				cardNum[i] = 2;*/
			if (startNum == -1)
				startNum = i;
			if (startNum == -1 || startNum + continueNum == i)
			{
				continueNum += 1;
			}
		}
		else
		{
			if (continueNum >= 3 && startNum != -1 )
			{
				rightContinueNum = continueNum;
				rightStartNum = startNum;
				for (int j = rightStartNum; j < rightContinueNum + rightStartNum; j++)
				{
					cardNum[j] = 2;
				}
				break;
			}
			startNum = -1;
			continueNum = 0;
		}
	}
	if (rightStartNum + rightContinueNum - 1 == 12)//抛出数字2
		rightContinueNum--;
	if (rightContinueNum >= 3)
	{
		for (int i = 0; i < playerCards[player].size(); i++)
		{
			if (cardNum[playerCards[player][i].num - 1] > 0 && playerCards[player][i].num >= rightStartNum + 1
				&& playerCards[player][i].num < rightStartNum + 1 + rightContinueNum)
			{
				temp.push_back(playerCards[player][i]);
				cardNum[playerCards[player][i].num - 1] -= 1;
			}
		}
	}
	return temp;
}
std::vector<puke> Net::getType4(int player)
{
	std::vector<puke> temp;
	std::vector<int> cardNum(15, 0);
	for (int i = 0; i < playerCards[player].size(); i++)
		cardNum[playerCards[player][i].num - 1] += 1;
	int continueNum = 0, startNum = -1, rightStartNum = -1, rightContinueNum = 0;
	for (int i = 0; i < 15; i++)
	{
		if (cardNum[i] >= 1)
		{
			if (cardNum[i] >= 2)
				cardNum[i] = 1;
			if (startNum == -1)
				startNum = i;
			if (startNum == -1 || startNum + continueNum == i)
			{
				continueNum += 1;
			}
		}
		else
		{
			if (continueNum >= 5 && startNum != -1)
			{
				rightContinueNum = continueNum;
				rightStartNum = startNum;	
				for (int j = rightStartNum; j < rightContinueNum + rightStartNum; j++)
				{
					cardNum[j] = 1;
				}
				break;
			}
			startNum = -1;
			continueNum = 0;
		}
	}
	if (rightStartNum + rightContinueNum - 1 >= 12)//去掉2以上的牌
	{
		rightContinueNum -= rightStartNum + rightContinueNum - 12;
	}
	if (rightContinueNum >= 5)
	{
		for (int i = 0; i < playerCards[player].size(); i++)
		{
			if (cardNum[playerCards[player][i].num - 1] > 0 && playerCards[player][i].num >= rightStartNum + 1
				&& playerCards[player][i].num < rightStartNum + 1 + rightContinueNum)
			{
				temp.push_back(playerCards[player][i]);
				cardNum[playerCards[player][i].num - 1] -= 1;
			}
		}
	}
	return temp;
}
std::vector<puke> Net::getType5(int player)//不拆开炸弹
{
	std::vector<puke> temp;
	std::vector<int> cardNum(15, 0);
	for (int i = 0; i < playerCards[player].size(); i++)
		cardNum[playerCards[player][i].num - 1] += 1;
	int oneNum = 0, twoNum = 0, fourNum = 0, threeValue = -1;
	for (int i = 0; i < 15; i++)
	{
		if (cardNum[i] == 1)
			oneNum += 1;
		if (cardNum[i] == 2)
			twoNum += 1;
		if (cardNum[i] == 3)
		{
			if (threeValue == -1)
				threeValue = i + 1;
		}
	}
	if (threeValue > 0)
	{
		for (int i = 0; i < playerCards[player].size(); i++)
		{
			if (playerCards[player][i].num == threeValue)
			{
				temp.push_back(playerCards[player][i]);
			}
		}
		if (twoNum >= 1)
		{
			int putedNum = 0;
			for (int i = 0; i < playerCards[player].size(); i++)
			{
				if (cardNum[playerCards[player][i].num - 1] == 2)
				{
					temp.push_back(playerCards[player][i]);
					putedNum += 1;
					if (putedNum == 2)
						break;
				}
			}
		}
		else if (oneNum + twoNum >= 1)
		{
			int singleNum = -1;
			for (int i = 0; i < 15; i++)
			{
				if (cardNum[i] == 1 || cardNum[i] == 2)
				{
					if (singleNum == -1)
						singleNum = i + 1;
					else
						break;
				}
			}
			for (int i = 0; i < playerCards[player].size(); i++)
			{
				if (playerCards[player][i].num == singleNum)
				{
					singleNum = -1;
					temp.push_back(playerCards[player][i]);
				}
			}
		}
	}
	return temp;
}
std::vector<puke> Net::getType6(int player)
{
	std::vector<puke> temp;
	std::vector<int> cardNum(15, 0);
	for (int i = 0; i < playerCards[player].size(); i++)
		cardNum[playerCards[player][i].num - 1] += 1;
	int fourNum = -1;
	for (int i = 0; i < 15; i++)
	{
		if (cardNum[i] == 4)
		{
			fourNum = i + 1;
			break;
		}
	}
	if (fourNum != -1)
	{
		for (int i = 0; i < playerCards[player].size(); i++)
		{
			if (playerCards[player][i].num == fourNum)
				temp.push_back(playerCards[player][i]);
		}
	}
	else if (cardNum[13] == 1 && cardNum[14] == 1)
	{
		for (int i = 0; i < playerCards[player].size(); i++)
		{
			if (playerCards[player][i].num == 14 || playerCards[player][i].num == 15)
				temp.push_back(playerCards[player][i]);
		}
	}
	return temp;
}
std::vector<puke> Net::getType7(int player)
{//选择单牌和对子中小的一个
	std::vector<puke> temp;
	std::vector<int> cardNum(15, 0);
	for (int i = 0; i < playerCards[player].size(); i++)
		cardNum[playerCards[player][i].num - 1] += 1;
	int twoNum = -1, oneNum = -1;
	for (int i = 0; i < 15; i++)
	{
		if (oneNum == -1 && cardNum[i] == 1)
			oneNum = i + 1;
		if (twoNum == -1 && cardNum[i] == 2)
			twoNum = i + 1;
	}
	if (twoNum < oneNum)
	{
		for (int i = 0; i < playerCards[player].size(); i++)
		{
			if (playerCards[player][i].num == twoNum)
			{
				temp.push_back(playerCards[player][i]);
			}
		}
	}
	else 
	{
		for (int i = 0; i < playerCards[player].size(); i++)
		{
			if (playerCards[player][i].num == oneNum)
			{
				temp.push_back(playerCards[player][i]);
				break;
			}
		}
	}
	return temp;
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
			else
			{
				m_mutex3.lock();
				serverStr.push_back(t);
				m_mutex3.unlock();
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
	if (listenSocket)
	{
		delete listenSocket;
		listenSocket = NULL;
	}
	for (std::vector<Connection*>::iterator it = clientList.begin(); it != clientList.end(); it++)
	{
		if (*it) 
		{
			delete *it;
			*it = NULL;
		}
	}
	clientList.clear();
	connect = false;
	serverIP.clear();
	clientStr.clear();
	serverStr.clear();
	maxfd = -1;
	exit = false;
}
