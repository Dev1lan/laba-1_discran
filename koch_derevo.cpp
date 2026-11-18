#include <cstdint>
#include <iostream>
#include <string>
#include <cassert>
#include <cctype>

using namespace std;

class RBTree {
    enum class Color {
        Red,
        Black
    };
    struct Node {
        string key;
        uint64_t value;
        Color color = Color::Red;
        Node* left = nullptr;
        Node* right = nullptr;
        Node* parent;
        Node(const string& key, uint64_t value, Node* parent)
            : key(key), value(value), parent(parent) {}
    };
    Node* root_ = nullptr;

    void Clear(Node* node){
        if (node == nullptr){
            return;
        }
        Clear(node->left);
        Clear(node->right);
        delete node;
    }

    Node* FindNode(const string& key) const{
        Node* current = root_;

        while (current != nullptr){
            if (key == current->key){
                return current;
            }

            if (key < current->key){
                current = current->left;
            } else{
                current = current->right;
            }
        }

        return nullptr;
    }

    void RotateLeft(Node* node){
        Node* right = node->right;
        Node* parent = node->parent;
        node->right = right->left;
        if (node->right != nullptr){
            node->right->parent = node;
        }
        right->parent = parent;
        if (parent == nullptr){
            root_ = right;
        } else if (parent->left == node){
            parent->left = right;
        } else{
            parent->right = right;
        }
        right->left = node;
        node->parent = right;
    }

    void RotateRight(Node* node){
        Node* left = node->left;
        Node* parent = node->parent;
        node->left = left->right;
        if (node->left != nullptr){
            node->left->parent = node;
        }
        left->parent = parent;
        if (parent == nullptr){
            root_ = left;
        } else if (parent->left == node){
            parent->left = left;
        } else{
            parent->right = left;
        }
        left->right = node;
        node->parent = left;
    }

    void FixInsert(Node* node){
        while (node->parent != nullptr && node->parent->color == Color::Red){
            Node* parent = node->parent;
            Node* grandparent = parent->parent;

            if (parent == grandparent->left){
                Node* uncle = grandparent->right;
                if (uncle != nullptr && uncle->color == Color::Red){
                    parent->color = Color::Black;
                    uncle->color = Color::Black;
                    grandparent->color = Color::Red;
                    node = grandparent;
                } else{
                    if (node == parent->right){
                        node = parent;
                        RotateLeft(node);
                        parent = node->parent;
                    }
                    parent->color = Color::Black;
                    grandparent->color = Color::Red;
                    RotateRight(grandparent);
                }
            } else{
                Node* uncle = grandparent->left;
                if (uncle != nullptr && uncle->color == Color::Red){
                    parent->color = Color::Black;
                    uncle->color = Color::Black;
                    grandparent->color = Color::Red;
                    node = grandparent;
                } else{
                    if (node == parent->left){
                        node = parent;
                        RotateRight(node);
                        parent = node->parent;
                    }
                    parent->color = Color::Black;
                    grandparent->color = Color::Red;
                    RotateLeft(grandparent);
                }
            }
        }
        root_->color = Color::Black;
    }

    void PrintNode(const Node* node, int depth, const char* label) const{
        if (node == nullptr){
            return;
        }
        cout << string(depth * 2, ' ') << label << node->key
            << (node->color == Color::Black ? " [B]" : " [R]") << '\n';
        PrintNode(node->left, depth + 1, "L: ");
        PrintNode(node->right, depth + 1, "R: ");
    }

    void ReplaceSub3(Node* node, Node* replacement){
        Node* parent = node->parent;
        if (parent == nullptr){
            root_ = replacement;
        } else if (parent->left == node){
            parent->left = replacement;
        } else{
            parent->right = replacement;
        }
        if (replacement != nullptr){
            replacement->parent = parent;
        }
    }

    Color ColorOf(const Node* node) const{
        return node == nullptr ? Color::Black : node->color;
    }

    void FixErase(Node* node){
        while (node != root_ && node->color == Color::Black){
            Node* parent = node->parent;
            bool isLeft = parent->left == node;
            Node* sibling = isLeft ? parent->right : parent->left;
            if (sibling->color == Color::Red){
                sibling->color = Color::Black;
                parent->color = Color::Red;
                if (isLeft){
                    RotateLeft(parent);
                } else{
                    RotateRight(parent);
                }
                sibling = isLeft ? parent->right : parent->left;
            }
            Node* near = isLeft ? sibling->left : sibling->right;
            Node* far = isLeft ? sibling->right : sibling->left;
            if (ColorOf(near) == Color::Black && ColorOf(far) == Color::Black){
                sibling->color = Color::Red;
                node = parent;
                continue;
            }
            if (ColorOf(far) == Color::Black){
                near->color = Color::Black;
                sibling->color = Color::Red;
                if (isLeft){
                    RotateRight(sibling);
                } else{
                    RotateLeft(sibling);
                }
                sibling = isLeft ? parent->right : parent->left;
                far = isLeft ? sibling->right : sibling->left;
            }
            sibling->color = parent->color;
            parent->color = Color::Black;
            far->color = Color::Black;
            if (isLeft){
                RotateLeft(parent);
            } else{
                RotateRight(parent);
            }
            return;
        }
        node->color = Color::Black;
    }

public:
    RBTree() = default;
    RBTree(const RBTree&) = delete;
    RBTree& operator=(const RBTree&) = delete;
    ~RBTree(){
        Clear(root_);
    }

    bool Insert(const string& key, uint64_t value){
        Node* current = root_;
        Node* parent = nullptr;
        while (current != nullptr){
            if (key == current->key){
                return false;
            }
            parent = current;
            if (key < current->key){
                current = current->left;
            } else{
                current = current->right;
            }
        }
        Node* node = new Node(key, value, parent);
        if (parent == nullptr){
            root_ = node;
        } else if (key < parent->key){
            parent->left = node;
        } else{
            parent->right = node;
        }
        FixInsert(node);
        return true;
    }

    bool Find(const string& key, uint64_t& value) const{
        const Node* node = FindNode(key);
        if (node == nullptr){
            return false;
        }
        value = node->value;
        return true;
    }

    bool Erase(const string& key){
        Node* node = FindNode(key);
        if (node == nullptr){
            return false;
        }
        Node* removed = node;
        if (node->left != nullptr && node->right != nullptr){
            removed = node->right;
            while (removed->left != nullptr){
                removed = removed->left;
            }
        }
        Node* child = removed->left;
        if (child == nullptr){
            child = removed->right;
        }
        if (removed != node){
            node->key = removed->key;
            node->value = removed->value;
        }
        if (removed->color == Color::Black && child == nullptr){
            FixErase(removed);
        }
        ReplaceSub3(removed, child);
        if (child != nullptr){
            child->color = Color::Black;
        }
        delete removed;
        return true;
    }

    void PrintTree() const{
        PrintNode(root_, 0, "Root: ");
    }
};

int main() {
    {
        RBTree tree;
        tree.Insert("mango", 10);
        tree.Insert("apple", 20);
        tree.Insert("pear", 30);
        uint64_t value = 0;
        assert(tree.Erase("apple"));
        assert(!tree.Find("apple", value));
        assert(tree.Find("pear", value) && value == 30);
        assert(!tree.Erase("apple"));
        cout << "1. Red leaf: OK\n";
    }

    {
        RBTree tree;
        tree.Insert("mango", 10);
        tree.Insert("apple", 20);
        tree.Insert("pear", 30);
        tree.Insert("banana", 40);
        uint64_t value = 0;
        assert(tree.Erase("apple"));
        assert(!tree.Find("apple", value));
        assert(tree.Find("banana", value) && value == 40);
        cout << "\n2. One child: OK\n";
        tree.PrintTree();
    }

    {
        RBTree tree;
        tree.Insert("mango", 10);
        tree.Insert("apple", 20);
        tree.Insert("pear", 30);
        uint64_t value = 0;
        assert(tree.Erase("mango"));
        assert(!tree.Find("mango", value));
        assert(tree.Find("pear", value) && value == 30);
        assert(tree.Find("apple", value) && value == 20);
        cout << "\n3. Two children, successor is the right child: OK\n";
        tree.PrintTree();
    }

    {
        RBTree tree;
        tree.Insert("mango", 10);
        tree.Insert("apple", 20);
        tree.Insert("pear", 30);
        tree.Insert("orange", 40);
        uint64_t value = 0;
        assert(tree.Erase("mango"));
        assert(!tree.Find("mango", value));
        assert(tree.Find("orange", value) && value == 40);
        assert(tree.Find("pear", value) && value == 30);
        assert(tree.Find("apple", value) && value == 20);
        cout << "\n4. Two children, successor is deeper: OK\n";
        tree.PrintTree();
        {
            cout << "\n5. ";
            RBTree tree;
            tree.Insert("mango", 10);
            tree.Insert("apple", 20);
            tree.Insert("pear", 30);
            tree.Insert("banana", 40);
            tree.Erase("banana");
            cout << boolalpha << tree.Erase("apple") << '\n';
            tree.PrintTree();
        }
        {
        RBTree tree;
        tree.Insert("mango", 10);
        tree.Insert("apple", 20);
        tree.Insert("pear", 30);
        tree.Insert("plum", 40);
        bool erased = tree.Erase("apple");
        assert(erased);
        cout << "\n6. Delete with one rotation\n";
        tree.PrintTree();
    }
    {
        RBTree tree;
        tree.Insert("mango", 10);
        tree.Insert("apple", 20);
        tree.Insert("pear", 30);
        tree.Insert("orange", 40);
        bool erased = tree.Erase("apple");
        assert(erased);
        cout << "\n7. Delete with two rotations\n";
        tree.PrintTree();
    }
    }
}