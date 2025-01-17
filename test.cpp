#include "test.h"
#include"CommonConnectionPool.h"
#include <thread>
#include <vector>
#include <memory>
/*
����ʹ�����ӳ�
*/
void Test::test(int thread_num, int connection_num) {
    // �̳߳أ�����ÿ�ζ������߳�
    std::vector<std::thread> vec;

    // ��ȡ���ݿ����ӳ�
    ConnectionPool* cp = ConnectionPool::getConnectionPool();

 
    for (int i = 0; i < thread_num; i++) {
        // �����߳�
        std::thread t([connection_num, cp]() {
            for (int i = 0; i < connection_num; ++i) {
                // ���ӳط�ʽ��ȡ���ݿ�����
                char sql[1024] = { 0 };
                sprintf_s(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
                    "zhang san", 20, "male");
                shared_ptr<Connection> sp = cp->getConnection();
                sp->update(sql);
            }
            });

        // ���̼߳����̳߳�
        vec.push_back(std::move(t));
    }

    // �ȴ������߳����
    for (auto& t : vec) {
        if (t.joinable()) {
            t.join();
        }
    }
}