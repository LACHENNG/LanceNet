#include "../ThreadPool.h"
#include <atomic>
#include <vector>
#include "../../timer.h"
#include <atomic>

CountDownLatch latch(1);
const int N = 100000;
const int nthreads = 8;
std::atomic_int workdone(0);

void Stop()
{
    latch.countdown();
}

class DirtyWork
{
public:
    DirtyWork(int workid)
      : workid_(workid)
    {

    }

    ~DirtyWork()
    {
//        printf("WARN: ~DirtyWork::DirtyWork, workid: %d\n", workid_);
    }

//    DirtyWork(DirtyWork&& rhs){
 //       workid_ = rhs.workid_;
  //      printf("WARN: DirtyWork::DirtyWork&&, workid: %d\n", workid_);
   // }

    void doWork(){
//        printf("+++From doWork()+++++%d\n", workid_);
        workdone++;   
        if(workdone == N){
            Stop();
        }
    }
private:
    int workid_;
};

void simpleFunc(int i){
//    printf("%d\n",i);
    if(i == N - 1){
        Stop();
    }

}

std::vector<DirtyWork> TaskObjs;
int main()
{
    Timer timer;

    ThreadPool thread_pool(nthreads);
    thread_pool.start();
    timer.update();

    for(int i = 0; i < N; i++){
        TaskObjs.emplace_back(std::move(DirtyWork(i)));
        auto _task = std::bind(&DirtyWork::doWork, &TaskObjs.back());
        thread_pool.doTask(_task);
        // thread_pool.addTask(std::bind(simpleFunc, i));
    }
    printf("waiting for latch\n");
    latch.wait();
    printf("wait done\n");
    printf("stoping thread pool\n");
    thread_pool.stop();
    printf("pool stoped\n");
    printf("done %d dirty works in %lf seconds\n", N, timer.getElapsedSecond());
    return 0;
}

