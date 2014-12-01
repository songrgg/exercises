#ifndef __SKIPLST_H__
#define __SKIPLST_H__

#include <stdlib.h>
#include <stdio.h>
#include <ctime>

template <typename Key, typename Value>
class SkipList {
public:
	SkipList() {
		Node *head = new Node();
		head->children = head->nextSibling = NULL;
		head_ = head;
		height_ = 1;
	}

	~SkipList() {
		Node *cur_level = head_;
		do {
			Node *cur_node = cur_level;
			Node *next_level = cur_level->children;
			while (cur_node != NULL) {
				Node *next_node = cur_node->nextSibling;
				delete cur_node;
				cur_node = next_node;
			}
			cur_level = next_level;
		} while (cur_level != NULL);
	}

	// Insert the key-value pair into the skiplist.
	void Insert(Key key, Value value);

	// Get the value with the specified key.
	Value Get(const Key& key);

	// Remove the value with the specified key.
	void Remove(Key key);

	// Return if the value with the specified key exists.
	bool Exists(const Key& key) const;

private:
	struct Node {
		Key  key;            // the key of the node
		Value value;         // the value of the node
		Node *nextSibling;   // next sibling
		Node *children;      // pointer to the children, the deeper level;
	};

	// Return a random height.
	int randomHeight();

	// Find the nodes with the key just less than or equal to the specified key.
	Node* FindLessOrEqualTo(const Key& key, Node** less) const;

	// Find the nodes with the key just less than the specified key.
	Node* FindLessThan(const Key& key, Node** less) const;

/*========================Fields start=================================*/
	// The position of the skiplist where the search starts
	Node *head_;

	// Skiplist height
	int  height_;
/*========================Fields end===================================*/

	const static int kMaxHeight = 12;
};

template<typename Key, typename Value>
void SkipList<Key, Value>::Insert(Key key, Value value) {
	// printf("%s\n", "Insert");
	int height = randomHeight();
	// printf("%d\n", height);
	Node* less[kMaxHeight];

	FindLessOrEqualTo(key, less);

	if (height > height_) {
		for (int h = height_; h < height; h++) {
			Node *head = new Node();
			head->nextSibling = NULL;
			head->children = head_;
			head_ = head;
			less[h] = head_; 
		}
	}

	// printf("%s\n", "After updating height");

	for (int h = 0; h < height; h++) {
		Node *node = new Node();
		node->key = key;
		node->value = value;
		node->nextSibling = less[h]->nextSibling;
		less[h]->nextSibling = node;

		if (h == 0) {
			node->children = NULL;
		} else {
			node->children = less[h-1]->nextSibling;
		}
	}

	// update current height
	height_ = height;
	printf("Current height is: %d\n", height);
}

template<typename Key, typename Value>
Value SkipList<Key, Value>::Get(const Key& key) {
	printf("%s\n", "Get");
	Node *lessNode = FindLessOrEqualTo(key, NULL);
	if (lessNode->key == key) {
		return lessNode->value;
	}
	return NULL;
}

template<typename Key, typename Value>
void SkipList<Key, Value>::Remove(Key key) {
	printf("%s\n", "Del");
	Node *less[kMaxHeight];
	Node *lessNode = FindLessThan(key, less);

	for (int h = 0; h < height_; h++) {
		Node *next = less[h]->nextSibling;
		if (next != NULL && next->key == key) {
			less[h]->nextSibling = next->nextSibling;
			delete next;
		}
	}

	// Remove the empty level after removing a key.
	Node *head = head_;
	while (height_ > 1 && head->nextSibling == NULL) {
		head_ = head->children;
		delete head;
		head = head_;
		--height_;
	}
	printf("Current height is: %d\n", height_);
}

template<typename Key, typename Value>
bool SkipList<Key, Value>::Exists(const Key& key) const {
	printf("%s\n", "Exists");
	Node *lessNode = FindLessOrEqualTo(key, NULL);
	if (lessNode->key == key) {
		return true;
	}
	return false;
}

template<typename Key, typename Value>
int SkipList<Key, Value>::randomHeight() {
	printf("%s\n", "RandownHeight");
	srand((unsigned)time(NULL));
	return rand()%kMaxHeight + 1;
}

template<typename Key, typename Value>
typename SkipList<Key, Value>::Node* SkipList<Key, Value>::FindLessThan(const Key& key, Node** less) const {
	int h = height_;
	Node *ret = NULL;
	for (Node *cur_level = head_; cur_level != NULL; cur_level = cur_level->children) {
		Node *lessNode = cur_level;
		for (Node *cur_node = cur_level->nextSibling; cur_node != NULL; cur_node = cur_node->nextSibling) {
			if (cur_node->key < key) {
				lessNode = cur_node;
			} else {
				// follow the less node's children to find the path.
				cur_level = lessNode;
				break;
			}
		}

		if (less != NULL) {
			less[--h] = lessNode;
		}

		ret = lessNode;
	}
	return ret;
}

template<typename Key, typename Value>
typename SkipList<Key, Value>::Node* SkipList<Key, Value>::FindLessOrEqualTo(const Key& key, Node** less) const {
	int height = height_;
	Node *ret = NULL;
	for (Node *cur_level = head_; cur_level != NULL; cur_level = cur_level->children) {
		Node *lessNode = cur_level;
		for (Node *cur_node = cur_level->nextSibling; cur_node != NULL; cur_node = cur_node->nextSibling) {
			if (cur_node->key <= key) {
				lessNode = cur_node;
			} else {
				// follow the less node's children to find the path.
				cur_level = lessNode;
				break;
			}
		}

		if (less != NULL) {
			less[--height] = lessNode;
		}

		ret = lessNode;
	}
	return ret;
}

#endif