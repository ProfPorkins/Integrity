/*
Copyright (c) 2015 James Dean Mathias

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _CONCURRENTDAG_HPP_
#define _CONCURRENTDAG_HPP_

#ifdef max
	#undef max
#endif

#include <iostream>
#include <cstdint>
#include <map>
#include <set>
#include <deque>
#include <mutex>
#include <atomic>

/// -----------------------------------------------------------------
///
/// @details A DAGNode is the internal storage structure used to hold the 
/// data element for a DAG node.  It is also the structure returned back
/// to an application for interaction with the main DAG class.  The reason
/// this is needed is that the DAGNode class is assigned a unique id
/// as it is added to the DAG, and this id is needed by the DAG class to
/// reference the node.
///
/// -----------------------------------------------------------------
template <typename T>
struct Node
{
	Node() {}
	Node(T value) :
		value(value)
	{
		//
		// Starting with the max value so that the std::map will order the items placed into the queue
		// based upon the time (order) they were added...with one little exceptional problem when the
		// value wraps around, I'll have to figure that one out still!  Maybe use a 64 bit value and don't
		// worry about it??
		static std::atomic<uint32_t> nextId = std::numeric_limits<uint32_t>::max();

		this->id = nextId;
		//
		// Because this is an unsigned value, when it overflows, it will wrap back around to 0, which
		// is what we want.  By that time (famous last words), the nodes assigned the ids that are being
		// reused will be long gone.
		nextId--;
	}
	uint32_t id;

	T value;
};

/// -----------------------------------------------------------------
///
/// @details This class provides the Graph structure required to represent
/// the work items as a DAG.  It performs all the topological sorting
/// and tracking of items as they are added and removed from the DAG, ensuring
/// that work item are correctly added and ordered for computation.
///
/// -----------------------------------------------------------------
template <typename T>
class ConcurrentDAG
{
public:
	ConcurrentDAG()
	{
	}

	/// -----------------------------------------------------------------
	///
	/// @details A transaction is needed when adding multiple nodes to the DAG
	/// that have dependencies among them.  The transaction prevents any other
	/// threads from adding or removing nodes during the transaction.
	///
	/// -----------------------------------------------------------------
	void beginTransaction()		{ m_mutex.lock(); }
	void endTransaction()		{ m_mutex.unlock(); }

	/// -----------------------------------------------------------------
	///
	/// @details This function allows two nodes that have a dependency between
	/// them to be added.
	/// NOTE: Should probably add code to ensure that a node isn't being added that causes
	/// a dependency on itself.
	///
	/// -----------------------------------------------------------------
	void addEdge(Node<T> source, Node<T> dependent)
	{
		//
		// Add these to the master list of nodes.
		m_nodes[source.id] = source;
		m_nodes[dependent.id] = dependent;

		//
		// Add to the adjacency list
		if (m_adjacent.find(source.id) == m_adjacent.end())
		{
			std::set<uint32_t> empty;
			m_adjacent[source.id] = empty;
		}
		m_adjacent[source.id].insert(dependent.id);

		//
		// Add the reference back into the adjacency list so that when the node
		// is removed, it can also be removed from all nodes that depended upon it.
		if (m_reference.find(dependent.id) == m_reference.end())
		{
			std::set<uint32_t> empty;
			m_reference[dependent.id] = empty;
		}
		m_reference[dependent.id].insert(source.id);

		m_visited[source.id] = false;
		m_visited[dependent.id] = false;
	}

	/// -----------------------------------------------------------------
	///
	/// @details Adds a single node to the DAG, one that has no dependencies
	/// to or from any other nodes in the DAG.
	///
	/// -----------------------------------------------------------------
	void addNode(Node<T> one)
	{
		//
		// adding a single node is considered a full transaction itself
		std::lock_guard<std::mutex> lock(m_mutex);

		m_nodes[one.id] = one;
		//
		// Create an empty adjacency list
		std::set<uint32_t> empty;
		m_adjacent[one.id] = empty;

		m_visited[one.id] = false;
	}

	/// -----------------------------------------------------------------
	///
	/// @details Returns the next unused node that has no dependencies
	///
	/// -----------------------------------------------------------------
	bool dequeue(Node<T>& node)
	{
		//
		// dequeue is considered a full transaction itself
		std::lock_guard<std::mutex> lock(m_mutex);
		//
		// return the next highest priority item that has no nodes in the adjacency list
		bool found = false;
		for (const auto& candidate : m_nodes)
		{
			auto itr = m_reference.find(candidate.first);
			if ((itr == m_reference.end() ||
				itr->second.size() == 0) &&
				m_inUse.find(candidate.first) == m_inUse.end())
			{
				found = true;
				node = candidate.second;
				m_inUse.insert(candidate.first);
				break;
			}
		}

		return found;
	}

	/// -----------------------------------------------------------------
	///
	/// @details Removes the node from the DAG. This node must have been
	/// returned from the .dequeue method!
	///
	/// -----------------------------------------------------------------
	void remove(Node<T> node)
	{
		//
		// remove is considered a full transaction itself
		std::lock_guard<std::mutex> lock(m_mutex);

		//
		// Using the adjacent list, remove it from the reference sets
		for (auto id : m_adjacent[node.id])
		{
			m_reference[id].erase(node.id);
		}
		m_adjacent.erase(node.id);
		m_nodes.erase(node.id);
		m_visited.erase(node.id);
		m_inUse.erase(node.id);
	}

	/// -----------------------------------------------------------------
	///
	/// @details This is a debugging method used to report the topological
	/// ordering of all nodes in the DAG.
	///
	/// -----------------------------------------------------------------
	void report()
	{
		//
		// report is considered a full transaction itself...but this isn't ever really
		// going to be run under production, only in debug.
		std::lock_guard<std::mutex> lock(m_mutex);

		std::deque<uint32_t> queue = topologicalSort();

		while (queue.empty() == false)
		{
			std::cout << queue.back() << " : ";
			std::cout << m_nodes[queue.back()].value << std::endl;
			queue.pop_back();
		}
	}

private:
	std::mutex m_mutex;
	std::map<uint32_t, Node<T>> m_nodes;					// Set of all nodes in the graph
	std::map<uint32_t, std::set<uint32_t>> m_adjacent;		// Adjacency list
	std::map<uint32_t, bool> m_visited;						// Used in the sort to indicate the visited status
	std::map<uint32_t, std::set<uint32_t>> m_reference;		// Set of nodes to which each node is referenced in the adjacency list
	std::set<uint32_t> m_inUse;								// Set of nodes dequeued, but not yet removed

	/// -----------------------------------------------------------------
	///
	/// @details This is the topological sort kickoff method.  Refer to this
	/// web reference for details on the implementation: http://www.geeksforgeeks.org/topological-sorting/
	///
	/// -----------------------------------------------------------------
	std::deque<uint32_t> topologicalSort()
	{
		std::deque<uint32_t> queue;
		
		for (auto& node : m_visited)
		{
			node.second = false;
		}

		for (auto& node : m_visited)
		{
			if (node.second == false)
			{
				topologicalSort(node.first, queue);
			}
		}

		return queue;	// Relying on move operator for efficient copy
	}

	/// -----------------------------------------------------------------
	///
	/// @details This is the supporting recursive topological sort method.
	///
	/// -----------------------------------------------------------------
	void topologicalSort(uint32_t id, std::deque<uint32_t>& queue)
	{
		m_visited[id] = true;

		for (auto id : m_adjacent[id])
		{
			if (m_visited[id] == false)
			{
				topologicalSort(id, queue);
			}
		}

		queue.push_back(id);
	}
};

#endif // _CONCURRENTDAG_HPP_
