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
            node->color = Color::Black;
        } else if (key < parent->key){
            parent->left = node;
        } else{
            parent->right = node;
        }
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
};

int main() {
    RBTree tree;
    cout << boolalpha;
    cout << tree.Insert("mango", 10) << '\n'; // true
    cout << tree.Insert("apple", 20) << '\n'; // true
    cout << tree.Insert("pear", 30) << '\n';  // true
    cout << tree.Insert("apple", 99) << '\n'; // false
    uint64_t value = 0;
    if (tree.Find("apple", value)){
        cout << value << '\n'; // 20
    }
    if (tree.Find("pear", value)){
        cout << value << '\n'; // 30
    }
    cout << tree.Find("banana", value) << '\n'; // false
}