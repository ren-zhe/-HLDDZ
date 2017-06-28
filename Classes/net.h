#ifndef __NET_SCENE_H__
#define __NET_SCENE_H__
#include "ODSocket.h"
#include<thread>
#include<mutex>
#include<deque>
enum Landlord
{
	p1 = 1,
	p2,
	p3,
	none
};
enum Turn {
	player1 = 1,
	player2,
	player3
};

class Net
{
public:
	bool makeClient();//创建客户端
	void clientRecv(std::string temp);//客户端发起连接
	void CheckConn(const fd_set& fdRead, const fd_set& fdWrite, const fd_set& fdExcept, Connection* conn);//客户端检查是否可读写
	bool TryRead(Connection* pConn);//读操作
	bool TryWrite(Connection* pConn);//写操作
	bool PassiveShutdown(SOCKET sd, const char* buff, int len);
	bool createUdpSocket();//创建udp套接字
	void sendStringC(std::string temp);//客户端发送消息
	static Net* getInstance()
	{
		if (m_net == NULL)
		{
			m_net = new Net();
		}
		return m_net;
	}
	void destroyInstance()
	{
		if (connectSocket)
		{
			delete connectSocket;
			connectSocket = NULL;
		}
		if (clientConnection)
		{
			delete clientConnection;
			clientConnection = NULL;
		}
		if (m_net)
			delete m_net;
		m_net = NULL;
	}
	void manageStr(std::string temp, bool isClient);
	void DeleteSocket();
	std::deque<std::string> clientStr;//客户端消息
	int playerNum; //记录玩家编号
	std::mutex m_mutex;//用于控制clientStr互斥访问
	std::mutex m_mutex2;//用于控制客户端sendBuffer的互斥访问
	Connection* clientConnection;//客户端的读写信息
	bool exit;//用于销毁线程
	static Net* m_net;
private:
	ODSocket* connectSocket;//客户端连接套接字
	ODSocket* udpSocket;//udp套接字，用于接收广播消息
	std::string serverIP;//服务器IP
	int serverPort;//服务器端口号
	bool connect;//标记是否与服务器连接上
	int maxfd;//Linux下使用记录最大文件描述符

	Net() :udpSocket(NULL), connectSocket(NULL), connect(false), maxfd(-1), clientConnection(NULL), playerNum(0)
	{
		serverIP = "49.140.162.151";
		exit = false;
	}
};

#endif // __NET_SCENE_H__

