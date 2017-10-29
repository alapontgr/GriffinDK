#include <windows.h>
#include "Entry.h"

int WINAPI WinMain(HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR lpCmdLine,
  int nCmdShow)
{
  return fdk::Framework::__fenix_entry__();
}