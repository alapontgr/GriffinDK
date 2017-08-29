#pragma once
#include "Task.h"

namespace fdk
{
  namespace Threading 
  {
    class Job 
    {
    public:

      Job() : m_totalProgress{ 0 } {}

      virtual ~Job();

      virtual void wait() = 0;

    private:

      ProgressCounter m_totalProgress;
    };
  }
}