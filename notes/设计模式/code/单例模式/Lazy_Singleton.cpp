#include<string>
//write by lzc
//以日志类为例，练习单例模式中的懒汉模式

//第一种写法
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

//第二种写法
static std::once_flag once;

class LLog{
    public:
        void Print(std::string s){}
        static LLog& GetInstance(){
            std::call_once(once,init);
            return *instance;
        }
        static void init(){
            if(!instance){
                instance=new LLog();
            }
        }
    private:
        LLog(){}
        LLog(const LLog& llog)=delete;
        LLog& operator=(const LLog& llog)=delete;
        static LLog *instance;
};

LLog* LLog::instance=nullptr;
