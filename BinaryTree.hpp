#ifndef _SEARCHTREE_HEADER
#define _SEARCHTREE_HEADER

#include "defs.hpp"
#include "Queue.hpp"
#include "Stack.hpp"

template <typename Any>
class BinaryTree // Base for all types of binary trees.
{
private:
    Queue<uintptr_t> pointer_record; // Record all using units.

protected:
    typedef char height_t; // Type of `unit::height`.

    struct unit
    {
        unsigned int element_count; // Support repeated elements.
        height_t height;            // The length from buttom to here.
        unit *left, *right;         // Point to the childs.
        Any element;                // Stored element.

        unit(void) : element()
        {
            element_count = 0;
            height = 0;
            left = right = nullptr;
        }

        template <typename... Args>
        unit(Args &&...parameters) : element(forward<Args>(parameters)...)
        {
            element_count = 0;
            height = 0;
            left = right = nullptr;
        }

        unit(const unit &other) : element(other.element)
        {
            element_count = other.element_count;
            left = other.left, right = other.right;
            height = other.height;
        }

        unit(unit &&other) : element(move(other.element))
        {
            element_count = other.element_count;
            left = other.left, right = other.right;
            height = other.height;
        }

        unit &operator=(const unit &other)
        {
            element_count = other.element_count;
            left = other.left, right = other.right;
            element = other.element;
            height = other.height;
            return *this;
        }

        unit &operator=(unit &&other)
        {
            element_count = other.element_count;
            left = other.left, right = other.right;
            element = move(other.element);
            return *this;
        }

        bool operator<(const unit &other) { return element < other.element; }
        bool operator>(const unit &other) { return element > other.element; }
        bool operator==(const unit &other) { return element == other.element; }

        bool operator<(unit &&other) { return element < other.element; }
        bool operator>(unit &&other) { return element > other.element; }
        bool operator==(unit &&other) { return element == other.element; }
    };

    static void preorder_traversal_tree(unit *root, Stack &des);
    static void inorder_traversal_tree(unit *root, Stack &des);
    static void postorder_traversal_tree(unit *root, Stack &des);

    template <typename... Args>
    unit *allocate_memory(Args &&...parameters)
    {
        unit *address = new unit(forward<Args>(parameters)...);
        pointer_record.append(reinterpret_cast<uintptr_t>(address));
        return address;
    }

protected:
    unit *root;
    size_t element_amount;

    void release(void);

public:
    BinaryTree(void) : root() { element_amount = 0, root = nullptr; }

    void preorder_traversal(Stack &des) { preorder_traversal_tree(root, des); }
    void inorder_traversal(Stack &des) { inorder_traversal_tree(root, des); }
    void postorder_traversal(Stack &des) { postorder_traversal_tree(root, des); }

    size_t size(void) { return this->element_amount; }
    size_t active_nodes(void) { return pointer_record.length(); }

    ~BinaryTree(void) noexcept { release(); }
};

template <typename Any>
void BinaryTree<Any>::preorder_traversal_tree(unit *root, Stack &des)
{
    if (root)
    {
        if (root->element_count)
            des << root->element;
        if (root->left)
            preorder_traversal_tree(root->left, des);
        if (root->right)
            preorder_traversal_tree(root->right, des);
    }
}

template <typename Any>
void BinaryTree<Any>::inorder_traversal_tree(unit *root, Stack &des)
{
    if (root)
    {
        if (root->left)
            inorder_traversal_tree(root->left, des);
        if (root->element_count)
            des << root->element;
        if (root->right)
            inorder_traversal_tree(root->right, des);
    }
}

template <typename Any>
void BinaryTree<Any>::postorder_traversal_tree(unit *root, Stack &des)
{
    if (root)
    {
        if (root->left)
            postorder_traversal_tree(root->left, des);
        if (root->right)
            postorder_traversal_tree(root->right, des);
        if (root->element_count)
            des << root->element;
    }
}

template <typename Any>
void BinaryTree<Any>::release(void)
{
    for (auto &pointer : pointer_record)
        delete reinterpret_cast<unit *>(pointer);
}

template <typename Any>
class SearchTree : public BinaryTree<Any>
{
private:
    typedef typename BinaryTree<Any>::unit unit;
    typedef typename BinaryTree<Any>::height_t height_t;
    // Define type so that the class could call conveniently.

protected:
    void _insert(void) {}

    template <typename first_t, typename... Args>
    void _insert(first_t &&element, Args &&...rest)
    {
        static_assert(is_same_v<decay_t<first_t>, decay_t<Any>>, "SearchTree::_insert <- Wrong type.");
        this->root = _insert_tree(this->root, forward<first_t>(element));
        _insert(forward<Args>(rest)...);
    }

    unit *_insert_tree(unit *root, Any &element);
    unit *_insert_tree(unit *root, Any &&element);

    static unit *single_rotate_right(unit *root)
    {
        unit *tmp = root->right;
        root->right = tmp->left;
        tmp->left = root;
        update_height(root);
        update_height(tmp);
        return tmp;
    }

    static unit *single_rotate_left(unit *root)
    {
        unit *tmp = root->left;
        root->left = tmp->right;
        tmp->right = root;
        update_height(root);
        update_height(tmp);
        return tmp;
    }

    static unit *double_rotate_right(unit *root)
    {
        root->right = single_rotate_left(root->right);
        return single_rotate_right(root);
    }

    static unit *double_rotate_left(unit *root)
    {
        root->left = single_rotate_right(root->left);
        return single_rotate_left(root);
    }

    static height_t measure_height(unit *root) { return (root) ? root->height : -1; }

    static void update_height(unit *root)
    {
        root->height = (measure_height(root->left) > measure_height(root->right)) ? measure_height(root->left) + 1 : measure_height(root->right) + 1;
    }

protected:
    static unit *find(unit *root, Any &element);

    void _remove(void) {}

    template <typename first_t, typename... Args>
    void _remove(first_t &&element, Args &&...rest)
    {
        static_assert(is_same_v<decay_t<first_t>, decay_t<Any>>, "SearchTree::_remove <- Wrong type.");
        unit *result = find(this->root, element);
        if (result)
            result->element_count--, this->element_amount--;
        _remove(forward<Args>(rest)...);
    }

public:
    SearchTree(void) : BinaryTree<Any>() {}

    template <typename... Args>
    SearchTree(Args &&...elements) : BinaryTree<Any>() { insert(forward<Args>(elements)...); }

    template <typename... Args>
    void insert(Args &&...elements) { _insert(forward<Args>(elements)...); }

    template <typename... Args>
    void remove(Args &&...elements) { _remove(forward<Args>(elements)...); }

    bool has(Any &element) { return static_cast<bool>(find(this->root, element)); }
    bool has(Any &&element) { return this->has(element); }

    size_t height(void) { return static_cast<size_t>(this->root->height); }
};

template <typename Any>
typename SearchTree<Any>::unit *
SearchTree<Any>::_insert_tree(unit *root, Any &element)
{
    if (root == nullptr)
    {
        root = this->allocate_memory(element);
        this->element_amount++;
        root->element_count = 1;
    }
    else
    {
        if (root->element < element)
        {
            root->right = _insert_tree(root->right, element);
            if (measure_height(root->right) - measure_height(root->left) == 2)
                if (root->right->element < element)
                    root = single_rotate_right(root);
                else
                    root = double_rotate_right(root);
        }
        else if (root->element > element)
        {
            root->left = _insert_tree(root->left, element);
            if (measure_height(root->left) - measure_height(root->right) == 2)
                if (root->left->element > element)
                    root = single_rotate_left(root);
                else
                    root = double_rotate_left(root);
        }
        else
            root->element_count++, this->element_amount++;
    }
    update_height(root);
    return root;
}

template <typename Any>
typename SearchTree<Any>::unit *
SearchTree<Any>::_insert_tree(unit *root, Any &&element)
{
    if (root == nullptr)
    {
        root = this->allocate_memory(forward<Any>(element));
        this->element_amount++;
        root->element_count = 1;
    }
    else
    {
        if (root->element < element)
        {
            root->right = _insert_tree(root->right, forward<Any>(element));
            if (measure_height(root->right) - measure_height(root->left) == 2)
                if (root->right->element < element)
                    root = single_rotate_right(root);
                else
                    root = double_rotate_right(root);
        }
        else if (root->element > element)
        {
            root->left = _insert_tree(root->left, forward<Any>(element));
            if (measure_height(root->left) - measure_height(root->right) == 2)
                if (root->left->element > element)
                    root = single_rotate_left(root);
                else
                    root = double_rotate_left(root);
        }
        else
            root->element_count++, this->element_amount++;
    }
    update_height(root);
    return root;
}

template <typename Any>
typename SearchTree<Any>::unit *SearchTree<Any>::find(unit *root, Any &element)
{
    if (root->element > element)
    {
        if (root->left)
            return find(root->left, element);
        else
            return nullptr;
    }
    else if (root->element < element)
    {
        if (root->right)
            return find(root->right, element);
        else
            return nullptr;
    }
    else
        return (root->element_count) ? root : nullptr;
}

#endif
