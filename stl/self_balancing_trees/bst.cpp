struct Node {
  int val;
  Node *left = nullptr;
  Node *right = nullptr;

  Node(int val_) : val(val_) {};
};

class BST {
private:
  Node *root;

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
    return root;
  }

  Node *erase(Node *root, int val) {
    if (!root)
      return nullptr;
    if (val < root->val) {
      root->left = erase(root->left, val);
    } else if (val > root->val) {
      root->right = erase(root->right, val);
    } else {
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
    return root;
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

  void destroy(Node *root) {
    if (!root)
      return;
    destroy(root->left);
    destroy(root->right);
    delete root;
  }

public:
  BST() : root(nullptr) {}
  ~BST() { destroy(root); }
  BST(const BST &) = delete;
  BST &operator=(const BST &) = delete;

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
};
