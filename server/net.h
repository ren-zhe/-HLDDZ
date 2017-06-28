#ifndef __NET_SCENE_H__
#define __NET_SCENE_H__
#include "ODSocket.h"
#include<thread>
#include<mutex>
#include<deque>
#include"rules.h"
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
	bool makeServer();//创建服务器
	//bool makeClient();//创建客户端
	//void clientRecv(std::string temp);//客户端发起连接
	//virtual void Manage(float dt);
	void accept();//服务器接收连接
	//void SendBroadcast();//发送广播
	//void RecvBroadcast();//接收广播
	//void RecvBroadcastFunc();
	void ResetFDSet(fd_set& fdRead, fd_set& fdWrite, fd_set& fdExcept, SOCKET sdListen, const ConnectionList& conns);
	int CheckAccept(const fd_set& fdRead, const fd_set& fdExcept, SOCKET sdListen, ConnectionList& conns);//检查有无连接
	void CheckConn(const fd_set& fdRead, const fd_set& fdWrite, const fd_set& fdExcept, ConnectionList& conns);//服务器检查是否可读写
	//void CheckConn(const fd_set& fdRead, const fd_set& fdWrite, const fd_set& fdExcept, Connection* conn);//客户端检查是否可读写
	bool TryRead(Connection* pConn);//读操作
	bool TryWrite(Connection* pConn);//写操作
	bool PassiveShutdown(SOCKET sd, const char* buff, int len);
	bool createUdpSocket();//创建udp套接字
	void cardManage();//处理扑克创建洗牌发牌
	void makeCards();
	void shuffleCards();
	void giveCards();//发牌
	void giveCardsA();//重发牌
	void manageServerMessage(std::string temp);//服务器处理消息
	//void sendStringC(std::string temp);//客户端发送消息
	void sendStringS(std::string temp);//向玩家发送消息
	void sendStringTo(std::string temp, int num);//向单个玩家发送消息
	void manageLandlord(std::string temp);//处理叫地主阶段的消息
	void manageTimeOut(std::string temp);//处理超时消息
	void manageTuoGuan(std::string temp);//处理托管消息
	void sendLandlordCards();//发送地主牌
	void managePuke(std::string temp);//处理出牌消息
	std::vector<puke> makeCards(std::string temp);//根据字符串创建扑克序列
	void insertLandlordCards(int num);
	std::vector<puke> getHintCards(int num);//拥有主动权时自动出的牌
	void removePuke(std::string temp);//移除扑克
	std::vector<puke> getTuoGuanCards(int player);//获取托管时主动出的牌
	//获取托管时指定牌型
	std::vector<puke> getType1(int player);//飞机类
	std::vector<puke> getType2(int player);//四带一或二
	std::vector<puke> getType3(int player);//连对
	std::vector<puke> getType4(int player);//顺子
	std::vector<puke> getType5(int player);//三带一 / 二,三张
	std::vector<puke> getType6(int player);//炸弹类牌
	std::vector<puke> getType7(int player);//单牌或对子
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
    std::deque<std::string> serverStr;//服务器消息
	std::deque<std::string> clientStr;//客户端消息
	std::vector<puke> myCards;//用于客户端接收自己的牌
	int playerNum; //记录玩家编号
	std::mutex m_mutex;//用于控制clientStr互斥访问
	std::mutex m_mutex2;//用于控制客户端sendBuffer的互斥访问
	std::mutex m_mutex1;//用于控制服务器sendBuffer的互斥访问
	std::mutex m_mutex3;//用于控制服务器serverStr的互斥访问
	int firstNum; //记录第一个开始的玩家
	Connection* clientConnection;//客户端的读写信息
	bool exit;//用于销毁线程
	static Net* m_net;
private:
	
	std::vector<int> readyPlayer;//已经准备好的玩家
	int circleNum;//记录阶段信息，0叫地主阶段，1抢地主阶段，2出牌阶段
	ODSocket* connectSocket;//客户端连接套接字
	ODSocket* listenSocket;//监听套接字
	ODSocket* udpSocket;//udp套接字，用于接收广播消息
	ConnectionList clientList;//已连接客户端列表
	std::string serverIP;//服务器IP
	int serverPort;//服务器端口号
	int readyNum;//已准备好的客户端数量
	Landlord callLandlord;//叫地主玩家
	Landlord grabLandlord;//抢地主玩家
	Turn big;//当前最大的玩家
	bool connect;//标记是否与服务器连接上
	std::vector<puke> allCards;//存储所有的牌
	std::vector<puke> lastCards;//存储上一次出的牌
	std::vector<std::vector<puke>> playerCards;//记录每个玩家的牌
	int maxfd;//Linux下使用记录最大文件描述符

	bool discardCards;//是否已经发完牌
	Rules rule;//判定游戏规则
	std::vector<int> cardsNum;//记录每个玩家的扑克数量,可以考虑删除，由playerCards代替
	Rules rules;
	//std::vector<bool> isTuoGuanState;//
	Net() :listenSocket(NULL), udpSocket(NULL), connectSocket(NULL), connect(false), maxfd(-1), clientConnection(NULL), playerNum(0)
	{
		clientList.clear();
		serverIP.clear();
		exit = false;
		readyNum = 0;
		discardCards = false;
		circleNum = 0;
		callLandlord = none;
		grabLandlord = none;
		for (int i = 0; i < 3; i++)
		{
			cardsNum.push_back(17);//初始化玩家牌数量
			playerCards.push_back(std::vector<puke>());//
			//isTuoGuanState.push_back(false);//初始化托管状态
		}
	}
};

#endif // __NET_SCENE_H__

