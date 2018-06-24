#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP

#include <exception>
#include <cassert>
#include <new>
#include "aligned_storage.hpp"

#define MY_NOEXCEPT throw()

namespace my
{
  template <class T>
  struct DynamicStorage
  {
  private:
    T* data;
  public:
    DynamicStorage(): data(NULL) {}
    template <class U>
    DynamicStorage(const U& val): data(new T(val)) {}
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
  struct LocalStorage
  {
  private:
    type_traits::aligned_storage<sizeof(T), type_traits::alignment_of<T>::value> storage;
    T* data; // for avoiding UB, std::launder is needed to remove this redundant pointer
  public:
    LocalStorage(): data(NULL) {}
    template <class U>
    LocalStorage(const U& val): data(new(storage.address()) T(val)) {}
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

  struct nullopt_t
  {
    struct nullopt_construct {};
    nullopt_t(nullopt_construct){}
  };

  extern nullopt_t nullopt;

  class bad_optional_access: public std::exception
  {
    virtual const char* what() const throw() { return "bad_optional_access"; }
  };

  template <class T, template <class> class Storage = LocalStorage>
  class optional
  {
  private:
    Storage<T> storage;
    // for conversion to unspecified bool idiom
    typedef void (optional::*bool_type)(optional&);
  public:
    typedef T value_type;
  public:
    bool has_value() const MY_NOEXCEPT { return storage.is_constructed(); }
    operator bool_type() const MY_NOEXCEPT { return this->has_value() ? &optional::swap : 0; }
    void reset() MY_NOEXCEPT
    {
      if (this->has_value()) storage.destruct();
    }
    T& operator*() MY_NOEXCEPT
    {
      assert(this->has_value() && "null optional dereferenced");
      return *storage.address();
    }
    const T& operator*() const MY_NOEXCEPT
    {
      assert(this->has_value() && "null optional dereferenced");
      return *storage.address();
    }
    T* operator->() MY_NOEXCEPT { return storage.address(); }
    const T* operator->() const MY_NOEXCEPT { return storage.address(); }
    T& value()
    {
      if (this->has_value()) return *(*this);
      else throw bad_optional_access();
    }
    const T& value() const
    {
      if (this->has_value()) return *(*this);
      else throw bad_optional_access();
    }
    template <class U>
    T value_or(const U& val) const { return this->has_value() ? *(*this) : val; }
    // TODO make this swap exception safe
    void swap(optional& other)
    {
      using std::swap;
      if (this->has_value() && other.has_value())
      {
        swap(*(*this), *other);
      }
      else if (!this->has_value() && other.has_value())
      {
        storage.construct(*other);
        other.reset();
      }
      else if (this->has_value() && !other.has_value())
      {
        other.storage.construct(*(*this));
        this->reset();
      }
    }

  public:
    optional() MY_NOEXCEPT {}
    optional(nullopt_t) MY_NOEXCEPT {}
    template <class U>
    optional(const U& val): storage(val) {}
    optional(const optional& other)
    {
      if (other) storage.construct(*other);
    }
    template <class U, template <class> class OtherStoragePolicy>
    optional(const optional<U, OtherStoragePolicy>& other)
    {
      if (other) storage.construct(*other);
    }
    optional& operator=(nullopt_t) MY_NOEXCEPT
    {
      this->reset();
      return *this;
    }
    template <class U>
    optional& operator=(const U& val)
    {
      if (this->has_value()) *(*this) = val;
      else storage.construct(val);
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
        storage.construct(*other);
      }
      else if (this->has_value() && !other.has_value())
      {
        this->reset();
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
				storage.construct(*other);
			}
			else if (this->has_value() && !other.has_value())
			{
        this->reset();
			}
      return *this;
    }
    ~optional() MY_NOEXCEPT
    {
      this->reset();
    }
  };

  template <class T, template <class> class StoragePolicy>
  void swap(optional<T, StoragePolicy>& o1, optional<T, StoragePolicy>& o2) MY_NOEXCEPT { o1.swap(o2); }

  template <class T1, class T2, template <class> class Storage1, template <class> class Storage2>
  bool operator==(const optional<T1, Storage1>& x, const optional<T2, Storage2>& y)
  {
    if (x.has_value() != y.has_value()) return false;
    else if (!x.has_value()) return true;
    else return *x == *y;
  }
  template <class T1, class T2, template <class> class Storage1, template <class> class Storage2>
  bool operator!=(const optional<T1, Storage1>& x, const optional<T2, Storage2>& y)
  {
    return !(x == y);
  }
  template <class T1, class T2, template <class> class Storage1, template <class> class Storage2>
  bool operator<(const optional<T1, Storage1>& x, const optional<T2, Storage2>& y)
  {
    if (!y) return false;
    else if (!x) return true;
    else return *x < *y;
  }
  template <class T1, class T2, template <class> class Storage1, template <class> class Storage2>
  bool operator>(const optional<T1, Storage1>& x, const optional<T2, Storage2>& y)
  {
    if (!x) return false;
    else if (!y) return true;
    else return *x > *y;
  }
  template <class T1, class T2, template <class> class Storage1, template <class> class Storage2>
  bool operator<=(const optional<T1, Storage1>& x, const optional<T2, Storage2>& y)
  {
    if (!x) return true;
    else if (!y) return false;
    else return *x <= *y;
  }
  template <class T1, class T2, template <class> class Storage1, template <class> class Storage2>
  bool operator>=(const optional<T1, Storage1>& x, const optional<T2, Storage2>& y)
  {
    if (!y) return true;
    else if (!x) return false;
    else return *x >= *y;
  }

  template <class T, template <class> class Storage>
  bool operator==(const optional<T, Storage>& x, nullopt_t) MY_NOEXCEPT { return !x; }
  template <class T, template <class> class Storage>
  bool operator==(nullopt_t, const optional<T, Storage>& x) MY_NOEXCEPT { return !x; }
  template <class T, template <class> class Storage>
  bool operator!=(const optional<T, Storage>& x, nullopt_t) MY_NOEXCEPT { return static_cast<bool>(x); }
  template <class T, template <class> class Storage>
  bool operator!=(nullopt_t, const optional<T, Storage>& x) MY_NOEXCEPT { return static_cast<bool>(x); }
  template <class T, template <class> class Storage>
  bool operator<(const optional<T, Storage>& x, nullopt_t) MY_NOEXCEPT { return false; }
  template <class T, template <class> class Storage>
  bool operator<(nullopt_t, const optional<T, Storage>& x) MY_NOEXCEPT { return static_cast<bool>(x); }
  template <class T, template <class> class Storage>
  bool operator<=(const optional<T, Storage>& x, nullopt_t) MY_NOEXCEPT { return !static_cast<bool>(x); }
  template <class T, template <class> class Storage>
  bool operator<=(nullopt_t, const optional<T, Storage>& x) MY_NOEXCEPT { return true; }
  template <class T, template <class> class Storage>
  bool operator>(const optional<T, Storage>& x, nullopt_t) MY_NOEXCEPT { return !static_cast<bool>(x); }
  template <class T, template <class> class Storage>
  bool operator>(nullopt_t, const optional<T, Storage>& x) MY_NOEXCEPT { return false; }
  template <class T, template <class> class Storage>
  bool operator>=(const optional<T, Storage>& x, nullopt_t) MY_NOEXCEPT { return true; }
  template <class T, template <class> class Storage>
  bool operator>=(nullopt_t, const optional<T, Storage>& x) MY_NOEXCEPT { return !static_cast<bool>(x); }

  template <class T, class U, template <class> class Storage>
  bool operator==(const optional<T, Storage>& x, const U& v) { return static_cast<bool>(x) ? *x == v : false; }
  template <class T, class U, template <class> class Storage>
  bool operator==(const U& v, const optional<T, Storage>& x) { return static_cast<bool>(x) ? v == *x : false; }
  template <class T, class U, template <class> class Storage>
  bool operator!=(const optional<T, Storage>& x, const U& v) { return !(x == v); }
  template <class T, class U, template <class> class Storage>
  bool operator!=(const U& v, const optional<T, Storage>& x) { return !(v == x); }
  template <class T, class U, template <class> class Storage>
  bool operator<(const optional<T, Storage>& x, const U& v) { return static_cast<bool>(x) ? *x < v : true; }
  template <class T, class U, template <class> class Storage>
  bool operator<(const U& v, const optional<T, Storage>& x) { return static_cast<bool>(x) ? v < *x : false; }
  template <class T, class U, template <class> class Storage>
  bool operator<=(const optional<T, Storage>& x, const U& v) { return static_cast<bool>(x) ? *x <= v : true; }
  template <class T, class U, template <class> class Storage>
  bool operator<=(const U& v, const optional<T, Storage>& x) { return static_cast<bool>(x) ? v <= *x : false; }
  template <class T, class U, template <class> class Storage>
  bool operator>(const optional<T, Storage>& x, const U& v) { return static_cast<bool>(x) ? *x > v : false; }
  template <class T, class U, template <class> class Storage>
  bool operator>(const U& v, const optional<T, Storage>& x) { return static_cast<bool>(x) ? v > *x : true; }
  template <class T, class U, template <class> class Storage>
  bool operator>=(const optional<T, Storage>& x, const U& v) { return static_cast<bool>(x) ? *x >= v : false; }
  template <class T, class U, template <class> class Storage>
  bool operator>=(const U& v, const optional<T, Storage>& x) { return static_cast<bool>(x) ? v >= *x : true; }
}

#endif

