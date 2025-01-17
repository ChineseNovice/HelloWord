#include "test.h"
#include"CommonConnectionPool.h"
#include <thread>
#include <vector>
#include <memory>
/*
测试使用链接池
*/
void Test::test(int thread_num, int connection_num) {
    // 线程池，避免每次都创建线程
    std::vector<std::thread> vec;

    // 获取数据库连接池
    ConnectionPool* cp = ConnectionPool::getConnectionPool();

 
    for (int i = 0; i < thread_num; i++) {
        // 创建线程
        std::thread t([connection_num, cp]() {
            for (int i = 0; i < connection_num; ++i) {
                // 连接池方式获取数据库连接
                char sql[1024] = { 0 };
                sprintf_s(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
                    "zhang san", 20, "male");
                shared_ptr<Connection> sp = cp->getConnection();
                sp->update(sql);
            }
            });

        // 将线程加入线程池
        vec.push_back(std::move(t));
    }

    // 等待所有线程完成
    for (auto& t : vec) {
        if (t.joinable()) {
            t.join();
        }
    }
}