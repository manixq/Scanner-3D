#ifndef _CPU_CLASS_H_
#define  _CPU_CLASS_H_

//=======
//linking
#pragma comment(lib, "pdh.lib")

//=======
//include
#include <Pdh.h>

//========
//CpuClass
class CpuClass
{
public:
 CpuClass();
 CpuClass(const CpuClass&);
 ~CpuClass();

 void Initialize();
 void Shutdown();
 void Frame();
 int Get_cpu_percentage();

private:
 bool can_read_cpu_;
 HQUERY query_handle_;
 HCOUNTER counter_handle_;
 unsigned long last_sample_time_;
 long cpu_usage_;
};
#endif
