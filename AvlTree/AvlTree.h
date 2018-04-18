#pragma once
#include <memory>

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

template<class T>
struct TreeNode
{
	TreeNode(T data) : m_data(data)
	{
	}

	void Delete(T value);
	void Insert(T value);
	void Destroy();
	int Height();

private:
	std::weak_ptr<TreeNode<T>> m_left{nullptr};
	std::weak_ptr<TreeNode<T>> m_right{nullptr};
	T m_data;

	void RotateRight();
	void RotateLeft();
	void RotateDoubleLeft();
	void RotateDoubleRight();

	BalanceRequirement BalanceCheck();
};
