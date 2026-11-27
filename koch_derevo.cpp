#include <cstdint>
#include <iostream>
#include <string>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <new>
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
        node->right = right->left;
        if (node->right != nullptr){
            node->right->parent = node;
        }
        ReplaceSub3(node, right);
        right->left = node;
        node->parent = right;
    }

    void RotateRight(Node* node){
        Node* left = node->left;
        node->left = left->right;
        if (node->left != nullptr){
            node->left->parent = node;
        }
        ReplaceSub3(node, left);
        left->right = node;
        node->parent = left;
    }

    void FixInsert(Node* node){
        while (node->parent != nullptr && node->parent->color == Color::Red){
            Node* parent = node->parent;
            Node* grandparent = parent->parent;

            bool isLeft = parent == grandparent->left;
            Node* uncle = isLeft ? grandparent->right : grandparent->left;

            if (ColorOf(uncle) == Color::Red){
                parent->color = Color::Black;
                uncle->color = Color::Black;
                grandparent->color = Color::Red;
                node = grandparent;
                continue;
            }

            if (node == (isLeft ? parent->right : parent->left)){
                node = parent;
                if (isLeft){
                    RotateLeft(node);
                } else{
                    RotateRight(node);
                }
                parent = node->parent;
            }

            parent->color = Color::Black;
            grandparent->color = Color::Red;
            if (isLeft){
                RotateRight(grandparent);
            } else{
                RotateLeft(grandparent);
            }
        }
        root_->color = Color::Black;
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

    bool SaveNodes(ofstream& file, const Node* node) const{
        if (node == nullptr){
            return true;
        }
        uint16_t length = static_cast<uint16_t>(node->key.size());
        file.write(reinterpret_cast<const char*>(&length), sizeof(length));
        file.write(node->key.data(), length);
        file.write(reinterpret_cast<const char*>(&node->value), sizeof(node->value));
        return file && SaveNodes(file, node->left) && SaveNodes(file, node->right);
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
            // Пока лист прикреплён, FixErase может найти его родителя.
            FixErase(removed);
        }
        ReplaceSub3(removed, child);
        if (child != nullptr){
            child->color = Color::Black;
        }
        delete removed;
        return true;
    }

    const char* Save(const string& path) const{
        ofstream file(path, ios::binary | ios::trunc);
        if (!file){
            return "Cant open file for writing";
        }
        file.write("RBT1", 4);
        if (!file || !SaveNodes(file, root_)){
            return "Cant write file";
        }
        uint16_t end = 0;
        file.write(reinterpret_cast<const char*>(&end), sizeof(end));
        file.close();

        return file ? nullptr : "Cant write file";
    }

    const char* Load(const string& path){
        RBTree loaded;
        ifstream file(path, ios::binary);
        if (!file){
            error_code error;
            bool exists = filesystem::exists(path, error);
            if (!exists && (!error || error == errc::no_such_file_or_directory)){
                swap(root_, loaded.root_);
                return nullptr;
            }
            return "Cant open file for reading";
        }
        if (file.peek() == char_traits<char>::eof()){
            if (!file.eof() || file.bad()){
                return "Cant read file";
            }
            swap(root_, loaded.root_);
            return nullptr;
        }
        char header[4];
        if (!file.read(header, sizeof(header)) || string(header, sizeof(header)) != "RBT1"){
            return "Invalid file format";
        }
        while (true){
            uint16_t length;
            if (!file.read(reinterpret_cast<char*>(&length), sizeof(length))){
                return "Invalid file format";
            }
            if (length == 0){
                break;
            }
            if (length > 256){
                return "Invalid key length";
            }
            string key(length, '\0');
            uint64_t value;
            if (!file.read(key.data(), length) ||
                !file.read(reinterpret_cast<char*>(&value), sizeof(value))){
                return "Invalid file format";
            }
            if (key.find_first_not_of("abcdefghijklmnopqrstuvwxyz") != string::npos ||
                !loaded.Insert(key, value)){
                return "Invalid dictionary entry";
            }
        }
        if (file.peek() != char_traits<char>::eof() || !file.eof() || file.bad()){
            return "Invalid file format";
        }
        swap(root_, loaded.root_);
        return nullptr;
    }
};

void ToLower(string& key){
    for (char& ch : key){
        ch = static_cast<char>(tolower(static_cast<unsigned char>(ch)));
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    RBTree tree;
    string command;

    while (cin >> command){
        try{
            if (command == "+"){
                string key;
                uint64_t value;
                if (!(cin >> key >> value)){
                    break;
                }
                ToLower(key);
                cout << (tree.Insert(key, value) ? "OK" : "Exist") << '\n';
            } else if (command == "-"){
                string key;
                if (!(cin >> key)){
                    break;
                }
                ToLower(key);
                cout << (tree.Erase(key) ? "OK" : "NoSuchWord") << '\n';
            } else if (command == "!"){
                string action;
                string path;
                if (!(cin >> action) || !getline(cin, path)){
                    break;
                }
                path.erase(0, path.find_first_not_of(" \t"));
                if (!path.empty() && path.back() == '\r'){
                    path.pop_back();
                }
                const char* error = "Unknown command";
                if (action == "Save"){
                    error = tree.Save(path);
                } else if (action == "Load"){
                    error = tree.Load(path);
                }
                if (error == nullptr){
                    cout << "OK\n";
                } else{
                    cout << "ERROR: " << error << '\n';
                }
            } else{
                ToLower(command);
                uint64_t value;
                if (tree.Find(command, value)){
                    cout << "OK: " << value << '\n';
                } else{
                    cout << "NoSuchWord\n";
                }
            }
        } catch (const bad_alloc&){
            cout << "ERROR: Not enough memory\n";
        }
    }
}