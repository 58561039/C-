#include<iostream>
#include<mutex>
#include<condition_variable>
#include<thread>
#include<queue>
//write by lzc
//生产者-消费者模型

std::mutex mtx;//互斥量
std::condition_variable conv;//条件变量
std::queue<int> task_queue;//任务队列

void Producer(){
    for(int i=0;i<10;i++){
        {
            std::unique_lock<std::mutex> umtx(mtx);//互斥锁，让生产者和消费者互斥访问任务队列
            task_queue.push(i);
            conv.notify_one();//通知
            std::cout<<"Producer:"<<i<<std::endl;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(100));//暂停100ms
    }
}

void Consumer(){
    while(1){
        std::unique_lock<std::mutex> umtx(mtx);//互斥锁
        conv.wait(umtx,[](){
            return !task_queue.empty();
        });
        int value=task_queue.front();
        task_queue.pop();
        std::cout<<"Consumer:"<<value<<std::endl;
    }
}

int main(){
    std::thread t1(Producer);
    std::thread t2(Consumer);

    t1.join();
    t2.join();

    return 0;
}
