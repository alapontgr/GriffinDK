#pragma once

#include "Task.h"
#include "Utilities\types.h"

namespace fdk
{
  namespace Threading 
  {
    void init_job_manager();

    void stop_job_manager();

    void queue_task(Task* pTask);

    u32 worker_thread_count();
  }
}