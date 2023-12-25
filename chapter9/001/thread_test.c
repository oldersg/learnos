#include<stdlib.h>
#include<pthread.h>

void* thread_func(void *_arg)
{
    unsigned int *arg = _arg;
    printf("new thread: my tid is %u\n",*arg);
}

void main()
{
    pthread_t new_thread_id;
    /*
    参数1用于存储新创建线程的id
    参数2用于指定线程类型
    参数3指定线程所调用的函数的地址
    参数4用于给参数3的函数传参，如果有多个参数，使用结构体封装后传结构体地址
    若返回值为0，则表示创建成功
    */
    pthread_create(&new_thread_id,NULL,thread_func,&new_thread_id);
    printf("main thread: my tid is %u\n",pthread_self());
    usleep(100);
}