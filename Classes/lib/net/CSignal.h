//
//  CSignal.h
//  TestCocos2dx
//
//  Created by othink on 15/8/6.
//
//

#ifndef _CSignal_
#define _CSignal_

#include <stddef.h>    /* for NULL */
#include <signal.h>
#include <assert.h>

static inline void sighandler( int signum );
/**
 * @brief The sig_action struct
 * 存储回调对象指针及回调函数
 */
struct sig_action{
    void *object;
    void (*cb)( void *object,int signum );
};

/**
 * @brief The CSignal class
 * 对sigaction封装的C++类
 */
class CSignal{
public:
    /**
     * @brief CSignal::CSignal
     * 初始化信号回调结构体
     */
    explicit CSignal(){
        for ( int i = 0;i < NSIG;i ++ ){
            m_actions[i] = NULL;
        }
    }
    /**
     * @brief CSignal::~CSignal
     * 销毁信号回调结构体
     */
    ~CSignal(){
        for ( int i = 0;i < NSIG;i ++ ){
            if ( m_actions[i] ){
                delete m_actions[i];
                m_actions[i] = NULL;
            }
        }
    }
    
    /**
     * @brief CSignal::reset
     * @param signum
     * @return 返回值同sigaction
     * 重置信号为默认
     */
    int reset( int signum ){
        struct sigaction sa;
        sa.sa_handler = SIG_DFL;
        sigemptyset( &sa.sa_mask );
        
        return sigaction( signum,&sa,0 );
    }
    /**
     * @brief CSignal::ignore
     * @param signum
     * @return  返回值同sigaction
     * 忽略信号
     */
    int ignore( int signum ){
        struct sigaction sa;
        sa.sa_handler = SIG_IGN;
        sigemptyset( &sa.sa_mask );
        return sigaction( signum,&sa,0 );
    }
    /**
     * @brief CSignal::feed_signal
     * @param signum
     * 触发回调
     */
    void feed_signal( int signum ){
        struct sig_action *sac = m_actions[signum];
        assert( NULL != sac );
        
        sac->cb( sac->object,signum );
    }
    /**
     * @brief CSignal::set
     * 设置信号对象回调
     */
    template<class K,void (K::*pf)(int)>
    int set( K *object,int signum,int sa_flag ){
        assert( 0 < signum && NSIG > signum ); /* valid signals range from 1 to NSIG-1 */
        
        struct sigaction sa;
        
        sa.sa_handler = sighandler;
        sigemptyset( &sa.sa_mask );     /* will not block any other signal */
        sa.sa_flags = sa_flag;          /* usually SA_RESTART or SA_RESETHAND,man sigaction for more */
        
        if ( NULL == m_actions[signum] ) /* first init */
        m_actions[signum] = new struct sig_action;
        
        m_actions[signum]->object = (void*)object;
        m_actions[signum]->cb = method_thunk<K,pf>;
        
        return sigaction( signum,&sa,0 );
    }
private:
    /* NSIG The value of this symbolic constant is the total number of signals defined.
     Since the signal numbers are allocated consecutively, NSIG is also one greater
     than the largest defined signal number.
     define in sys/signal.h
     */
    struct sig_action* m_actions[NSIG];
    
    template<class K, void (K::*method)(int)>
    static void method_thunk ( void *object,int signum ){
        (static_cast<K *>(object)->*method)(signum);
    }
};

/*  global funtion and object */

static class CSignal signal_watcher;
static inline class CSignal *default_signal_watcher(){
    return ((class CSignal*)&signal_watcher);
}

#define CSIG_DEFAULT default_signal_watcher()
/**
 * @brief sighandler
 * @param signum
 * 信号回调函数主入口
 */
static inline void sighandler( int signum ){
    CSIG_DEFAULT->feed_signal( signum );
}

#endif
