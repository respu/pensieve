#ifndef ALLOCATORS_HPP
#define ALLOCATORS_HPP


template<typename T>
struct basic_alloc {
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;

    template<typename U>
    struct rebind {
        using other = basic_alloc<U>;
    };

    pointer allocate(std::size_t n) {
        std::cout << "building: " << n << std::endl;
        return (pointer) ::operator new(n * sizeof(T));
    }

    void deallocate(pointer ptr, std::size_t n) {
        std::cout << "deleting: " << n << std::endl;
        ::operator delete(ptr);
    }

    template<typename U, typename ...Args>
    void construct(U* addr_ptr, Args&& ...args) {
        std::cout << "constructing" << std::endl;
        new((void*) addr_ptr)U(std::forward<Args>(args)...);
    }

    template<typename U>
    void destroy(U* addr_ptr) {
        std::cout << "destroying" << std::endl;
        addr_ptr->~U();
    }

    size_type max_size() const {
        return std::numeric_limits<std::size_t>::max() / sizeof(T);
    }
    
};

template<typename T1, typename T2>
bool operator==(const basic_alloc<T1>& , const basic_alloc<T2>& ) {
    return true;
}


template<typename T1, typename T2>
bool operator!=(const basic_alloc<T1>& , const basic_alloc<T2>& ) {
    return false; 
}





#endif 
