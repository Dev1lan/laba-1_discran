#include <cstdint>
#include <iostream>
#include <string>
#include <memory>
#include <utility>

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
        unique_ptr<Node> left;
        unique_ptr<Node> right;
        Node* parent;

        Node(const string& key, uint64_t value, Node* parent)
            : key(key), value(value), parent(parent) {}
    };
    unique_ptr<Node> root_;
public:
    bool Insert(const string& key, uint64_t value){
        Node* current = root_.get();
        Node* parent = nullptr;
        while (current != nullptr){
            if (key == current->key){
                return false;
            }
            parent = current;
            if (key < current->key){
                current = current->left.get();
            } else{
                current = current->right.get();
            }
        }
        auto node = make_unique<Node>(key, value, parent);
        if (parent == nullptr){
            node->color = Color::Black;
            root_ = std::move(node);
        } else if (key < parent->key){
            parent->left = std::move(node);
        } else{
            parent->right = std::move(node);
        }
        return true;
    }

    bool Find(const string& key, uint64_t& value) const{
        const Node* current = root_.get();
        while (current != nullptr){
            if (key == current->key){
                value = current->value;
                return true;
            }
            if (key < current->key){
                current = current->left.get();
            }
            else{
                current = current->right.get();
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