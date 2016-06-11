#ifndef _CONCURRENTDAG_HPP_
#define _CONCURRENTDAG_HPP_

#include <cassert>
#include <cstdint>
#include <map>
#include <mutex>
#include <unordered_map>
#include <unordered_set>

#include <boost/optional.hpp>

// ------------------------------------------------------------------
//
// @details This class provides the Graph structure required to represent
// the work items as a DAG.  It performs all the topological sorting
// and tracking of items as they are added and removed from the DAG, ensuring
// that work item are correctly added and ordered for computation.
//
// ------------------------------------------------------------------
template <typename T>	// T must be a std::shared_ptr
class ConcurrentDAG
{
public:
	// ------------------------------------------------------------------
	//
	// @details A group operation is needed when adding multiple nodes to the DAG
	// that have dependencies among them.  The group operatrion prevents any other
	// threads from adding or removing nodes during the group operation.
	//
	// ------------------------------------------------------------------
	void beginGroup()		{ m_mutex.lock(); }
	void endGroup()			{ m_mutex.unlock(); }

	// ------------------------------------------------------------------
	//
	// @details This function allows two nodes that have a dependency between
	// them to be added.
	// NOTE: Should probably add code to ensure that a node isn't being added that causes
	// a dependency on itself.
	//
	// ------------------------------------------------------------------
	void addEdge(T source, T dependent)
	{
		std::lock_guard<std::recursive_mutex> lock(m_mutex);
		//
		// Add these to the master list of nodes.
		m_nodes[source->getId()] = source;
		m_nodes[dependent->getId()] = dependent;

		//
		// Add to the adjacency list
		if (m_adjacent.find(source->getId()) == m_adjacent.end())
		{
			m_adjacent[source->getId()] = std::unordered_set<uint64_t>();
		}
		m_adjacent[source->getId()].insert(dependent->getId());

		//
		// Add the reference back into the adjacency list so that when the node
		// is removed, it can also be removed from all nodes that depended upon it.
		if (m_reference.find(dependent->getId()) == m_reference.end())
		{
			m_reference[dependent->getId()] = std::unordered_set<uint64_t>();
		}
		m_reference[dependent->getId()].insert(source->getId());
	}

	// ------------------------------------------------------------------
	//
	// @details Adds a single node to the DAG, one that has no dependencies
	// to or from any other nodes in the DAG.
	//
	// ------------------------------------------------------------------
	void addNode(T one)
	{
		//
		// adding a single node is considered a full group operation itself
		std::lock_guard<std::recursive_mutex> lock(m_mutex);

		m_nodes[one->getId()] = one;
		//
		// Create an empty adjacency list
		m_adjacent[one->getId()] = std::unordered_set<uint64_t>();
	}

	// ------------------------------------------------------------------
	//
	// @details Returns the next unused node that has no dependencies
	//
	// ------------------------------------------------------------------
	boost::optional<T> dequeue()
	{
		//
		// dequeue is considered a full group operation itself
		std::lock_guard<std::recursive_mutex> lock(m_mutex);

		boost::optional<T> item = boost::none;
		//
		// return the next highest priority item that has no nodes in the adjacency list
		auto found = bool{ false };
		for (const auto& candidate : m_nodes)
		{
			auto itr = m_reference.find(candidate.first);
			if ((itr == m_reference.end() || 
				itr->second.size() == 0) &&
				m_inUse.find(candidate.first) == m_inUse.end())
			{
				found = true;
				item = candidate.second;
				m_inUse.insert(candidate.first);
				break;
			}
		}

		return item;
	}

	// ------------------------------------------------------------------
	//
	// @details Removes (finalizes) the node from the DAG. This node must 
	// have been returned from the .dequeue method!
	//
	// ------------------------------------------------------------------
	void finalize(const T& node)
	{
		//
		// remove is considered a full group operation itself
		std::lock_guard<std::recursive_mutex> lock(m_mutex);

		//
		// As a quick gut check, just make sure it really is in the inUse
		// container...that means that it was dequeued.
		assert(m_inUse.find(node->getId()) != m_inUse.end());
		//
		// Using the adjacent list, remove it from the reference sets
		for (auto id : m_adjacent[node->getId()])
		{
			m_reference[id].erase(node->getId());
		}
		m_adjacent.erase(node->getId());
		m_nodes.erase(node->getId());
		m_inUse.erase(node->getId());
	}

private:
	std::recursive_mutex m_mutex;
	std::map<uint64_t, T> m_nodes;												// Set of all nodes in the graph
	std::unordered_map<uint64_t, std::unordered_set<uint64_t>> m_adjacent;		// Adjacency list
	std::unordered_map<uint64_t, std::unordered_set<uint64_t>> m_reference;		// Set of nodes to which each node is referenced in the adjacency list
	std::unordered_set<uint64_t> m_inUse;										// Set of nodes dequeued, but not yet removed
};

#endif // _CONCURRENTDAG_HPP_
