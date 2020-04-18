// The MIT License (MIT)

// Copyright 2020 Vitalii Minnakhmetov <restlessmonkey@ya.ru>

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.


#ifndef VcppBits_SIMPLE_VECTOR_HPP_INCLUDED__
#define VcppBits_SIMPLE_VECTOR_HPP_INCLUDED__

#include <cstring>
#include <stdexcept>

namespace VcppBits {

class SimpleVectorStaticData {
public:
    static void (*_static_grow_callback) (void);
};


template <typename T>
class SimpleVector {
public:
    SimpleVector (const size_t sz = 0) {
        if (sz)
            _data = new T[sz];
        _allocated_size = sz;
    }

    ~SimpleVector() {
        if (!_isViewOnly) {
            delete[] _data;
        }
    }

    SimpleVector (const SimpleVector&) = delete;
    SimpleVector operator= (const SimpleVector&) = delete;
    SimpleVector (SimpleVector &&pOther)
        : _grow_callback (pOther._grow_callback),
          _used_size (pOther._used_size),
          _allocated_size (pOther._allocated_size),
          _data (pOther._data),
          _isViewOnly (pOther._isViewOnly) {
        // TODO5: figure out proper way of doing this (and move assignment)
        pOther._grow_callback = nullptr;
        pOther._used_size = 0;
        pOther._allocated_size = 0;
        pOther._data = nullptr;
        pOther._isViewOnly = false;
    }
    SimpleVector& operator= (SimpleVector && pOther) {
        _grow_callback = (pOther._grow_callback);
        _used_size = (pOther._used_size);
        _allocated_size = (pOther._allocated_size);
        _data = pOther._data;
        _isViewOnly = pOther._isViewOnly;

        pOther._grow_callback = nullptr;
        pOther._used_size = 0;
        pOther._allocated_size = 0;
        pOther._data = nullptr;
        pOther._isViewOnly = false;

        return *this;
    }

    void initializeByMemory (T* pData,
                             const size_t pAllocatedSize,
                             const size_t pUsedSize) {
        _data = pData;
        _used_size = pUsedSize;
        _allocated_size = pAllocatedSize;
        _isViewOnly = true;
    }

    void nullify () {
        _used_size = 0;
    }

    size_t size () const {
        return _used_size;
    }

    void resize (const size_t pSize) {
        reserve(pSize);
        _used_size = pSize;
    }

    void reserve (const size_t pSize) {
        if (pSize > _allocated_size) {
            grow(pSize);
        }
    }

    T& operator[] (const size_t pNum) {
        if (pNum >= _allocated_size) {
            throw std::out_of_range("eh");
        }
        return _data[pNum];
    }

    const T& operator[] (const size_t pNum) const {
        if (pNum >= _allocated_size) {
            throw std::out_of_range("ehh");
        }
        return _data[pNum];
    }

    void grow (const size_t pNewSize) {
        if (_isViewOnly) {
            throw;
        }

        T* new_ptr = new T[pNewSize];
        if (_data) {
            std::memcpy(new_ptr, _data, _used_size * sizeof(T));
            delete[] _data;
        }
        _allocated_size = pNewSize;
        _data = new_ptr;

        if (SimpleVectorStaticData::_static_grow_callback) {
            SimpleVectorStaticData::_static_grow_callback();
        }

        if (_grow_callback) {
            _grow_callback();
        }
    }

    class iterator {
    public:
        explicit iterator (T* pPtr)
            : _current (pPtr) {
        }
        iterator operator++() {
            ++_current;
            return *this;
        }
        bool operator!=(const iterator& pOther) const {
            return _current != pOther._current;
        }
        T& operator*() const {
            return *_current;
        }
    private:
        T* _current;
    };

    iterator begin () const { return iterator(_data); }
    iterator end () const { return iterator(_data + _used_size); }

    void push_back (const T& pElement) {
        if ((_used_size) >= _allocated_size) {
            grow(std::max<size_t>(8, _allocated_size * 2));
        }
        _data[_used_size++] = pElement;
    }

    T* data () const {
        return _data;
    }

    size_t capacity () const {
        return _allocated_size;
    }

    void set_grow_callback (void (*f_ptr)(void)) {
        _grow_callback = f_ptr;
    }

    static void set_static_grow_callback (void (*f_ptr)(void)) {
        SimpleVectorStaticData::_static_grow_callback = f_ptr;
    }

private:
    void (*_grow_callback) (void) = nullptr;

    size_t _used_size = 0;
    size_t _allocated_size;
    T *_data = nullptr;

    bool _isViewOnly = false;
};

} // namespace VcppBits

#endif // VcppBits_SIMPLE_VECTOR_HPP_INCLUDED__
