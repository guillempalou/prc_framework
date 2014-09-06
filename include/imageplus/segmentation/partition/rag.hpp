/*
 * rag.hpp
 *
 *  Created on: Dec 21, 2012
 *      Author: gpalou
 */

#ifndef RAG_HPP_
#define RAG_HPP_

#include <imageplus/math/graphs/graph.hpp>

namespace imageplus {
	namespace segmentation {

	template<class PartitionModel, uint64 dimensions, ConnectivityType adjacency_type = neighborhood_traits<dimensions>::default_forward_connectivity>
	class RAG {

		typedef PartitionModel		PartitionType;

	public:

		typedef math::graphs::BoostGraph<math::graphs::kGraphUndirected> Graph;

		//! Inits the set of regions scanning _leaves_partition
		//! This function must be implemented
		void init(PartitionType& partition)
		{
			// Scan all regions
			typename PartitionType::iterator part_end = partition.end();
			typename PartitionType::iterator part_it = partition.begin();

			std::set<uint64> labels;
			for(; part_it != part_end; ++part_it) {
				uint64 label = (*part_it)(0);
				labels.insert(label);
			}

			_rag = Graph();

			std::map<uint64,Graph::Node> nodes;
			for (std::set<uint64>::iterator l = labels.begin(); l != labels.end(); l++) {
				Graph::Node n = _rag.add_node();
				_rag.node_properties(n).id = *l;
				nodes[*l] = n;
			}

			// Include neighbor information

			part_it = partition.begin();
			typedef typename PartitionType::template general_adjacency_iterator<adjacency_type>::type  adj_iterator;

			for(; part_it != part_end; ++part_it) {
				adj_iterator adj		 	= partition.template general_adjacency_begin<adjacency_type>(part_it.pos());
				adj_iterator adj_end 		= partition.template general_adjacency_end<adjacency_type>(part_it.pos());
				uint64 label = (*part_it)(0);

				for (; adj!=adj_end;++adj) {
					uint64 label_adj = (*adj)(0);

					if (label == label_adj) continue;
					if (_rag.edge_exists(nodes[label], nodes[label_adj])) continue;

					_rag.add_edge(nodes[label], nodes[label_adj]);
				}
			}

		}

		Graph& rag() {
			return _rag;
		}

	protected:

		Graph _rag;

	};

	}
}

#endif /* RAG_HPP_ */
