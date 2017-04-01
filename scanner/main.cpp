#include "headers/system_class.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
 SystemClass* system;
 bool result;

 //Creating system object
 system = new SystemClass;
 if(!system)
 {
  return 0;
 }

 //Initializing system object
 result = system->Initialize();
 if(result)
 {
  system->Run();
 }

 //Shutdown with release
 system->Shutdown();
 delete system;
 system = nullptr;

 return 0;
}