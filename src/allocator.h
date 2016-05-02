#ifndef INCLUDE_ALLOC_H
#define INCLUDE_ALLOC_H

#include <cstdint>
#include <iostream>


#include <map>
#include <vector>



// use this when calling STL object if you want
// to keep track of memory usage
template <class T> class MemoryCountingAllocator {
public:
    // type definitions
    typedef T value_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    static uint64_t memory_usage;

    // rebind allocator to type U
    template <class U> struct rebind {
        typedef MemoryCountingAllocator<U> other;
    };

    pointer address(reference value) const {
        return &value;
    }
    const_pointer address(const_reference value) const {
        return &value;
    }

    MemoryCountingAllocator() {}
    MemoryCountingAllocator(const MemoryCountingAllocator &)  {}
    template <typename U>
    MemoryCountingAllocator(const MemoryCountingAllocator<U> &) {}
    ~MemoryCountingAllocator() {}

    // return maximum number of elements that can be allocated
    size_type max_size() const throw() {
        return std::numeric_limits<std::size_t>::max() / sizeof(T);
    }

    pointer allocate(size_type num, const void * = 0) {
        memory_usage += num * sizeof(T);
        return reinterpret_cast<pointer>( ::operator new(num * sizeof(T) ) );
    }

    void construct(pointer p, const T &value) {
        // initialize memory with placement new
        new (p) T(value);
    }

    // destroy elements of initialized storage p
    void destroy(pointer p) {
        p->~T();
    }

    // deallocate storage p of deleted elements
    void deallocate(pointer p, size_type num ) {
        memory_usage -= num * sizeof(T);
        ::operator delete(
            reinterpret_cast<pointer>(reinterpret_cast<uintptr_t>(p)));
    }

};

// for our purposes, we don't want to distinguish between allocators.
template <class T1, class T2>
bool operator==(const MemoryCountingAllocator<T1> &, const T2 &) throw() {
    return true;
}

template <class T1, class T2>
bool operator!=(const MemoryCountingAllocator<T1> &, const T2 &) throw() {
    return false;
}

#endif
