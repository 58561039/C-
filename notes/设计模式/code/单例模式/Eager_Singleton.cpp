#include<iostream>
#include<string>
//write by lzc
//以日志类为例，练习单例模式中的饿汉模式

class Log{
    private:
        Log(){}//构造函数
        Log(const Log& log)=delete;//复制构造函数被删除
        Log& operator=(const Log& log)=delete;//用operator重载=也不行
        
        static Log instance;//静态实例，在事先就声明


    public:
        void Print(std::string s){}//打印日志
        static Log& GetInstance(){//获取实例
            return instance;
        }
};
