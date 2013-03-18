// (C) Copyright Stephan Dollberg 2012-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef ATOMIC_CONTAINERS_ATOMIC_STACK
#define ATOMIC_CONTAINERS_ATOMIC_STACK

#include <memory>

namespace atomic_containers {

    template<typename T>
    class stack {
    public:
        static_assert(std::is_nothrow_move_assignable<T>::value, "We require T to be no-throw move-assignable for excep safety");
        using value_type = T;
        using pointer_type = T*;

        stack() = default;

        stack(const stack&) = delete;
        stack& operator=(const stack&) = delete;

        stack(stack&&) = delete;
        stack& operator=(stack&&) = delete;
    
        template<typename Type>
        void push(Type&& value) {
            auto new_shared_node = std::make_shared<node>(std::forward<Type>(value), std::atomic_load(&head));
            while(!std::atomic_compare_exchange_weak(&head, &(new_shared_node->next_node), new_shared_node));
        }

        bool try_pop(T& value);

    private:
        struct node {
            value_type value;
            std::shared_ptr<node> next_node;

            template<typename Type>
            node(Type&& new_value, std::shared_ptr<node>&& next) 
                : value(std::forward<Type>(new_value)), next_node(std::move(next)) {}

        };

    private:
        std::shared_ptr<node> head;
    };
}

#include "detail/stack_impl.hpp"

#endif // ATOMIC_CONTAINERS_ATOMIC_STACK
