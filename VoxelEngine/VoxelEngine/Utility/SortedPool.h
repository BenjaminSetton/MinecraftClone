#pragma once

#include "Utility.h"

// SortedPool templated class sorts T
// objects in the dynamically allocated
// pool to reduce dynamic allocations
// every frame

template<typename T>
class SortedPool
{

public:

	SortedPool(const uint32_t& numberOfInstances)
	{	
		if (m_pool) VX_ASSERT_MSG(false, "Sorted pool should not exist here");

		m_pool = new T[numberOfInstances];
		m_size = 0;
		m_capacity = numberOfInstances;
	}

	SortedPool(const SortedPool& other)
	{
		// TODO: Implement
	}

	~SortedPool()
	{
		if (m_pool) delete m_pool;
		m_size = 0;
		m_capacity = 0;
	}

	T* GetAt(const uint32_t& index)
	{
		if (index > m_size) return nullptr;

		return &m_pool[index];
	}

	T* Insert(const T object)
	{
		if (m_size >= m_capacity) return nullptr;
		
		m_pool[m_size++] = object;

		return &m_pool[m_size - 1];
	}

	void Remove(const uint32_t& index)
	{
		if (index >= m_size || m_size <= 0) return;

		T temp = m_pool[index];
		m_pool[index] = m_pool[m_size - 1];
		m_pool[m_size - 1] = temp;

		m_size--;

	}

	void Clear()
	{
		memset(&m_pool[0], 0, sizeof(T) * m_capacity);
	}

	T* operator[](const uint32_t index)
	{
		return GetAt(index);
	}

	const uint32_t Size() { return m_size; }

	const uint32_t Capacity() { return m_capacity; }

private:

	// REMARKS:
	// m_size can be used as an index to the last available space
	// if m_size == m_capacity, there is no more space in the pool
	uint32_t m_size = 0;
	uint32_t m_capacity = 0;

	// Dynamically allocated pool object
	T* m_pool = nullptr;
};

