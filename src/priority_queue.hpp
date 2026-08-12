#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {
/**
 * @brief a container like std::priority_queue which is a heap internal.
 * **Exception Safety**: The `Compare` operation might throw exceptions for certain data.
 * In such cases, any ongoing operation should be terminated, and the priority queue should be restored to its state before the operation began.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
	struct Node {
		T val;
		Node *left;
		Node *right;
		int dist;
		Node(const T &v) : val(v), left(nullptr), right(nullptr), dist(0) {}
	};

	Node *root;
	size_t sz;
	Compare cmp;

	Node *merge_nodes(Node *a, Node *b) {
		if (a == nullptr) return b;
		if (b == nullptr) return a;
		if (cmp(a->val, b->val)) {
			Node *t = a;
			a = b;
			b = t;
		}
		Node *saved_right = a->right;
		try {
			a->right = merge_nodes(a->right, b);
		} catch (...) {
			a->right = saved_right;
			throw;
		}
		if (a->left == nullptr || (a->right != nullptr && a->left->dist < a->right->dist)) {
			Node *t = a->left;
			a->left = a->right;
			a->right = t;
		}
		a->dist = (a->right == nullptr) ? 0 : a->right->dist + 1;
		return a;
	}

	Node *copy_node(Node *n) {
		if (n == nullptr) return nullptr;
		Node *res = new Node(n->val);
		try {
			res->left = copy_node(n->left);
			res->right = copy_node(n->right);
		} catch (...) {
			clear_node(res);
			throw;
		}
		res->dist = n->dist;
		return res;
	}

	void clear_node(Node *n) {
		if (n == nullptr) return;
		clear_node(n->left);
		clear_node(n->right);
		delete n;
	}

public:
	/**
	 * @brief default constructor
	 */
	priority_queue() : root(nullptr), sz(0) {}

	/**
	 * @brief copy constructor
	 * @param other the priority_queue to be copied
	 */
	priority_queue(const priority_queue &other) : root(nullptr), sz(0) {
		root = copy_node(other.root);
		sz = other.sz;
	}

	/**
	 * @brief deconstructor
	 */
	~priority_queue() {
		clear_node(root);
	}

	/**
	 * @brief Assignment operator
	 * @param other the priority_queue to be assigned from
	 * @return a reference to this priority_queue after assignment
	 */
	priority_queue &operator=(const priority_queue &other) {
		if (this != &other) {
			Node *new_root = copy_node(other.root);
			clear_node(root);
			root = new_root;
			sz = other.sz;
		}
		return *this;
	}

	/**
	 * @brief get the top element of the priority queue.
	 * @return a reference of the top element.
	 * @throws container_is_empty if empty() returns true
	 */
	const T & top() const {
		if (root == nullptr) throw container_is_empty();
		return root->val;
	}

	/**
	 * @brief push new element to the priority queue.
	 * @param e the element to be pushed
	 */
	void push(const T &e) {
		Node *new_node = new Node(e);
		try {
			root = merge_nodes(root, new_node);
		} catch (...) {
			delete new_node;
			throw;
		}
		++sz;
	}

	/**
	 * @brief delete the top element from the priority queue.
	 * @throws container_is_empty if empty() returns true
	 */
	void pop() {
		if (root == nullptr) throw container_is_empty();
		Node *old_root = root;
		try {
			root = merge_nodes(root->left, root->right);
		} catch (...) {
			root = old_root;
			throw;
		}
		delete old_root;
		--sz;
	}

	/**
	 * @brief return the number of elements in the priority queue.
	 * @return the number of elements.
	 */
	size_t size() const {
		return sz;
	}

	/**
	 * @brief check if the container is empty.
	 * @return true if it is empty, false otherwise.
	 */
	bool empty() const {
		return sz == 0;
	}

	/**
	 * @brief merge another priority_queue into this one.
	 * The other priority_queue will be cleared after merging.
	 * The complexity is at most O(logn).
	 * @param other the priority_queue to be merged.
	 */
	void merge(priority_queue &other) {
		Node *new_root = merge_nodes(root, other.root);
		root = new_root;
		other.root = nullptr;
		sz += other.sz;
		other.sz = 0;
	}
};

}

#endif
