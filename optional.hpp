#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP

#include <stdexcept>
#include <cassert>
#include "aligned_storage.hpp"

#define MY_NOEXCEPT throw()

namespace my
{
  template <class T>
  struct DynamicStoragePolicy
  {
  private:
    T* data;
  public:
    DynamicStoragePolicy(): data(NULL) {}
    template <class U>
    DynamicStoragePolicy(const U& val): data(new T(val)) {}
  public:
    template <class U>
    void construct(const U& val)
    {
      assert(!data);
      data = new T(val);
    }
    void destruct() MY_NOEXCEPT
    {
      assert(data);
      delete data;
      data = NULL;
    }
    T* address() MY_NOEXCEPT { return data; }
    const T* address() const MY_NOEXCEPT { return data; }
    bool is_constructed() const MY_NOEXCEPT { return data; }
  };

  template <class T>
  struct LocalStoragePolicy
  {
  private:
    type_traits::aligned_storage<sizeof(T), type_traits::alignment_of<T>::value> storage;
    T* data; // for avoiding UB, std::launder is needed to remove this redundant pointer
  public:
    LocalStoragePolicy(): data(NULL) {}
    template <class U>
    LocalStoragePolicy(const U& val): data(new(storage.address()) T(val)) {}
  public:
    template <class U>
    void construct(const U& val)
    {
      assert(!data);
      data = new(storage.address()) T(val);
    }
    void destruct() MY_NOEXCEPT
    {
      assert(data);
      data->~T();
      data = NULL;
    }
    T* address() MY_NOEXCEPT { return data; }
    const T* address() const MY_NOEXCEPT { return data; }
    bool is_constructed() const MY_NOEXCEPT { return data; }
  };

  template <class T, template <class> class StoragePolicy = LocalStoragePolicy>
  class optional: private StoragePolicy<T>
  {
  private:
    // for conversion to unspecified bool idiom
    typedef void (optional::*bool_type)(optional&);
  public:
    typedef T value_type;
  public:
    void reset() MY_NOEXCEPT
    {
      if (this->is_constructed()) this->destruct();
    }
    T& operator*() MY_NOEXCEPT
    {
      assert(this->is_constructed() && "null optional dereferenced");
      return *this->address();
    }
    const T& operator*() const MY_NOEXCEPT
    {
      assert(this->is_constructed() && "null optional dereferenced");
      return *this->address();
    }
    T* operator->() MY_NOEXCEPT { return this->address(); }
    const T* operator->() const MY_NOEXCEPT { return this->address(); }
    bool has_value() const MY_NOEXCEPT { return this->is_constructed(); }
    T& value()
    {
      if (this->is_constructed()) return *this->address();
      else throw std::runtime_error("null optional dereferenced");
    }
    const T& value() const
    {
      if (this->is_constructed()) return *this->address();
      else throw std::runtime_error("null optional dereferenced");
    }
    template <class U>
    T value_or(const U& val) const { return this->has_value() ? *(this->address()) : val; }
    operator bool_type() const MY_NOEXCEPT { return this->has_value() ? &optional::swap : 0; }
    // TODO make this swap exception safe
    void swap(optional& other)
    {
      using std::swap;
      if (this->is_constructed() && other.is_constructed())
      {
        swap(*(*this), *other);
      }
      else if (!this->is_constructed() && other.is_constructed())
      {
        this->construct(*other);
        other.destruct();
      }
      else if (this->is_constructed() && !other.is_constructed())
      {
        other.construct(*(*this));
        this->destruct();
      }
    }

  public:
    optional() {}
    template <class U>
    optional(const U& val): StoragePolicy<T>(val) {}
    optional(const optional& other)
    {
      if (other) this->construct(*other);
    }
    template <class U, template <class> class OtherStoragePolicy>
    optional(const optional<U, OtherStoragePolicy>& other)
    {
      if (other) this->construct(*other);
    }
    template <class U>
    optional& operator=(const U& val)
    {
      if (this->has_value()) *(this->address()) = val;
      else this->construct(val);
      return *this;
    }
    optional& operator=(const optional& other)
    {
      if (this->has_value() && other.has_value())
      {
        *(*this) = *other;
      }
      else if (!this->has_value() && other.has_value())
      {
        this->construct(*other);
      }
      else if (this->has_value() && !other.has_value())
      {
        this->destruct();
      }
      return *this;
    }
    template <class U, template <class> class OtherStoragePolicy>
    optional& operator=(const optional<U, OtherStoragePolicy>& other)
		{
			if (this->has_value() && other.has_value())
			{
				*(*this) = *other;
			}
			else if (!this->has_value() && other.has_value())
			{
				this->construct(*other);
			}
			else if (this->has_value() && !other.has_value())
			{
				this->destruct();
			}
      return *this;
    }
    ~optional() MY_NOEXCEPT
    {
      if (this->is_constructed()) this->destruct();
    }
  };

  template <class T, template <class> class StoragePolicy>
  void swap(optional<T, StoragePolicy>& o1, optional<T, StoragePolicy>& o2) MY_NOEXCEPT { o1.swap(o2); }
}

#endif

