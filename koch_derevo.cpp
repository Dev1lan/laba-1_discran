#include <cstdint>
#include <iostream>
#include <string>

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
        const Node* current = root_;
        while (current != nullptr){
            if (key == current->key){
                value = current->value;
                return true;
            }
            if (key < current->key){
                current = current->left;
            }
            else{
                current = current->right;
            }
        }
        return false;
    }

    void PrintTree() const{
        PrintNode(root_, 0, "Root: ");
    }
};

int main() {
    RBTree tree;
    tree.Insert("apple", 10);
    tree.Insert("mango", 20);
    cout << "After two inserts\n";
    tree.PrintTree();
    tree.Insert("pear", 30);
    cout << "\nAfter three inserts\n";
    tree.PrintTree();
}