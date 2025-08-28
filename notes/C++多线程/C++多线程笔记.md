## 一、thread基础

### 1.用thread库函数创建线程，调用线程



包含头文件thread

定义一个线程，选择该线程执行的函数，传参数

线程对象的join函数：主线程需要等待该线程执行结束之后才可以从join处继续执行。

joinable函数返回一个bool值，为1时，可以调用join和detach函数，否则不能调用

detach函数：将主线程与子线程分离，不管有没有执行完，都会继续向下执行。

```C++
#include<iostream>
#include<thread>
#include<string>
using namespace std;

void Output(string s){
    cout<<s<<endl;
    return;
}

int main(){
    thread t1(Output,"lzc");
    bool isJoin=t1.joinable();
    if(isJoin){
        t1.join();
    }
    cout<<"over"<<endl;
    return 0;
}
```

### 2.当函数的参数是引用

thread t1(worker,a,b,c);

这种方式本质是将a，b，c拷贝一份，然后线程根据这些参数运行worker，但如果worker的参数是引用类型，worker中修改了参数，但主线程无法访问到。

因为thread不支持传引用，本质传参数是拷贝。

所以需要用到一个函数：std::ref()，该函数可以把一个参数包装成引用，从而避免thread拷贝参数的特点。

```C++
#include<iostream>
#include<thread>
using namespace std;
//write by lzc 

void worker(int& a){
    a++;
}

int main(){
    int a=1;
    thread t1(worker,std::ref(a));
    t1.join();
    cout<<a<<endl;//输出a=2
}
```

std::cref()传的是const的引用

### 3.当函数的参数是指针，优先考虑智能指针

如果需要参数的是对象，传递参数的方式是引用时，无法保证始终有效

因为使用了智能指针，不需要考虑delete，如果传的参数是引用，可能会因为局部变量的问题导致出错，如果传递的是普通指针，可能会导致指针指向的位置被释放，虽然地址有效，但指向的内容已经失效，所以智能指针是更好的选择。

```C++
class myclass{
    public:
        void foo(){
            cout<<"foo"<<endl;
        }
};

int main(){
    shared_ptr<myclass> a=make_shared<myclass>();//使用智能指针

    thread t1(&myclass::foo,a);

    t1.join();
}
```

## 二、互斥量

### 1.互斥量基础

使用互斥锁

一个线程对于一段代码加锁之后，其他线程无法访问这段代码（临界区）

下列代码是手动添加锁，释放锁

```C++
#include<iostream>
#include<thread>
#include<mutex>
using namespace std;

int a=0;
mutex mtx;

void worker(){
    for(int i=0;i<10000;i++){
        mtx.lock();
        a++;
        mtx.unlock();
    }
}


int main(){
    thread t1(worker);
    thread t2(worker);
    
    t1.join();
    t2.join();

    cout<<a<<endl;
}
```

### 2.RAII机制封装基础

可以使用RAII机制

lock_guard在调用构造函数时会自动上锁，当调用析构函数时会自动解锁

毕竟使用有点像智能指针，指针的new和delete，锁的lock与unlock，都可以使用RAII机制

```C++
lock_guard<mutex> lock(mtx);//加锁，作用域内有效，出了作用域自动释放锁，不需要手动释放

//例如
for(int i=0;i<1000;i++){
    lock_guard<mutex> lock(mtx);//从这里上锁
    dosomething;
}//出了作用域立刻释放
```

### 3.死锁

互斥

不剥夺

请求和保持

循环等待

```C++
mutex m1,m2;

void worker1(){
    for(int i=0;i<50;i++){
        m1.lock();
        m2.lock();
        m1.unlock();
        m2.unlock();
    }
}

void worker2(){
    for(int i=0;i<50;i++){
        m2.lock();
        m1.lock();
        m1.unlock();
        m2.unlock();
    }
}

int main(){
    thread t1(worker1);
    thread t2(worker2);

    t1.join();
    t2.join();

    cout<<"over"<<endl;
}
```

该程序会发生死锁

进程1占用m1，等待m2

进程2占用m2，等待m1

### 4.所有控制互斥锁的RAII封装

#### (1)lock_guard

简单的对于互斥锁的封装

|函数|功能|
|-|-|
|`lock()`|上锁|
|`unlock()`|解锁|
|`try_lock()`|尝试上锁|


```C++
    mutex m2;
    lock_guard<mutex> lgm(m2);//这里就直接上锁，作用域内有效，出作用域自动调用析构函数解锁
    
    m2.lock();
    lock_guard<mutex> lgm(m2,std::adopt_lock);
    //通过std::adopt_lock，告诉lock_guard无需上锁，是拿了一个已经上锁的互斥量
```

#### (2)unique_lock

相比于lock_guard，提供更多功能

|函数|功能|
|-|-|
|`lock()`|上锁|
|`unlock()`|解锁|
|`try_lock()`|尝试上锁|
|`try_lock_for()`|规定时间内尝试上锁|
|`try_lock_until()`|截止时间内尝试上锁|
|`onws_lock()`|当前是否持有互斥量|
|`mutex()/native_handle()`|获取原生mutex对象|


```C++
    unique_lock<mutex> umtx1;
    unique_lock<mutex> umtx2(m2,defer_lock);
    
    umtx1=std::move(umtx2);//支持右值的传递，原来的umtx2就不再持有锁
```

```C++
    unique_lock<mutex> umtx1(m1,defer_lock);
    unique_lock<mutex> umtx2(m2,defer_lock);

    umtx1.swap(umtx2);
```

#### (3)scoped_lock

#### (4)shared_lock

### 5.所有的互斥量

#### (1)mutex

|函数|功能|备注|
|-|-|-|
|`lock()`|阻塞并加锁，直到成功|如果已经被锁住，会阻塞当前线程|
|`try_lock()`|尝试加锁，如果锁已经被占用立即返回`false`|非阻塞|
|`unlock()`|解锁|必须在持有锁的线程调用，否则未定义行为|


#### (2)recursive_mutex

#### (3)timed_mutex

|函数|功能|备注|
|-|-|-|
|`lock()`|阻塞并加锁，直到成功|如果已经被锁住，会阻塞当前线程|
|`unlock()`|解锁|必须在持有锁的线程调用，否则未定义行为|
|`try_lock()`|尝试加锁，如果锁已经被占用立即返回`false`，上锁成功返回`true`|非阻塞|
|`try_lock_for()`|给一个参数，表示规定时间，在规定时间内如果没有上锁成功，会返回`false`，上锁成功返回`true`|阻塞|
|`try_lock_until()`  |指定时间点，在截止时间点之前上锁，上锁成功返回`true`，失败返回`false`|阻塞|


```C++
    timed_mutex tmx;
    tmx.lock();
    tmx.try_lock();
    tmx.try_lock_for(chrono::seconds(5));
    tmx.try_lock_until(chrono::steady_clock::now() + chrono::seconds(5));
```

#### (4)recursive_timed_mutex

#### (5)shared_timed_mutex



## 三、单例模式

### 1.饿汉模式

```C++
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
```

### 2.懒汉模式

```C++
#include<iostream>
#include<string>
//write by lzc
//以日志类为例，练习单例模式中的懒汉模式


class Log{//第一种写法
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

```

懒汉模式的第二种写法：

```C++
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
```

`std::one_flag`定义一个量

`std::call_once`(变量名，函数)，代表多个线程同时指向该函数，只执行一次。



## 四、条件变量

### 1.条件变量

```C++
#include<condition_variable>
std::condition_variable conv;
```

|函数|功能|
|-|-|
|wait(unique_lock)/带谓词wait(unique_lock,pre)|释放互斥锁，等待被唤醒，被唤醒之后再申请互斥锁/谓词是一个函数，为了防止假唤醒，需要调用谓词判断|
|wait_for(unique_lock,time)/带谓词wait(unique_lock,time,pre)|释放互斥锁，在指定时间内等待被唤醒，超时会自动执行下面代码，被强制唤醒（先获得互斥锁再执行下面代码）。|
|wait_until(unique_lock,time_point)/带谓词wait(unique_lock,time_point,pre)|在截止时间前等待被唤醒|
|notify_one()|唤醒一个正在等待被唤醒的线程|
|notify_all()|唤醒所有正在等待被唤醒的线程|


### 2.生产者-消费者模型

```C++
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
```

## 五、线程池

```C++
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
```

### 1.线程池包含内容

线程数组

任务队列

判断线程池是否停止的标志位

互斥量

条件变量

构造函数

析构函数

添加任务的外部接口





### 2.线程池构造函数

```C++
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
```

构造函数功能：将`stop`置为0，标志线程池正在运行

for循环创建线程，用`vector`中的`emplace_back`，而非`push_back`

每个线程是一个具体的函数，所以插入的是一个lambda表达式，因为线程执行过程中，访问任务队列和`stop`，需要互斥访问

先上锁，然后检查`wait`，`wait`的作用是先释放互斥锁，等待收到条件变量通知后再申请上锁，同时检查lambda表达式看是否是假唤醒。

再检查该唤醒是有任务时的唤醒还是线程池结束时的唤醒

再从任务队列中取出任务，用`function`对象绑定该任务函数

解锁，因为各个任务的执行要保证并行

执行



### 3.线程池析构函数

首先需要上锁修改`stop`

然后通知所有线程安全退出

最后主线程阻塞，等待所有线程结束

```C++
        ~Thread_Pool(){//析构函数
            {
                std::unique_lock<std::mutex> lock(mtx);//上锁，修改stop为1，即线程池停止运行
                stop=true;
            }
            conv.notify_all();//通知所有线程安全退出

            for(auto &t:thread_vector){//等待线程执行完毕，结束
                t.join();
            }
        }
```

### 4.线程池添加任务

`std::bind`：将一个函数对象和对应的参数绑定起来，生成一个新的对象

```C++
#include<iostream>
#include<utility>
#include<functional>

int add(int a,int b){
    return a+b;
}

int main(){
    auto f=std::bind(add,2,3);
    std::cout<<f();//cout 5
}
```

`std::forward` 完美转发

函数参数是左值还是右值，不变的转发到后续的执行中

```C++
#include<iostream>
#include<utility>
using namespace std;

void Process(int &a){//左值引用
    cout<<"left : "<<a<<endl;
}

void Process(int &&a){//右值引用
    cout<<"right : "<<a<<endl;
}

template<typename T>//两个取地址符号配合模板，万能引用
void mid(T &&a){
    Process(std::forward<T>(a));
}


int main(){
    int a=20;
    mid(a);
    mid(40);
}

/*
输出
left : 20
right : 40
*/
```

```C++
void Process(int &a){//左值引用
    cout<<"left : "<<a<<endl;
}
```

```C++
void Process(int &&a){//右值引用
    cout<<"right : "<<a<<endl;
}
```

```C++
template<typename T>//两个取地址符号配合模板，万能引用
void mid(T &&a){
    Process(std::forward<T>(a));
}
```

如果只有两个&，是右值引用，但是配合模板`template`，就是万能引用，同时参数也要使用`std::forward`

std::forward<T>的作用是将参数按照原样（左值/右值）转发给其他函数



```C++
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
```

### 5.线程池扩展

#### (1)单例模式线程池


## 五、异步并发

### 1.async，future

`std::future`是一个容器，<返回值类型>

存储的内容是一个线程执行的结果，会在另一个线程计算结果，然后装在容器里等待被使用

`std::async`是一个启动一个异步任务的工具，这两个搭配使用，调用`std::async`指定异步线程执行的内容和方式

该函数第一个参数指定立即启动(`std::launch::async`)，还是等后续容器调用`get()`获得结果之后才启动—延迟启动(`std::launch::deferred`)

```C++
#include<iostream>
#include<future>
using namespace std;

int func(){
    int i=0;
    for(i=0;i<1000;i++);
    return i;
}

int main(){
    std::future<int> futrue_result=std::async(std::launch::async,func);

    cout<<func()<<endl;
    cout<<futrue_result.get()<<endl;
}
```

### 2.package_task

`package_task`的作用是将一个函数对象打包，生成一个新的对象

可以调用该对象的`get_future`函数返回一个`future`对象(异步线程结果存储容器)，并且需要手动创建线程，控制线程的`join`

和`async`方法相比，这个方式更灵活。

`async`方法要么立即执行，要么等到需要的时候，调用`get`的时候再执行

而该方式可以控制执行的实际

```C++
#include<iostream>
#include<future>
using namespace std;

int func(){
    int i=0;
    for(i=0;i<1000;i++);
    return i;
}

int main(){

    std::packaged_task<int(void)> task(func);//将函数包装为一个task

    auto future_result=task.get_future();//获得该包装的future对象
    std::thread t1(std::move(task));//异步线程执行该函数

    cout<<func()<<endl;
    t1.join();
    cout<<future_result.get()<<endl;
}
```

### 3.promise

线程间通信，可以使用`promise`

可以理解为，设置好`promise`，设置对应的`future`，将`promise`传递，对方线程修改结果

该线程从`future`获取结果

这是在线程运行过程中获取的

```C++
#include<iostream>
#include<future>
using namespace std;

void func(std::promise<int> &prom){
    /*线程自己一大堆计算*/
    prom.set_value(45);//调用promise::set_value修改值
}

int main(){//主线程和子线程之间通信
    std::promise<int> prom;
    std::future<int> task=prom.get_future();//从future中获取结果
    std::thread t1(func,std::ref(prom));
    cout<<task.get()<<endl;
    t1.join();
}
```

### 4.atomic

用`atomic`定义的变量，修改和读取是原子操作，效果类似于有一个互斥锁保护，但是实际效率要更高，速度更快

```C++
#include<iostream>
#include<atomic>
#include<thread>
using namespace std;

atomic<int> tmp;

void func(){
    for(int i=0;i<1000;i++){
        tmp++;
    }
}

int main(){
    thread t1(func);
    thread t2(func);
    t1.join();
    t2.join();
    cout<<tmp<<endl;
}
```

`atomic`还有读写函数，保证原子操作

