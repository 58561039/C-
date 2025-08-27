#include<iostream>
#include<mutex>
#include<condition_variable>
#include<thread>
#include<vector>
#include<queue>
#include<functional>

class Thread_Pool{
    public:
        Thread_Pool(int num):stop(false){
            for(int i=0;i<num;i++){
                thread_vector.emplace_back([this]{
                    while(1){
                        std::unique_lock<std::mutex> lock(mtx);
                        conv.wait(lock,[this]{
                            return !tasks.empty()||stop;
                        });
                        if(tasks.empty()&&stop){
                            return;
                        }
                        std::function<void()> task(std::move(tasks.front()));
                        tasks.pop();
                        lock.unlock();
                        task();
                    }
                });
            }
        }

        ~Thread_Pool(){//析构函数
            {
                std::unique_lock<std::mutex> lock(mtx);//上锁，修改stop为1，即线程池停止运行
                stop=true;
            }
            conv.notify_all();//？

            for(auto &t:thread_vector){//等待线程执行完毕，结束
                t.join();
            }
        }

        template<class T,class...Args>
        void enqueue(T &&t,Args&&...args){//添加任务
            std::function<void()> task=
            std::bind(std::forward<T>(t),std::forward<Args>(args)...);
            {
                std::unique_lock<std::mutex> lock(mtx);
                tasks.emplace(std::move(task));
            }
            conv.notify_one();
        }
    private:
        std::queue<std::function<void()>> tasks;//任务队列
        std::vector<std::thread> thread_vector;//线程数组
        std::mutex mtx;//互斥信号量
        std::condition_variable conv;//条件变量

        bool stop;//用来判别线程池是否停止运行
};


int main(){
    Thread_Pool pool(5);
    for(int i=0;i<10;i++){
        pool.enqueue([i]{
            std::cout<<"Task : "<<i<<"start now"<<std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout<<"Task : "<<i<<"done now"<<std::endl;
        });
    }
    return 0;
}
