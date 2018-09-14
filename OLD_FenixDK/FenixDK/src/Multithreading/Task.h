#pragma once

#include <atomic> 

namespace fdk
{
  namespace Threading 
  {
    using ProgressCounter = std::atomic_uint32_t;

    class Task 
    {
    public:

      Task();

      virtual ~Task();

      void set_progress_counter(ProgressCounter* pCounter) { m_totalProgress = pCounter; }

      virtual void execute() = 0;

    private:

      ProgressCounter* m_totalProgress;
    };
  }
}