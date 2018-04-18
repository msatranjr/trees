#include "stdafx.h"
#include<stdlib.h>
#include<math.h>

typedef struct tree_node
{
	tree_node* left;
	tree_node* right;
	int data;

} Node;

Node* Node_init()
{
	Node* node = (Node*)malloc(sizeof(Node));
	node->left = 0;
	node->right = 0;
	node->data = 0;
	return node;
}

void Node_rotate_right(Node** subroot)
{
	Node* Q = *subroot;
	Node* P = Q->left;
	Node* B = P->right;
	Q->left = B;
	P->right = Q;
	*subroot = P;
}

void Node_rotate_left(Node** subroot)
{
	Node* P = *subroot;
	Node* Q = P->right;
	Node* B = Q->left;
	P->right = B;
	Q->left = P;
	*subroot = Q;
}

void Node_rotate_double_left(Node** subroot)
{
	Node** right = &((*subroot)->right);
	Node_rotate_right(right);
	Node_rotate_left(subroot);
}

void Node_rotate_double_right(Node** subroot)
{
	Node** left = &((*subroot)->left);
	Node_rotate_left(left);
	Node_rotate_right(subroot);
}

void Node_rotate_none(Node** subroot)
{
	// No-op.
}

// Calculates the tree height.
int Tree_height(Node* root)
{
	if (root == 0)
	{
		return 0;
	}

	int left = Tree_height(root->left);
	int right = Tree_height(root->right);

	return 1 + (int)fmax(left, right);
}

// Function array to avoid creating a switch.
void(*rotationFunctions[5])(Node**) = { &Node_rotate_right, &Node_rotate_double_right, &Node_rotate_double_left, &Node_rotate_left, &Node_rotate_none };

enum BalanceRequirement
{
	// single right rotation.
	Right = 0,
	// double right rotation.
	DoubleRight,
	// double left rotation.
	DoubleLeft,
	//single left rotation.
	Left,
	// no rotation.
	None
};

// Checks if a rebalance is required and if so, which type.
BalanceRequirement Node_balance_check(Node* subroot)
{
	if (subroot == 0)
	{
		return None;
	}

	int balanceFactor = Tree_height(subroot->left) - Tree_height(subroot->right);
	BalanceRequirement requirement = None;

	// The tree is right heavy, so we need to visit the right subtree and find out which child is heavier.
	if (balanceFactor < -1)
	{
		Node* subtree = subroot->right;
		int subtreeBalanceFactor = Tree_height(subtree->left) - Tree_height(subtree->right);
		requirement = subtreeBalanceFactor <= 0 ? Left : DoubleLeft;
	}
	// The tree is left heavy, so we need to visit the left subtree and find out which child is heavier.
	else if (balanceFactor > 1)
	{
		Node* subtree = subroot->left;
		int subtreeBalanceFactor = Tree_height(subtree->left) - Tree_height(subtree->right);
		requirement = subtreeBalanceFactor >= 0 ? Right : DoubleRight;
	}
	else
	{
		return None;
	}
}

void Tree_delete(Node** rootPtr, int value)
{
	Node* root = *rootPtr;
	Node** deleteNodePtr = 0;

	// Base case: we've reached the desired node to delete.
	if (root != 0 && root->data == value)
	{
		return;
	}

	if (root != 0 && value < root->data)
	{
		Tree_delete(&(root->left), value);
		deleteNodePtr = &root->left;
	}
	else if (root != 0 && value > root->data)
	{
		Tree_delete(&(root->right), value);
		deleteNodePtr = &root->right;
	}

	// Didn't find the value.
	if (*deleteNodePtr == 0)
	{
		return;
	}

	Node* deleteNode = *deleteNodePtr;

	// If deleted is a leaf, simply delete the node.
	if (deleteNode->data == value && deleteNode->left == 0 && deleteNode->right == 0)
	{
		*deleteNodePtr = 0;
		free(deleteNode);
	}
	// If deleted only has a left child, delete the node and replace it with it's child.
	else if (deleteNode->data == value && deleteNode->left != 0 && deleteNode->right == 0)
	{
		*deleteNodePtr = deleteNode->left;
		free(deleteNode);
	}
	// If deleted only has a right child, delete the node and replace it with it's child.
	else if (deleteNode->data == value && deleteNode->left == 0 && deleteNode->right != 0)
	{
		*deleteNodePtr = deleteNode->right;
		free(deleteNode);
	}
	// If delete has two children, find minimum node in right subtree then replace with the deleted node.
	else if (deleteNode->data == value && deleteNode->left != 0 && deleteNode->right != 0)
	{
		// Find minimum node in right subtree.
		Node *prev = deleteNode->right;
		Node *min = deleteNode->right;
		while (min->left != 0)
		{
			prev = min;
			min = min->left;
		}

		// Replace deleted node with minimum node in it's right subtree.
		*deleteNodePtr = min;
		min->left = deleteNode->left;

		// Edge-case, minimum element in right subtree is the deleted node's right element. If this case isn't covered, there will be a cycle in the tree.
		if (deleteNode->right != min)
		{
			min->right = deleteNode->right;
		}
		else
		{
			deleteNode->right = 0;
		}

		// Remove reference to minimum element and free memory for deleted node.
		if (prev != min)
		{
			prev->left = 0;
		}
		
		free(deleteNode);
	}

	// While going back up the tree, re-balance each node if it's required.
	(rotationFunctions[Node_balance_check(root)])(rootPtr);

	return;
}

void Tree_insert(Node** rootPtr, int value)
{
	Node* root = *rootPtr;
	Node** direction = 0;

	// Find out which direction in the tree to go next.
	if (root != 0 && value < root->data)
	{
		direction = &(root->left);
	}
	else if (root != 0 && value > root->data)
	{
		direction = &(root->right);
	}
	
	// If we've reached a leaf node, create a new node and insert it in the next direction.
	if (direction != 0 && *direction == 0)
	{
		Node* newNode = Node_init();
		newNode->data = value;
		*direction = newNode;
		return;
	}
	
	// Recursively find the insertion point.
	Tree_insert(direction, value);

	// While going back up the tree, re-balance each node if it's required.
	(rotationFunctions[Node_balance_check(root)])(rootPtr);
}

void Tree_destroy(Node* root)
{
	// Base case: found a leaf node.
	if (root != 0 && root->left == 0 && root->right == 0)
	{
		return;
	}

	if (root != 0)
	{
		Tree_destroy(root->left);
		free(root->left);

		Tree_destroy(root->right);
		free(root->right);
	}

	return;
}

#pragma region PrintTree
typedef struct queue_node
{
	Node* data;
	queue_node* next;
	bool placeholder;
} QueueNode;
QueueNode* QueueNode_init(Node* data)
{
	QueueNode* node = (QueueNode*)malloc(sizeof(QueueNode));
	node->data = data;

	if (data == 0)
	{
		node->placeholder = true;
	}
	else
	{
		node->placeholder = false;
	}

	node->next = 0;
	return node;
}
void Queue_enque(QueueNode** head, Node* data)
{
	QueueNode* node = QueueNode_init(data);

	QueueNode* current = *head;
	while (current != 0 && current->next != 0)
	{
		current = current->next;
	}

	if (current == 0)
	{
		current = node;
		*head = current;
	}
	else
	{
		current->next = node;
	}
}
Node* Queue_deque(QueueNode** head)
{
	QueueNode* current = *head;

	if (*head != 0)
	{
		*head = current->next;
		return current->data;
	}
	else
	{
		return 0;
	}
}
void Tree_debug_print(Node* root)
{
	QueueNode* head = QueueNode_init(root);
	int previousHeight = Tree_height(root);

	while (head != 0)
	{
		Node* offset = Queue_deque(&head);

		// Check if we are at a different level from the previous node, if so, print a new line.
		int offsetHeight = Tree_height(offset);
		if (previousHeight > offsetHeight)
		{
			printf("\n");
			previousHeight = offsetHeight;
		}

		// Print spaces between nodes depending on the level we are to create a pyramid effect.
		for (int i = 0; i < offsetHeight; i++)
		{
			printf(" ");
		}

		// Print the node.
		if (offset != 0)
			printf("%d", offset->data);
		else
			printf(" ");
		
		// Enqueue the left and right nodes, if there are any.
		if (offset != 0 && offset->left != 0)
		{
			Queue_enque(&head, offset->left);
		}
		else if (offset->left != 0)
		{
			Queue_enque(&head, 0);
		}

		if (offset != 0 && offset->right != 0)
		{
			Queue_enque(&head, offset->right);
		}
		else if (offset->right != 0)
		{
			Queue_enque(&head, 0);
		}
	}
}
bool Tree_print_test()
{
	bool status = true;
	Node* root = Node_init();
	root->data = 10;
	Tree_insert(&root, 8);
	Tree_insert(&root, 11);
	Tree_insert(&root, 7);
	Tree_insert(&root, 9);
	Tree_insert(&root, 12);
	Tree_insert(&root, 13);

	Tree_debug_print(root);
	Tree_destroy(root);
	return status;
}
#pragma endregion Print Tree using breadth-first traversal, which uses a queue.

#pragma region DeprecatedTests
bool Tree_delete_test()
{
	Node* root = Node_init();

	root->data = 10;
	Tree_insert(&root, 5);
	Tree_insert(&root, 2);
	Tree_insert(&root, 1);
	Tree_insert(&root, 3);
	Tree_insert(&root, 8);
	Tree_insert(&root, 6);
	Tree_insert(&root, 9);

	printf("Before deleting:\n");
	Tree_debug_print(root);

	printf("\nAfter deleting:\n");
	Tree_delete(&root, 10);

	Tree_debug_print(root);

	Tree_destroy(root);
	return false;
}
bool Tree_height_test()
{
	bool status = false;
	Node* root = Node_init();
	root->data = 10;
	Tree_insert(&root, 9);
	Tree_insert(&root, 8);
	Tree_insert(&root, 7);
	Tree_insert(&root, 6);
	Tree_insert(&root, 5);

	int height = Tree_height(root);

	if (height == 6)
	{
		status = true;
	}

	Tree_destroy(root);
	return status;
}
bool Tree_balance_check()
{
	bool status = true;
	Node* root = Node_init();

	root->data = 10;
	Tree_insert(&root, 5);
	Tree_insert(&root, 2);
	BalanceRequirement problem = Node_balance_check(root);
	status &= (problem == Left);
	(rotationFunctions[problem])(&root);
	status &= root->data == 5 &&
		root->left->data == 2 &&
		root->right->data == 10;
	Tree_destroy(root);

	root = Node_init();
	root->data = 10;
	Tree_insert(&root, 5);
	Tree_insert(&root, 8);
	problem = Node_balance_check(root);
	status &= (problem == DoubleRight);
	(rotationFunctions[problem])(&root);
	status &= root->data == 8 &&
		root->left->data == 5 &&
		root->right->data == 10;
	Tree_destroy(root);

	root = Node_init();
	root->data = 10;
	Tree_insert(&root, 15);
	Tree_insert(&root, 11);
	problem = Node_balance_check(root);
	status &= (problem == DoubleLeft);
	(rotationFunctions[problem])(&root);
	status &= root->data == 11 &&
		root->left->data == 10 &&
		root->right->data == 15;
	Tree_destroy(root);

	root = Node_init();
	root->data = 10;
	Tree_insert(&root, 15);
	Tree_insert(&root, 20);
	problem = Node_balance_check(root);
	status &= (problem == Right);
	(rotationFunctions[problem])(&root);
	status &= root->data == 15 &&
		root->left->data == 10 &&
		root->right->data == 20;

	Tree_destroy(root);
	return status;
}
#pragma endregion These tests were created during each step of development. They are currently deprecated, because rotations happen in the insert and delete functions now.

// Checks that the balance factor of every node is between -1 and 1.
bool Tree_AVL_tree_bf_invariant(Node* root)
{
	if (root == 0)
	{
		return true;
	}

	bool thisResult = Node_balance_check(root) == None;
	bool leftResult = Tree_AVL_tree_bf_invariant(root->left) && Node_balance_check(root->left) == None;
	bool rightResult = Tree_AVL_tree_bf_invariant(root->right) && Node_balance_check(root->right) == None;

	return thisResult && leftResult && rightResult;
}

bool Tree_final_insert_test()
{
	bool status = false;

	Node* root = Node_init();
	root->data = 100;

	for (int i = 99; i > 0; i--)
	{
		Tree_insert(&root, i);
	}

	status = Tree_AVL_tree_bf_invariant(root);

	for (int i = 101; i < 201; i++)
	{
		Tree_insert(&root, i);
	}

	status &= Tree_AVL_tree_bf_invariant(root);
	
	Tree_destroy(root);

	return status;
}

bool Tree_final_delete_test()
{
	bool status = false;

	Node* root = Node_init();
	root->data = 100;

	for (int i = 99; i > 0; i--)
	{
		Tree_insert(&root, i);
	}

	status = Tree_AVL_tree_bf_invariant(root);

	for (int i = 1; i < 50; i++)
	{
		Tree_delete(&root, i);
	}

	status &= Tree_AVL_tree_bf_invariant(root);

	Tree_destroy(root);
	return status;
}

int main()
{
	if (Tree_final_insert_test())
	{
		printf("Insert test passed!\n");
	}

	if (Tree_final_delete_test())
	{
		printf("Delete test passed!\n");
	}
}

