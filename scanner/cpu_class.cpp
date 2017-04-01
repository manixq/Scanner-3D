#include "headers/cpu_class.h"

CpuClass::CpuClass()
{
 
}

CpuClass::CpuClass(const CpuClass&)
{
 
}

CpuClass::~CpuClass()
{
 
}

void CpuClass::Initialize()
{
 PDH_STATUS status;

 can_read_cpu_ = true;

 status = PdhOpenQuery(nullptr, 0, &query_handle_);
 if (status != ERROR_SUCCESS)
  can_read_cpu_ = false;

 status = PdhAddCounter(query_handle_, TEXT("\\Processor(_Total)\\% procesor time"), 0, &counter_handle_);
 if (status != ERROR_SUCCESS)
  can_read_cpu_ = false;

 last_sample_time_ = GetTickCount();

 cpu_usage_ = 0;
}

void CpuClass::Shutdown()
{
 if (can_read_cpu_)
  PdhCloseQuery(query_handle_);
}

void CpuClass::Frame()
{
 PDH_FMT_COUNTERVALUE value;

 if(can_read_cpu_)
 {
  if((last_sample_time_ + 1000) < GetTickCount())
  {
   last_sample_time_ = GetTickCount();
   PdhCollectQueryData(query_handle_);
   PdhGetFormattedCounterValue(counter_handle_, PDH_FMT_LONG, nullptr, &value);
   cpu_usage_ = value.longValue;
  }
 }
}

int CpuClass::Get_cpu_percentage()
{
 int usage;
 if (can_read_cpu_)
  usage = static_cast<int>(cpu_usage_);
 else
  usage = 0;

 return usage;
}
