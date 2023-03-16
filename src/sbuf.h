/* MT-Safe Circular Buffer with bounded size （Template class) */
/* The easiest way to avoid link errors involving templates is to declare and define them in a single xxx.h file */
#pragma once 
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>

template<typename T>
class SBuf{
public:
    explicit SBuf(size_t capacity = 10000);
    ~SBuf() = default;
    /* insert if have idle slot, suspend otherwise*/
    /* TODO: support move semantic */
    void insert(T item);
    /* remove if buf is not empty, suspend otherwise*/
    T remove();

    /* observer */
    int size();
    bool empty();

private:
    std::vector<T> m_buf;               /* Buffer array */
    size_t m_maximum_slots;             /* Maxinum number of slots(slots limits)*/
    size_t m_front;                     /* m_buf[m_front % n] is the first element*/
    size_t m_rear;                      /* m_buf[(m_rear - 1 + n) % n] is the last element*/
    std::mutex m_mutex;                 /* Protects accesses to buf*/
    std::condition_variable m_cv_slots; /* Scheduling constraints */
    std::condition_variable m_cv_items; /* Scheduling constraints */
    int m_available_slots;              /* Counts available slots*/
    int m_available_items;              /* Counts available items*/  
    std::atomic_int _nsize;
};

template<typename T>
SBuf<T>::SBuf(size_t capacity): m_buf(capacity), m_maximum_slots(capacity), 
                                m_front(0), m_rear(0), m_available_slots(capacity), 
                                m_available_items(0)
{ 
}

template<typename T>
void SBuf<T>::insert(T item){
    std::unique_lock<std::mutex> lk(m_mutex);
    // m_cv_slots.wait(lk, [this]{return m_available_slots > 0;});
    while(m_available_slots <= 0){
        m_cv_slots.wait(lk);
    }

    m_buf[(m_rear) % m_maximum_slots] = std::move(item);
    m_rear = (m_rear + 1) % m_maximum_slots;
    m_available_items++;
    m_available_slots--;

    lk.unlock();             // can not change the two lines code order
    m_cv_items.notify_one(); // TODO: figure out why not right when notify_one 
    
}

template<typename T>
T SBuf<T>::remove(){    
    std::unique_lock<std::mutex> lk(m_mutex);
    // m_cv_items.wait(lk, [this]{return m_available_items > 0;});
    while(m_available_items <= 0){
        m_cv_items.wait(lk);
    }

    T rv = m_buf[(m_front) % m_maximum_slots];
    m_front = (m_front + 1) % m_maximum_slots;
    m_available_slots++; 
    m_available_items--;
    --_nsize;
    lk.unlock();
    m_cv_slots.notify_one();
    
    return rv;
}

template<typename T>
bool SBuf<T>::empty(){
    return _nsize == 0;
}

template<typename T>
int SBuf<T>::size(){
    return _nsize.load();
}