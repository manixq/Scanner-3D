#include "headers/fps_class.h"


FpsClass::FpsClass()
{
 
}

FpsClass::FpsClass(const FpsClass&)
{
 
}

FpsClass::~FpsClass()
{
 
}



void FpsClass::Initialize()
{
 fps_ = 0;
 count_ = 0;
 start_time_ = timeGetTime();
}

void FpsClass::Frame()
{
 count_++;
 if(timeGetTime() >= (start_time_ + 1000))
 {
  fps_ = count_;
  count_ = 0;
  start_time_ = timeGetTime();
 }
}

int FpsClass::Get_fps()
{
 return fps_;
}

