#ifndef _STACK_HEADER
#define _STACK_HEADER

#include "defs.hpp"

class Stack
{
private:
    void *memory;
    size_t memory_size;
    char *sp; // Point to the byte prepared for usage.
    size_t used_bytes;

protected:
    void _push_element(void) {}

    template <typename element_t, typename... Args>
    void _push_element(element_t &&element, Args &&...rest)
    {
        *this << forward<element_t>(element);
        _push_element(forward<Args>(rest)...);
    }

public:
    Stack(void)
    {
        memory_size = 256;
        memory = malloc(memory_size);
        sp = static_cast<char *>(memory);
        used_bytes = 0;
    }

    Stack(const Stack &other)
    {
        memory_size = other.memory_size;
        used_bytes = other.used_bytes;
        memory = malloc(memory_size);
        memcpy(memory, other.memory, used_bytes);
        sp = static_cast<char *>(memory) + used_bytes;
    }

    Stack(Stack &&other)
    {
        memory_size = other.memory_size, other.memory_size = 0;
        used_bytes = other.used_bytes, other.used_bytes = 0;
        memory = other.memory, other.memory = nullptr;
        sp = other.sp, other.sp = 0;
    }

    Stack(size_t size)
    {
        memory_size = size;
        memory = malloc(memory_size);
        sp = static_cast<char *>(memory);
        used_bytes = 0;
    }

    template <typename... Args>
    Stack &push(Args &&...parameters)
    {
        _push_element(forward<Args>(parameters)...);
        return *this;
    }

    template <typename Any>
    Stack &operator<<(Any &&element)
    {
        typedef decay_t<Any> element_t;
        used_bytes += sizeof(element_t);
        element_t *address = new (static_cast<void *>(sp)) element_t;
        *address = forward<Any>(element);
        sp += sizeof(element_t);
        return *this;
    }

    template <typename Any>
    Stack &operator>>(Any &des)
    {
        typedef decay_t<Any> des_t;
        sp -= sizeof(des_t), used_bytes -= sizeof(des_t);
        des = move(*reinterpret_cast<des_t *>(sp));
        return *this;
    }

    void allocate_memory(size_t size)
    {
        memory_size = size;
        void *tmp = memory;
        memory = malloc(memory_size);
        memcpy(memory, tmp, used_bytes);
        free(memory);
        sp = static_cast<char *>(memory) + used_bytes;
    }

    bool is_full(void) { return used_bytes == memory_size; }
    bool is_empty(void) { return !static_cast<bool>(used_bytes); }
    operator bool(void) { return static_cast<bool>(used_bytes); }

    ~Stack(void) noexcept
    {
        if (memory)
            free(memory);
    }
};

#endif
