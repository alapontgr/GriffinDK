#pragma once

#include "Utilities/VulkanInclude.h"

namespace fdk
{
  namespace Rendering 
  {
  
    template<class T, class F>
    class AutoDeleter {
    public:
      AutoDeleter() :
        Object(VK_NULL_HANDLE),
        Deleter(nullptr),
        Device(VK_NULL_HANDLE) {
      }

      AutoDeleter(T object, F deleter, VkDevice device) :
        Object(object),
        Deleter(deleter),
        Device(device) {
      }

      AutoDeleter(AutoDeleter&& other) {
        *this = std::move(other);
      }

      ~AutoDeleter() {
        release();
      }

      AutoDeleter& operator=(AutoDeleter&& other) {
        if (this != &other) {
          Object = other.Object;
          Deleter = other.Deleter;
          Device = other.Device;
          other.Object = VK_NULL_HANDLE;
        }
        return *this;
      }

      T get() {
        return Object;
      }

      void release() 
      {
        if ((Object != VK_NULL_HANDLE) && (Deleter != nullptr) && (Device != VK_NULL_HANDLE)) {
          Deleter(Device, Object, nullptr);
          Object = VK_NULL_HANDLE;
        }
      }

      bool operator !() const {
        return Object == VK_NULL_HANDLE;
      }

    private:
      AutoDeleter(const AutoDeleter&);
      AutoDeleter& operator=(const AutoDeleter&);
      T         Object;
      F         Deleter;
      VkDevice  Device;
    };

  }
}