#pragma once

namespace fdk
{
  namespace Utilities
  {
    template<typename T>
    class SingletonHolder
    {
    private:
      SingletonHolder() {}

    public:
      static T& instance() {
        static T s_instance;
        return s_instance;
      }

      SingletonHolder(const SingletonHolder&) = delete;

      SingletonHolder& operator = (const SingletonHolder&) = delete;
    };

  } // namespace Utilities
} // namespace fdk