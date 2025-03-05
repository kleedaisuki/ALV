#include "BinaryTree.hpp"

// Example.
int main(int argc, char *argv[])
{
    SearchTree<int> search_tree;
    search_tree.insert(1, 5, 3, 8, 7, 9, 2);
    search_tree.remove(1, 7, 8);

    Stack stack(search_tree.size() * sizeof(int));
    search_tree.inorder_traversal(stack);

    int tmp;
    while (stack)
    {
        stack >> tmp;
        print(tmp, '\n');
    }
}
