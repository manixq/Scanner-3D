#ifndef _FPS_CLASS_H_
#define _FPS_CLASS_H_

//=======
//Linking
#pragma comment(lib, "winmm.lib")

//========
//Includes
#include <windows.h>
#include <mmsystem.h>

//========
//FpsClass
class FpsClass
{
public:
 FpsClass();
 FpsClass(const FpsClass&);
 ~FpsClass();

  void Initialize();
  void Frame();
  int Get_fps();

private:
 int fps_, count_;
 unsigned long start_time_;
};
#endif