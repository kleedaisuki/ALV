#ifndef _QUEUE_HEADER
#define _QUEUE_HEADER

#include "defs.hpp"

template <typename Any>
class Queue
{
    friend void print<>(Queue<Any> &queue);
    // Automatic formatting output function for the public output interface.

private:
    struct unit
    {
        Any data;
        unit *next;

        unit(void) : data() { next = nullptr; }

        template <typename... Args>
        unit(Args &&...parameters) : data(forward<Args>(parameters)...) { next = nullptr; }
    };

    size_t element_amount;
    unit *start, *tail;

public:
    class Iterator // Forward only.
    {
        friend Queue;

    private:
        unit *here;

    protected:
    public:
        Iterator(void) { here = nullptr; }
        Iterator(unit *address) { here = address; }

        Iterator &operator++(void)
        {
            here = here->next;
            return *this;
        }
        Iterator &operator++(int) { ++(*this); }

        Iterator operator+(int offset);
        Iterator &operator+=(int offset);

        bool operator==(const Iterator other) { return here == other.here; }
        bool operator!=(const Iterator other) { return here != other.here; }
        operator bool(void) { return static_cast<bool>(here); }

        Any &operator*(void) { return here->data; }

        operator Any *(void) { return &(here->data); }
        Any *operator->(void) { return &(here->data); }
    };

protected:
    /*
          The following functions are inner member functions that realize
          public member function `append` below.
          Expand template parameter package recursively.
    */
    template <typename first_t, typename... Args>
    void _append(first_t &&element, Args &&...parameters)
    {
        static_assert(is_same_v<decay_t<first_t>, decay_t<Any>>, "Queue::_append <- Wrong type.");
        append(forward<first_t>(element));
        _append(forward<Args>(parameters)...); // Recursive call.
    }

    void _append(void) {}
    // For a more robust program, set the tail of the recursion.

    void release(void); // Deconstructor.

public:
    Queue(void)
    {
        element_amount = 0;
        start = tail = nullptr;
    }

    template <typename... Args>
    Queue(Args &&...parameters) // A more powerful constructor.
    {
        element_amount = 0;
        start = tail = nullptr;

        _append(forward<Args>(parameters)...); // Recursive call.
    }

    Queue(const Queue &queue); // Copy constructor for Queue.
    Queue(Queue &&other)
    {
        element_amount = other.element_amount;
        start = other.start;
        tail = other.tail;

        other.element_amount = 0;
        other.start = other.tail = nullptr; // Avoid calling deconstructor unexpectedly.
    }

    Queue(std::initializer_list<Any> list);
    // Support initializer list syntax.

    /* More override operators for copy and move. */
    Queue &operator=(const Queue &other);

    Queue &operator=(Queue &&other)
    {
        release(); // Call deconstructor to prevent memory leak.

        element_amount = other.element_amount;
        start = other.start;
        tail = other.tail;

        other.element_amount = 0;
        other.start = other.tail = nullptr; // Avoid calling deconstructor unexpectedly.
    }

    /* Functions to append elements to the end. */
    Queue &append(Any &in);
    Queue &append(Any &&in);

    template <typename... Args>
    Queue &append(Args &&...parameters)
    {
        _append(forward<Args>(parameters)...);
        return *this;
    }
    /*
        One of the main features of Queue.
        The function is realized in the previous `_append`, actually plays the role of iterface.
        Support passing a pointer instead directly.
    */

    template <typename element_t>
    Queue &operator<<(element_t &&in)
    {
        static_assert(is_same_v<decay_t<element_t>, decay_t<Any>>, "Queue::operator<< <- Wrong type.");
        return append(forward<element_t>(in));
    }
    // Append elements to the end.

    Queue &operator>>(Any &out);
    // Pop an element if the queue is not empty.

    Any &operator[](unsigned int offset);
    // Get a reference to a certain element.

    Iterator begin(void) { return Iterator(start); }
    Iterator end(void) { return Iterator(start); }

    size_t length(void) const { return element_amount; }
    operator bool(void) const { return static_cast<bool>(element_amount); }
    // We can use the object directly when computing logical expressions.
    // Return false if empty, or it will return true.

    ~Queue(void) noexcept { release(); } // Call deconstructor.
};

template <typename Any>
typename Queue<Any>::Iterator Queue<Any>::Iterator::operator+(int offset)
{
    Iterator iterator(*this);
    for (int i = 0; i < offset; i++)
        if (iterator.here->next)
            iterator++;
        else
            break;
    return iterator;
}

template <typename Any>
typename Queue<Any>::Iterator &Queue<Any>::Iterator::operator+=(int offset)
{
    for (int i = 0; i < offset; i++)
        if (here->next)
            here = here->next;
        else
            break;
    return *this;
}

template <typename Any>
Queue<Any>::Queue(const Queue<Any> &other)
{
    element_amount = 0;
    start = tail = nullptr;

    unit *iter = other.start;
    for (int i = 0; i < other.element_amount; i++, iter = iter->next)
        *this << iter->data;
}

template <typename Any>
Queue<Any>::Queue(std::initializer_list<Any> list)
{
    element_amount = 0;
    start = tail = nullptr;

    auto iterator = list.begin();
    for (int i = 0; i < list.size(); i++)
    {
        append(*iterator);
        ++iterator;
    }
}

template <typename Any>
Queue<Any> &Queue<Any>::append(Any &in)
{
    if (element_amount == 0) // The first element appended to the chain list.
    {
        start = new unit;

        start->data = in;
        start->next = start;
        tail = start;

        element_amount++;
    }
    else
    {
        unit *temp = new unit;
        temp->data = in;
        temp->next = start;

        tail->next = temp;
        tail = temp;

        element_amount++;
    }
    return *this;
}

template <typename Any>
Queue<Any> &Queue<Any>::append(Any &&in)
{
    if (element_amount == 0) // The first element appended to the chain list.
    {
        start = new unit;

        start->data = forward<Any>(in);
        start->next = start;
        tail = start;

        element_amount++;
    }
    else
    {
        unit *temp = new unit;
        temp->data = forward<Any>(in);
        temp->next = start;

        tail->next = temp;
        tail = temp;

        element_amount++;
    }
    return *this;
}

template <typename Any>
Queue<Any> &Queue<Any>::operator>>(Any &out)
{
    if (element_amount == 0)
        throw "null queue";

    unit *temp = start;
    out = move(start->data);
    start = temp->next;

    delete temp;
    element_amount--;
    return *this;
}

template <typename Any>
Any &Queue<Any>::operator[](unsigned int offset)
{
    if (offset >= element_amount)
        throw "out of range";

    unit *temp = start;
    for (int i = 0; i < offset; i++)
        temp = temp->next;
    return temp->data;
}

template <typename Any>
void Queue<Any>::release(void)
{
    if (start)
    {
        unit *iter = start;
        unit *temp;
        for (int i = 0; i < element_amount - 1; i++)
        {
            temp = iter;
            iter = iter->next;
            delete temp;
        }

        delete iter;
    }
}

template <typename Any>
Queue<Any> &Queue<Any>::operator=(const Queue &other)
{
    release(); // Call deconstructor to prevent memory leak.

    element_amount = 0;
    start = tail = nullptr;

    unit *iter = other.start;
    for (int i = 0; i < other.element_amount; i++, iter = iter->next)
        *this << iter->data;
}

template <typename Any>
void print(Queue<Any> &queue) // Override for public output interface.
{
    auto iter = queue.start;
    printf("[");
    if (queue.length())
    {
        for (int i = 0; i < queue.length() - 1; i++, iter = iter->next)
            print(iter->data, ", ");
        print(iter->data, "]");
    }
    else
        putchar(']');
}

#endif
