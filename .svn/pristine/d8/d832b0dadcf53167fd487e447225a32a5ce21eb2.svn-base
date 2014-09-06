/*
 * boundary_recall.hpp
 *
 *  Created on: Jan 8, 2013
 *      Author: gpalou
 */

#ifndef BOUNDARY_RECALL_HPP_
#define BOUNDARY_RECALL_HPP_

namespace imageplus {
	namespace segmentation {

	template<class PartitionModel>
	float64 boundary_recall(PartitionModel& partition, PartitionModel& groundtruth) {

		float64 groundtruth_contours = 0;
		float64 true_positives = 0;

		typename PartitionModel::iterator p = partition.begin();
		typename PartitionModel::iterator end = partition.end();

		typedef typename PartitionModel::template general_adjacency_iterator<PartitionModel::default_forward_connectivity>::type  adj_iterator;
		for (; p != end; ++p) {
			typename PartitionModel::coord_type pos = p.pos();
			adj_iterator adj		 	= partition.template general_adjacency_begin<PartitionModel::default_forward_connectivity>(p.pos());
			adj_iterator adj_end 		= partition.template general_adjacency_end<PartitionModel::default_forward_connectivity>(p.pos());

			uint64 label 	= (*p)(0);
			uint64 label_gt = groundtruth(pos)(0);

			for (; adj!=adj_end;++adj) {
				uint64 label_adj 		= (*adj)(0);
				uint64 label_adj_gt 	= groundtruth(adj.pos())(0);
				if (label_gt != label_adj_gt) {
					groundtruth_contours++;
					if (label_adj !=label) {
						true_positives++;
					}
				}
			}
		}

		return true_positives / groundtruth_contours;
	}

	}
}

#endif /* BOUNDARY_RECALL_HPP_ */
