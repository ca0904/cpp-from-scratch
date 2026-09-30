#include <algorithm>
#include <cstdlib>

struct Node {
  int val;
  int height;
  Node *left = nullptr;
  Node *right = nullptr;

  Node(int val_) : val(val_), height{} {};
};

class AVL {
private:
  Node *root;

  int height(Node *root) { return root ? root->height : -1; }

  Node *leftRotate(Node *parent) {
    Node *child = parent->right;
    Node *grandchild = child->left;
    child->left = parent;
    parent->right = grandchild;
    parent->height = std::max(height(parent->left), height(parent->right)) + 1;
    child->height = std::max(height(child->left), height(child->right)) + 1;
    return child;
  }

  Node *rightRotate(Node *parent) {
    Node *child = parent->left;
    Node *grandchild = child->right;
    child->right = parent;
    parent->left = grandchild;
    parent->height = std::max(height(parent->left), height(parent->right)) + 1;
    child->height = std::max(height(child->left), height(child->right)) + 1;
    return child;
  }

  Node *rotate(Node *root) {
    int balance = height(root->left) - height(root->right);
    if (balance > 1) {
      if (height(root->left->left) >= height(root->left->right))
        return rightRotate(root);
      else {
        root->left = leftRotate(root->left);
        return rightRotate(root);
      }
    } else if (balance < -1) {
      if (height(root->right->right) >= height(root->right->left))
        return leftRotate(root);
      else {
        root->right = rightRotate(root->right);
        return leftRotate(root);
      }
    }
    return root;
  }

  Node *insert(Node *root, int val) {
    if (!root) {
      root = new Node(val);
      return root;
    }
    if (root->val == val)
      return root;
    if (root->val > val)
      root->left = insert(root->left, val);
    else
      root->right = insert(root->right, val);
    root->height = std::max(height(root->left), height(root->right)) + 1;
    return rotate(root);
  }

  Node *erase(Node *root, int val) {
    if (!root)
      return nullptr;
    if (root->val > val)
      root->left = erase(root->left, val);
    else if (root->val < val)
      root->right = erase(root->right, val);
    else {
      if (!root->left && !root->right) {
        delete root;
        return nullptr;
      } else if (!root->left) {
        Node *temp = root->right;
        delete root;
        return temp;
      } else if (!root->right) {
        Node *temp = root->left;
        delete root;
        return temp;
      } else {
        int minVal = min(root->right);
        root->val = minVal;
        root->right = erase(root->right, minVal);
      }
    }
    root->height = std::max(height(root->left), height(root->right)) + 1;
    return rotate(root);
  }

  int min(Node *root) {
    while (root && root->left)
      root = root->left;
    return root ? root->val : -1;
  }

  int max(Node *root) {
    while (root && root->right)
      root = root->right;
    return root ? root->val : -1;
  }

  bool isBalanced(Node *root) {
    if (!root)
      return true;
    return isBalanced(root->left) && isBalanced(root->right) &&
           std::abs(height(root->left) - height(root->right)) <= 1;
  }

  void destroy(Node *root) {
    if (!root)
      return;
    destroy(root->left);
    destroy(root->right);
    delete root;
  }

public:
  AVL() : root(nullptr) {}
  ~AVL() { destroy(root); }
  AVL(const AVL &) = delete;
  AVL &operator=(const AVL &) = delete;

  void insert(int val) { root = insert(root, val); }
  void erase(int val) { root = erase(root, val); }

  // -1 if the tree is empty
  int min() { return min(root); }
  int max() { return max(root); }

  bool search(int val) {
    Node *node = root;
    while (node) {
      if (node->val == val)
        return true;
      node = val < node->val ? node->left : node->right;
    }
    return false;
  }

  bool isBalanced() { return isBalanced(root); }
};