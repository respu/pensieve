// (C) Copyright Stephan Dollberg 2012-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "../stack.hpp"

#include <type_traits>

namespace atomic_containers {
    /**
     * @brief tries to pop a value from the stack and assigns to the given T, else returns false
     */
    template<typename T> 
    bool stack<T>::try_pop(T& value) {

        auto ptr = std::atomic_load(&head);

        while(ptr && !std::atomic_compare_exchange_weak(&head, &ptr, ptr->next_node));

        if(ptr == nullptr) {
            return false;
        }
        else {
            value = std::move(ptr->value);
            return true;
        }
    }
}
