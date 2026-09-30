enum Color { RED, BLACK };

struct Node {
  int val;
  Color color;
  Node *left = nullptr;
  Node *right = nullptr;
  Node *parent = nullptr;

  Node(int val_) : val(val_), color(RED) {};
};

class RedBlackTree {
private:
  Node *root;
  Node *NIL;

  void leftRotate(Node *x) {
    Node *y = x->right;
    x->right = y->left;
    if (y->left != NIL) // NIL's parent is used by deleteFixup; keep it
      y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == NIL)
      root = y;
    else if (x == x->parent->left)
      x->parent->left = y;
    else
      x->parent->right = y;
    y->left = x;
    x->parent = y;
  }

  void rightRotate(Node *x) {
    Node *y = x->left;
    x->left = y->right;
    if (y->right != NIL)
      y->right->parent = x;
    y->parent = x->parent;
    if (x->parent == NIL)
      root = y;
    else if (x == x->parent->left)
      x->parent->left = y;
    else
      x->parent->right = y;
    y->right = x;
    x->parent = y;
  }

  void insertFixup(Node *z) {
    while (z->parent->color == RED) {
      Node *grandparent = z->parent->parent;
      if (z->parent == grandparent->left) {
        Node *uncle = grandparent->right;
        if (uncle->color == RED) {
          uncle->color = z->parent->color = BLACK;
          grandparent->color = RED;
          z = grandparent;
        } else {
          if (z == z->parent->right) {
            z = z->parent;
            leftRotate(z);
          }
          z->parent->color = BLACK;
          grandparent->color = RED;
          rightRotate(grandparent);
        }
      } else {
        Node *uncle = grandparent->left;
        if (uncle->color == RED) {
          uncle->color = z->parent->color = BLACK;
          grandparent->color = RED;
          z = grandparent;
        } else {
          if (z == z->parent->left) {
            z = z->parent;
            rightRotate(z);
          }
          z->parent->color = BLACK;
          grandparent->color = RED;
          leftRotate(grandparent);
        }
      }
    }
    root->color = BLACK;
  }

  void transplant(Node *u, Node *v) {
    if (u->parent == NIL)
      root = v;
    else if (u == u->parent->left)
      u->parent->left = v;
    else
      u->parent->right = v;
    v->parent = u->parent;
  }

  void deleteFixup(Node *x) {
    while (x != root && x->color == BLACK)
      if (x == x->parent->left) {
        Node *w = x->parent->right;
        if (w->color == RED) {
          w->color = BLACK;
          x->parent->color = RED;
          leftRotate(x->parent);
          w = x->parent->right;
        }
        if (w->left->color == BLACK && w->right->color == BLACK) {
          w->color = RED;
          x = x->parent;
        } else {
          if (w->right->color == BLACK) {
            w->left->color = BLACK;
            w->color = RED;
            rightRotate(w);
            w = x->parent->right;
          }
          w->color = x->parent->color;
          x->parent->color = w->right->color = BLACK;
          leftRotate(x->parent);
          x = root;
        }
      } else {
        Node *w = x->parent->left;
        if (w->color == RED) {
          w->color = BLACK;
          x->parent->color = RED;
          rightRotate(x->parent);
          w = x->parent->left;
        }
        if (w->left->color == BLACK && w->right->color == BLACK) {
          w->color = RED;
          x = x->parent;
        } else {
          if (w->left->color == BLACK) {
            w->right->color = BLACK;
            w->color = RED;
            leftRotate(w);
            w = x->parent->left;
          }
          w->color = x->parent->color;
          x->parent->color = w->left->color = BLACK;
          rightRotate(x->parent);
          x = root;
        }
      }
    x->color = BLACK;
  }

  Node *min(Node *node) {
    while (node->left != NIL)
      node = node->left;
    return node;
  }

  void destroy(Node *node) {
    if (node == NIL)
      return;
    destroy(node->left);
    destroy(node->right);
    delete node;
  }

public:
  RedBlackTree() {
    NIL = new Node(-1);
    NIL->color = BLACK;
    NIL->left = NIL->right = NIL->parent = NIL;
    root = NIL;
  }

  ~RedBlackTree() {
    destroy(root);
    delete NIL;
  }
  RedBlackTree(const RedBlackTree &) = delete;
  RedBlackTree &operator=(const RedBlackTree &) = delete;

  void insert(int val) {
    Node *x = root;
    Node *y = NIL;
    while (x != NIL && x->val != val) {
      y = x;
      x = x->val > val ? x->left : x->right;
    }
    if (x != NIL)
      return;
    Node *z = new Node(val);
    z->left = z->right = z->parent = NIL;
    z->parent = y;
    if (y == NIL)
      root = z;
    else if (y->val > z->val)
      y->left = z;
    else
      y->right = z;
    insertFixup(z);
  }

  void erase(int val) {
    Node *z = root;
    while (z != NIL && z->val != val)
      z = (val < z->val) ? z->left : z->right;
    if (z == NIL)
      return;
    Node *y = z;
    Node *x = NIL;
    Color yColor = y->color;
    if (z->left == NIL) {
      x = z->right;
      transplant(z, x);
    } else if (z->right == NIL) {
      x = z->left;
      transplant(z, x);
    } else {
      y = min(z->right);
      yColor = y->color;
      x = y->right;
      if (y->parent == z)
        x->parent = y;
      else {
        transplant(y, x);
        y->right = z->right;
        z->right->parent = y;
      }
      transplant(z, y);
      y->left = z->left;
      z->left->parent = y;
      y->color = z->color;
    }
    delete z;
    if (yColor == BLACK)
      deleteFixup(x);
  }

  bool search(int val) {
    Node *node = root;
    while (node != NIL) {
      if (node->val == val)
        return true;
      node = val < node->val ? node->left : node->right;
    }
    return false;
  }
};
