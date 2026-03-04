#pragma once
#include <stack>
#include "Shared/Graph/Graph.h"

namespace GameAI
{
	enum class Eulerianity
	{
		notEulerian,
		semiEulerian,
		eulerian,
	};

	class EulerianPath final
	{
	public:
		EulerianPath(Graph* const pGraph);

		Eulerianity IsEulerian() const;
		std::vector<Node*> FindPath(Eulerianity& eulerianity) const;

	private:
		void VisitAllNodesDFS(const std::vector<Node*>& pNodes, std::vector<bool>& visited, int startIndex) const;
		bool IsConnected() const;

		Graph* m_pGraph;
	};

	inline EulerianPath::EulerianPath(Graph* const pGraph)
		: m_pGraph(pGraph)
	{
	}

	inline Eulerianity EulerianPath::IsEulerian() const
	{
		// TODO If the graph is not connected, there can be no Eulerian Trail
		if (!IsConnected()) return Eulerianity::notEulerian;

		// TODO Count nodes with odd degree
		auto nodes = m_pGraph->GetNodes();
		auto connection = m_pGraph->GetConnections();

		int numOddsNodes{ 0 };

		for (int index = 0; index < nodes.size(); ++index)
		{
			if (m_pGraph->FindConnectionsFrom(index).size() % 2 == 0) continue;
			
			++numOddsNodes;
		}

		// TODO A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian
		if (numOddsNodes < 2) return Eulerianity::notEulerian;

		// TODO A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian (unless there are only 2 nodes)
		if (numOddsNodes == 2) return Eulerianity::semiEulerian;

		// TODO An Euler trail can be made, but only starting and ending in these 2 nodes
		// start at 1 of the 2 and end at the other one
		// does not need to be implented here

		// TODO A connected graph with no odd nodes is Eulerian
		if (numOddsNodes == 0) return Eulerianity::eulerian;

		return Eulerianity::eulerian;
	}

	inline std::vector<Node*> EulerianPath::FindPath(Eulerianity& eulerianity) const
	{
		// Get a copy of the graph because this algorithm involves removing edges
		Graph graphCopy = m_pGraph->Clone();
		std::vector<Node*> Path = {};
		std::vector<Node*> Nodes = graphCopy.GetActiveNodes();
		int currentNodeId{ Graphs::InvalidNodeId };
		
		// TODO Check if there can be an Euler path
		// TODO If this graph is not eulerian, return the empty path
		
		switch (eulerianity)
		{
		case GameAI::Eulerianity::notEulerian:
			return Path;
			break;
		case GameAI::Eulerianity::semiEulerian:
			for (int index = 0; index < Nodes.size(); ++index)
			{
				if (m_pGraph->FindConnectionsFrom(index).size() % 2 == 0) continue;

				currentNodeId = index;
				break;
			}
			break;
		case GameAI::Eulerianity::eulerian:
			currentNodeId = 0;
			break;
		}

		// TODO Start algorithm loop
		std::stack<int> nodeStack;

		bool currentHasNeighbors = true;

		while (currentHasNeighbors && !nodeStack.empty())
		{
			auto connections = graphCopy.FindConnectionsFrom(currentNodeId);

			currentHasNeighbors = connections.size() > 0;

			if (currentHasNeighbors)
			{
				int newId = connections[0]->GetToId();

				graphCopy.RemoveConnection(connections[0]);

				currentNodeId = newId;


			}
		}

		std::reverse(Path.begin(), Path.end());
		return Path;
	}

	inline void EulerianPath::VisitAllNodesDFS(const std::vector<Node*>& Nodes, std::vector<bool>& visited, int startIndex ) const
	{
		// TODO Mark the visited node
		if (Nodes.size() != visited.size())
		{
			visited = std::vector<bool>(Nodes.size(), false);
		}

		visited[startIndex] = true;

		// TODO Ask the graph for the connections from that node
		auto connections = m_pGraph->FindConnectionsFrom(startIndex);

		// TODO recursively visit any valid connected nodes that were not visited before
		// TODO Tip: use an index-based for-loop to find the correct index

		for (int index = 0; index < connections.size(); ++index)
		{
			int id = connections[index]->GetToId();

			if (visited[id]) continue;

			VisitAllNodesDFS(Nodes, visited, id);
		}

	}

	inline bool EulerianPath::IsConnected() const
	{
		std::vector<Node*> Nodes = m_pGraph->GetActiveNodes();
		if (Nodes.size() == 0)
			return false;


		std::vector<bool> visitedNodes(Nodes.size(), false);
		// TODO choose a starting node

		// TODO start a depth-first-search traversal from the node that has at least one connection
		VisitAllNodesDFS(Nodes, visitedNodes, 0);

		// TODO if a node was never visited, this graph is not connected
		for (const auto& visited : visitedNodes)
		{
			if (!visited) return false;
		}

		return true;
	}
}