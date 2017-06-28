#include<iostream>
#include"net.h"
#include<thread>
using namespace std;
void func()
{
	while (true)
	{
		std::cout << "-------f----" << endl;
		static int i = 0;
		i++;
		/*if (i == 10)
			break;*/
	}
	//Net::getInstance()->makeServer();
	
}
int main()
{
	//创建服务端
	Net::getInstance()->createUdpSocket();
	Net::getInstance()->makeServer();
	return 0;
}