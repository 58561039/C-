#include<string>
//write by lzc
//以日志类为例，练习单例模式中的懒汉模式


class Log{
    public:
        static Log& GetInstance(){
            static Log instance;
            return instance;
        }
        void Print(std::string s){}

    private:
        Log(){}
        Log(const Log& log)=delete;
        Log& operator=(const Log& log)=delete;
};
