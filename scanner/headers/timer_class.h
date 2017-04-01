#ifndef _TIMER_CLASS_H_
#define _TIMER_CLASS_H_

//========
//includes
#include <windows.h>

//==========
//TimerClass
class TimerClass
{
public:
 TimerClass();
 TimerClass(const TimerClass&);
 ~TimerClass();

 bool Initialize();
 void Frame();
 float Get_time();

private:
 INT64 frequency_;
 float ticks_per_ms_;
 INT64 start_time_;
 float frame_time_;
};
#endif
