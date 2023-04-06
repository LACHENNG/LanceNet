# LanceNet: A web library From Scratch


1. concurrent echo server with Select IO multiplexing
 
   a. the main thread  select on read_set to monitor any possible readable events of sockets fds, if listen socket is readalbe, accepts new connection and update read_set. and inform one of the thread in thread pool to do the `echo` job
   the difficulty is how to monitor the events of close when peer close the connect, Which thread updates readsets and closes fd? Because here are serveral race conditions that may occur:
   1) if the peer close the connection,  or a file descriptor being monitored by select()  is  closed  in  another
       thread,  the  result  is  unspecified.   On  some  UNIX systems, select()
       unblocks and returns, with an indication  that  the  file  descriptor  is
       ready  (a subsequent I/O operation will likely fail with an error  
      

# 遇到的问题
 当慢速发送少量数据时，服务器和客户端收发数据正常，当客户端高速向服务器发生大量数据，而接收端每次只读取一个消息（少量）数据，运行一会出现运行变慢甚至阻塞。深入分析原因， `send`, `recv`, `read`, `write` socket其实并不是直接网络发送数据，而是将数据从**应用层缓冲区**拷贝到**内核缓冲区**，一端高速发，另一端低速接收，最终导致接收端内核缓冲区满, 假如我们现在是在使用TCP，由于TCP拥塞控制机制的存在，此时发送方的send_win会慢慢减小
   
   不同的程序进行网络通信时，发送的一方会将内核缓冲区的数据通过网络传输给接收方的内核缓冲区。在应用程序 A 与 应用程序 B 建立了 TCP 连接之后，假设应用程序 A 不断调用 send 函数，这样数据会不断拷贝至对应的内核缓冲区中，如果 B 那一端一直不调用 recv 函数，那么 B 的内核缓冲区被填满以后，A 的内核缓冲区也会被填满，此时 A 继续调用 send 函数会是什么结果呢？ 具体的结果取决于该 socket 是否是阻塞模式。我们这里先给出结论：

1. 当 socket 是阻塞模式的，继续调用 send/recv 函数会导致程序阻塞在 send/recv 调用处。
2. 当 socket 是非阻塞模式，继续调用 send/recv 函数，send/recv 函数不会阻塞程序执行流，而是会立即出错返回，我们会得到一个相关的错误码，Linux 平台上该错误码为 `EWOULDBLOCK` 或 `EAGAIN`（这两个错误码值相同），Windows 平台上错误码为 WSAEWOULDBLOCK。


另外，  细心的读者如果实际去做一下这个实验会发现一个现象，即当 tcpdump 已经显示对端的 TCP 窗口是 0 时， blocking_client 仍然可以继续发送一段时间的数据，此时的数据已经不是在发往对端，而是逐渐填满到本端的内核发送缓冲区中去了，这也验证了 send 函数实际上是往内核缓冲区中拷贝数据这一行为 

【解决方法】 提高客户端和用户端接受数据的大小，我们每次接收大量数据，到用户缓冲区，以便能快速消耗内核缓冲数据防止缓冲区满 
例如:
char userBuf[400*1024]; // 400 kb user buffer 
int nLen = (int) recv(sock, userBuf, 400*1024, 0); 

但这会引入另一个问题， 即`少包`，`粘包`问题，少包问题是我们期望接受n个字节，却只收到少于n个字节，这时候我们要等待并读取更多数据，进行**组包**；粘包是指接受的字节数大于一个message的字节数，也就是数据包含了多个message对象的数据, 我们需要把这些包一个个**拆出来**

性能优化问题：
1. select 第一个参数表示了遍历的空间大小，我们如何高效的记录maxfd？
   1. 传统的方法采用维护maxfd，每次对新加入的fd取max，维护一个全局的可用maxfd，但这种方法只会使得maxfd越来越大，最终导致select底层的线性遍历空间越来越大


遇到的问题：
   如何处理客户端退出事件？简单的采取消息队列的方式让工作线程向`退出消息队列`添加消息，让主线程读取退出消息队列并对fd_set等相关状态更新，但这不是一件容易的任务，极易导致竞争，可能的就有两种：
   1. 消费者worker线程1检测到了客户端退出消息，将该消息加入`退出消息队列`并采用event_fd通知以便下次被select检测到,但就在通知之前，cpu时间片切换到select线程，此时end-of-file同样再次触发reabable事件，此时交由work线程2处理，对同一个fd重复加入'退出消息队列',容易导致select线程重复记数退出客户的数量，并且无法区分这个退出消息是重复的，还是由于文件描述符复用的新连接关闭关闭产生的消息，还是该首次退出的消息。导致处理混乱，`解决方案`: 简单的fd数值无法区分新旧连接的退出消息，我们将fd包装成一个客户端clientfd类，用clientfd*指针进行消息传递，这样每个客户连接都会不同，不会因为fd的数值相同而产生重复操作的风险


## 编译问题
1. 循环引用
```c++
   // A.h
   class B; // forward declaration

   class A {
   public:
      void foo(B& b);
   };

   // B.h
   class A; // forward declaration

   class B {
   public:
      void bar(A& a);
   };

   // A.cpp
   #include "A.h"
   #include "B.h"

   void A::foo(B& b) {
      b.bar(*this);
   }

   // B.cpp
   #include "A.h"
   #include "B.h"

   void B::bar(A& a) {
      a.foo(*this);
   }
```


## 技术总结
Unique/Shared ptr它们的区别在于：

- unique_ptr是独享被管理对象指针所有权的智能指针，不能被复制或赋值14。它适合用于表示一些独一无二的资源，如文件句柄、互斥锁等5。
- shared_ptr是共享被管理对象指针所有权的智能指针，可以被复制或赋值24。它采用引用计数技术来记录有多少个shared_ptr指向同一个对象34。当引用计数变为0时，才会释放对象内存24。它适合用于表示一些可以被多个对象共享的资源，如链表节点、树节点等5。
   
在面向对象编程中，shared_ptr和unique_ptr常见的用法有：

- 作为类成员变量，表示类与其他类之间的关联关系。例如，如果一个类A包含一个unique_ptr<B>类型的成员变量，则表示A拥有B，并且B不能被其他类拥有；如果一个类A包含一个shared_ptr<B>类型的成员变量，则表示A与其他可能拥有B的类共享B。
- 作为函数参数或返回值，表示函数对传入或返回的对象指针的所有权控制。例如，如果一个函数f接受一个unique_ptr<T>类型的参数，则表示f会获取T对象的所有权，并且调用者不能再使用该T对象；如果一个函数f返回一个shared_ptr<T>类型的值，则表示f会创建或共享一个T对象，并且调用者可以继续使用该T对象。


## 问题：如何检测客户端退出
网络编程中，检测socket对应的客户端退出有两种情况：正常退出和异常退出¹。

- 正常退出是指客户端主动关闭连接，发送一个FIN包给服务器。这时，服务器端的socket会收到一个0字节的数据包¹²。因此，服务器可以通过读取socket的返回值来判断是否为0，如果是0，则表示客户端已经正常退出¹²。例如：

int n = read(socket_fd, buffer, size); // 读取socket数据
if (n == 0) {
  // 客户端已经正常退出
}

- 异常退出是指客户端由于网络故障或其他原因断开连接，没有发送FIN包给服务器。这时，服务器端的socket无法通过读取数据来检测客户端的状态¹。因此，服务器需要通过写入socket来判断是否出现错误¹³。如果写入失败，并且错误码为EPIPE或ECONNRESET，则表示客户端已经异常退出¹³。例如：
```c++
int n = write(socket_fd, buffer, size); // 写入socket数据
if (n == -1 && (errno == EPIPE || errno == ECONNRESET)) {
  // 客户端已经异常退出
}
```



# 内存管理
   减少内存碎片的产生，让程序长期稳定运行

1. 内存池
   - 从内存申请足够大小的内存让程序自己管理
2. 对象池
   -  一次创建足够多的对象，减少频繁创建过程的消耗
3. 智能指针
   - 保证被创建的对象正确的释放
   
1.内存池的必要性

  1. 减少频繁陷入系统调用，减少产生的内存碎片和提高运行效率
  2. 精细控制new/delete, 方便内存管理
 
 **技术**
   1. 重载operator new 和 operator delete 
   2. 在重载版本的版本使用自己的内存管理类
   3. 管理多个不同大小的内存单元（为什么要多个？） 
   ```c++
   MemoryMgr::instance().alloc(nSize);
   MemoryMgr::instance().free(nSize);
   ```
![image](images/MemoryPool.png)
