

#include "pch.h"
#include "CommonConnectionPool.h"
#include "public.h"

// �̰߳�ȫ���������������ӿ�
ConnectionPool* ConnectionPool::getConnectionPool()
{
	static ConnectionPool pool; // ��̬��Ա�����������Զ�lock��unlock
	return &pool;
}

// �������ļ��м���������
bool ConnectionPool::loadConfigFile()
{
	FILE* pf = nullptr;
	errno_t err = fopen_s(&pf, "information.ini", "r");
	if (err != 0 || pf == nullptr)
	{
		LOG("information.ini file does not exist!");
		return false;
	}


	while (!feof(pf))	//��ȡ���ã�ע�������ļ�ÿ�к��\n
	{
		char line[1024] = { 0 };
		fgets(line, 1024, pf);
		string str = line;
		int idx = str.find('=', 0);
		if (idx == -1) // ��Ч��������
		{
			continue;
		}

		int endidx = str.find('\n', idx);
		string key = str.substr(0, idx);
		string value = str.substr(idx + 1, endidx - idx - 1);

		if (key == "ip")
		{
			_ip = value;
		}
		else if (key == "port")
		{
			_port = atoi(value.c_str());
		}
		else if (key == "username")
		{
			_username = value;
		}
		else if (key == "password")
		{
			_password = value;
		}
		else if (key == "dbname")
		{
			_dbname = value;
		}
		else if (key == "initSize")
		{
			_initSize = atoi(value.c_str());
		}
		else if (key == "maxSize")
		{
			_maxSize = atoi(value.c_str());
		}
		else if (key == "maxIdleTime")
		{
			_maxIdleTime = atoi(value.c_str());
		}
		else if (key == "connectionTimeOut")
		{
			_connectionTimeout = atoi(value.c_str());
		}
	}
	return true;
}

// ���ӳصĹ���
ConnectionPool::ConnectionPool()
{
	// ������������
	if (!loadConfigFile())
	{
		return;
	}

	// ������ʼ����������
	for (int i = 0; i < _initSize; ++i)
	{
		Connection* p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		p->refreshAliveTime(); // ˢ��һ�¿�ʼ���е���ʼʱ��
		_connectionQue.push(p);
		_connectionCnt++;
	}

	// ����һ���µ��̣߳���Ϊ���ӵ������� linux thread => pthread_create
	thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
	produce.detach();

	// ����һ���µĶ�ʱ�̣߳�ɨ�賬��maxIdleTimeʱ��Ŀ������ӣ����ж��ڵ����ӻ���
	thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
	scanner.detach();
}

// �����ڶ������߳��У�ר�Ÿ�������������
void ConnectionPool::produceConnectionTask()
{
	for (;;)
	{
		unique_lock<mutex> lock(_queueMutex);	//�������Ӷ��в���һ��Ҫ����������֤��ȫ
		while (!_connectionQue.empty())
		{
			cv.wait(lock); // ���в��գ��˴������߳̽���ȴ�״̬
		}

		// ��������û�е������ޣ����������µ�����
		if (_connectionCnt < _maxSize)
		{
			Connection* p = new Connection();
			p->connect(_ip, _port, _username, _password, _dbname);
			p->refreshAliveTime(); // ˢ��һ�¿�ʼ���е���ʼʱ��
			_connectionQue.push(p);
			_connectionCnt++;
		}

		// ֪ͨ�������̣߳���������������
		cv.notify_all();
	}
}

// ���ⲿ�ṩ�ӿڣ������ӳ��л�ȡһ�����õĿ�������
shared_ptr<Connection> ConnectionPool::getConnection()
{
	unique_lock<mutex> lock(_queueMutex);
	while (_connectionQue.empty())
	{
		// ��������sleep��sleepһ˯���ѣ���Ҫ���ǿ�����ʱ����
		if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeout)))
		{
			if (_connectionQue.empty())
			{
				LOG("��ȡ�������ӳ�ʱ��...��ȡ����ʧ��!");
				return nullptr;
			}
		}
	}

	/*
	shared_ptr����ָ������ʱ�����connection��Դֱ��delete�����൱��
	����connection������������connection�ͱ�close���ˡ�
	������Ҫ�Զ���shared_ptr���ͷ���Դ�ķ�ʽ��������push��queue����
	*/
	shared_ptr<Connection> sp(_connectionQue.front(),
		[&](Connection* pcon) {
			// �������ڷ�����Ӧ���߳��е��õģ�����һ��Ҫ���Ƕ��е��̰߳�ȫ����
			unique_lock<mutex> lock(_queueMutex);
			pcon->refreshAliveTime(); // ˢ��һ�¿�ʼ���е���ʼʱ��
			_connectionQue.push(pcon);
		});

	_connectionQue.pop();
	cv.notify_all();  // �����������Ժ�֪ͨ�������̼߳��һ�£��������Ϊ���ˣ��Ͻ���������

	return sp;
}

// ɨ�賬��maxIdleTimeʱ��Ŀ������ӣ����ж��ڵ����ӻ���
void ConnectionPool::scannerConnectionTask()
{
	for (;;)
	{
		// ͨ��sleepģ�ⶨʱЧ��
		this_thread::sleep_for(chrono::seconds(_maxIdleTime));

		// ɨ���������У��ͷŶ��������
		unique_lock<mutex> lock(_queueMutex);
		while (_connectionCnt > _initSize)
		{
			Connection* p = _connectionQue.front();
			if (p->getAliveeTime() >= (_maxIdleTime * 1000))
			{
				_connectionQue.pop();
				_connectionCnt--;
				delete p; // ���������ͷ�����
			}
			else
			{
				break; // ��ͷ������û�г���_maxIdleTime���������ӿ϶�û��
			}
		}
	}
}