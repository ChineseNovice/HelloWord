// MysqlConnectionPool.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include"Connection.h"
#include"CommonConnectionPool.h"
#include"test.h"
using namespace std;


int main()
{

    
	int thread_num = 0; int connection_num = 0;
	cout << "线程数量:";
	cin >> thread_num;
	cout << endl;
	cout << "每个线程连接数量:";
	cin >> connection_num;
    clock_t begin = clock();
    //ConnectionPool* pool = ConnectionPool::getConnectionPool();
    for (int i = 0; i < 10000; ++i) {
#if 0
        Connection conn;    //不使用连接池40000左右
        char sql[1024] = { 0 };
        sprintf_s(sql, "insert into user(name,age,sex) values('%s','%d','%s')",
            "zhang san", 20, "MALE");
        conn.connect("127.0.0.1", 3306, "root", "20030127zhang", "chat");
        conn.update(sql);
#endif

        

#if 0
        shared_ptr<Connection> sp = pool->getConnection();  //使用连接池，9500左右
        char sql[1024] = { 0 };
        sprintf_s(sql, "insert into user(name,age,sex) values('%s','%d','%s')",
            "zhang san", 20, "MALE");
        sp->update(sql);
#endif
    }
#if 0	
		thread t1([]() {
		//
		for (int i = 0; i < 250; ++i)
		{
			ConnectionPool *cp = ConnectionPool::getConnectionPool();
			char sql[1024] = { 0 };
			sprintf_s(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		}
		});
	thread t2([]() {
		//
		for (int i = 0; i < 250; ++i)
		{
			ConnectionPool *cp = ConnectionPool::getConnectionPool();
			char sql[1024] = { 0 };
			sprintf_s(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		}
		});
	thread t3([]() {
		//
		for (int i = 0; i < 250; ++i)
		{
			ConnectionPool *cp = ConnectionPool::getConnectionPool();
			char sql[1024] = { 0 };
			sprintf_s(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		}
		});
	thread t4([]() {
		//
		for (int i = 0; i < 250; ++i)
		{
			ConnectionPool *cp = ConnectionPool::getConnectionPool();
			char sql[1024] = { 0 };
			sprintf_s(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		}
});

	t1.join();
	t2.join();
	t3.join();
	t4.join();
#endif

	Test t;
	t.test(thread_num, connection_num);

    clock_t end = clock();
    cout << "Time=" << end - begin << "ms" << endl;
    return 0;
    
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
