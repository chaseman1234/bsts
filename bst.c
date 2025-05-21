/*
 * This file contains an implementation of the basic BST functions prototyped
 * in bst.h.  At the bottom of this file is where you will implement your
 * functions for this assignment.  Make sure to add your name and
 * @oregonstate.edu email address below:
 *
 * Name:
 * Email:
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "bst.h"
#include "stack.h"

/*
 * This structure represents a single node in a BST.
 */
struct bst_node {
  int val;
  struct bst_node* left;
  struct bst_node* right;
};

/*
 * This structure represents an entire BST.  Note that we only need a
 * reference to the root node of the tree.
 */
struct bst {
  struct bst_node* root;
};

struct bst* bst_create() {
  struct bst* bst = malloc(sizeof(struct bst));
  assert(bst);
  bst->root = NULL;
  return bst;
}

void bst_free(struct bst* bst) {
  assert(bst);
  /*
   * Assume that bst_remove() frees each node it removes and use it to free
   * all of the nodes in the tree.
   */
  while (!bst_isempty(bst)) {
    bst_remove(bst->root->val, bst);
  }
  free(bst);
}

int bst_isempty(struct bst* bst) {
  assert(bst);
  return bst->root == NULL;
}

/*
 * Helper function to generate a single BST node containing a given value.
 */
struct bst_node* _bst_node_create(int val) {
  struct bst_node* n = malloc(sizeof(struct bst_node));
  assert(n);
  n->val = val;
  n->left = n->right = NULL;
  return n;
}

/*
 * Helper function to insert a given value into a subtree of a BST rooted at
 * a given node. Operates recursively by determining into which subtree (left
 * or right) under the given node the value should be inserted and performing
 * the insertion on that subtree.
 *
 * Returns the root of the given subtree, modified to contain a new node with
 * the specified value.
 */
struct bst_node* _bst_subtree_insert(int val, struct bst_node* n) {
  if (n == NULL) {
    return _bst_node_create(val);
  } else if (val < n->val) {
    n->left = _bst_subtree_insert(val, n->left);
  } else {
    n->right = _bst_subtree_insert(val, n->right);
  }
  return n;
}

void bst_insert(int val, struct bst* bst) {
  assert(bst);
  bst->root = _bst_subtree_insert(val, bst->root);
}

/*
 * Helper function to return the minimum value in a subtree of a BST.
 */
int _bst_subtree_min_val(struct bst_node* n) {
  while (n->left != NULL) {
    n = n->left;
  }
  return n->val;
}

/*
 * Helper function to remove a given value from a subtree of a BST rooted at
 * a specified node. Operates recursively by figuring out whether val is in
 * the left or the right subtree of the specified node and performing the
 * remove operation on that subtree.
 *
 * Returns the potentially new root of the given subtree, modified to have
 * the specified value removed.
 */
struct bst_node* _bst_subtree_remove(int val, struct bst_node* n) {
  if (n == NULL) {
    return NULL;
  } else if (val < n->val) {
    n->left = _bst_subtree_remove(val, n->left);
    return n;
  } else if (val > n->val) {
    n->right = _bst_subtree_remove(val, n->right);
    return n;
  } else {
    if (n->left && n->right) {
      n->val = _bst_subtree_min_val(n->right);
      n->right = _bst_subtree_remove(n->val, n->right);
      return n;
    } else if (n->left) {
      struct bst_node* left = n->left;
      free(n);
      return left;
    } else if (n->right) {
      struct bst_node* right = n->right;
      free(n);
      return right;
    } else {
      free(n);
      return NULL;
    }
  }
}

void bst_remove(int val, struct bst* bst) {
  assert(bst);
  bst->root = _bst_subtree_remove(val, bst->root);
}

int bst_contains(int val, struct bst* bst) {
  assert(bst);
  struct bst_node* cur = bst->root;
  while (cur != NULL) {
    if (val == cur->val) {
      return 1;
    } else if (val < cur->val) {
      cur = cur->left;
    } else {
      cur = cur->right;
    }
  }
  return 0;
}

/*****************************************************************************
 *
 * Below are the functions and structures you'll implement in this assignment.
 *
 *****************************************************************************/

/*
 * This is the structure you will use to create an in-order BST iterator. It
 * is up to you how to define this structure.
 */
struct bst_iterator {
  struct stack* stack;
};

/*
 * Helper function to recursively compute the size of a subtree.
 */
static int _bst_size(struct bst_node* node) {
  if (!node) return 0;
  return 1 + _bst_size(node->left) + _bst_size(node->right);
}

int bst_size(struct bst* bst) {
  assert(bst);
  return _bst_size(bst->root);
}

/*
 * Helper function to recursively compute the height of a subtree.
 */
static int _bst_height(struct bst_node* node) {
  if (!node) return -1;
  int left = _bst_height(node->left);
  int right = _bst_height(node->right);
  return 1 + (left > right ? left : right);
}

int bst_height(struct bst* bst) {
  assert(bst);
  return _bst_height(bst->root);
}

/*
 * Helper function to check for a root-to-leaf path that sums to a target.
 */
static int _bst_path_sum(struct bst_node* node, int sum) {
  if (!node) return 0;
  if (!node->left && !node->right) return node->val == sum;
  return _bst_path_sum(node->left, sum - node->val) ||
         _bst_path_sum(node->right, sum - node->val);
}

int bst_path_sum(int sum, struct bst* bst) {
  assert(bst);
  return _bst_path_sum(bst->root, sum);
}

/*
 * Helper function to push a path of left children to the stack.
 */
static void _push_left_path(struct stack* s, struct bst_node* node) {
  while (node) {
    stack_push(s, node);
    node = node->left;
  }
}

/*
 * Initializes a new BST iterator for in-order traversal.
 */
struct bst_iterator* bst_iterator_create(struct bst* bst) {
  assert(bst);
  struct bst_iterator* iter = malloc(sizeof(struct bst_iterator));
  assert(iter);
  iter->stack = stack_create();
  _push_left_path(iter->stack, bst->root);
  return iter;
}

/*
 * Frees all memory used by a BST iterator.
 */
void bst_iterator_free(struct bst_iterator* iter) {
  assert(iter);
  stack_free(iter->stack);
  free(iter);
}

/*
 * Returns 1 if there are more values to return in in-order traversal.
 */
int bst_iterator_has_next(struct bst_iterator* iter) {
  assert(iter);
  return !stack_isempty(iter->stack);
}

/*
 * Returns the next value in the in-order traversal.
 */
int bst_iterator_next(struct bst_iterator* iter) {
  assert(iter);
  assert(bst_iterator_has_next(iter));
  struct bst_node* node = stack_pop(iter->stack);
  int val = node->val;
  if (node->right) {
    _push_left_path(iter->stack, node->right);
  }
  return val;
}
