/*
 * global_depth_consistency.hpp
 *
 *  Created on: Apr 25, 2013
 *      Author: gpalou
 */

#ifndef GLOBAL_DEPTH_CONSISTENCY_HPP_
#define GLOBAL_DEPTH_CONSISTENCY_HPP_

#include <imageplus/core/image_signal.hpp>
#include <imageplus/segmentation/partition/partition.hpp>

#include <set>
#include <queue>
#include <algorithm>
#include <map>


namespace imageplus {
	namespace monocular_depth {

	template<class ImageModel>
	class GlobalDepthConsistency {

		typedef ImageModel ImageType;

	public:
		struct MatchingStruct {
			float64 true_precision;
			float64 true_recall;
			float64 inconsistent_precision;
			float64 inconsistent_recall;

			MatchingStruct() : true_precision(0), true_recall(0), inconsistent_precision(0), inconsistent_recall(0) {
			}
		} ;

		MatchingStruct evaluate(ImageType& result,ImageType& gt) {
			segmentation::Partition<float64,2> p_result(result.sizes());
			segmentation::Partition<float64,2> p_groundtruth(result.sizes());

			p_result.set_flatzone_labels<Connectivity2D4>(result);
			p_groundtruth.set_flatzone_labels<Connectivity2D4>(gt);

			uint64 N1 = p_result.max_label();
			uint64 N2 = p_groundtruth.max_label();

			std::vector<float64> depth_result(N1);
			std::vector<float64> depth_groundtruth(N2);

			// compute jaccard index
			math::Matrix intersection(N1,N2); intersection.fill(0);

			math::Vector area1(N1); area1.fill(0);
			math::Vector area2(N2); area2.fill(0);

			std::vector<bool> ignore(N2,false);

			for (typename ImageType::iterator p = result.begin(); p != result.end(); ++p) {
				typename ImageType::coord_type c = p.pos();

				uint64 l1 = p_result(c)(0)-1;
				uint64 l2 = p_groundtruth(c)(0)-1;

				depth_result[l1] = result(c)(0);
				depth_groundtruth[l2] = gt(c)(0);

				intersection(l1,l2)++;
				area2(l2)++;
				area1(l1)++;
			}
			//for (uint64 i = 0; i < N2; i++) if (area2[i] < 10) ignore[i] = true;

			//std::cout << intersection << "-------------------" << std::endl;

			// Assign each region to the maximum jaccard index
			std::vector<uint64> assignments(N1);
			for (uint64 i = 0; i < N1; i++) {
				uint64 	max_k;
				float64 max_val = 0;
				for (uint64 k = 0; k < N2; k++) {
					if (max_val < intersection(i,k) && ignore[k] == false) {max_val = intersection(i,k); max_k = k;}
				}
				assignments[i] = max_k;
			}

			//std::cout << N1 << " " << N2 << "::::::" << std::endl;
			//for (uint64 i = 0; i < N1; i++) std::cout << assignments[i] << " ";
			//std::cout << std::endl;

			math::Matrix result_transitions(N1,N1);
			math::Matrix gt_transitions(N2,N2);

			for (uint64 i = 0; i < N1; i++) {
				for (uint64 k = 0; k < N1; k++) {
					if (depth_result[i] > depth_result[k]) result_transitions(i,k) = 1;
					if (depth_result[i] < depth_result[k]) result_transitions(i,k) = -1;
					if (depth_result[i] == depth_result[k]) result_transitions(i,k) = 0;
				}
			}

			for (uint64 i = 0; i < N2; i++) {
				for (uint64 k = 0; k < N2; k++) {
					if (depth_groundtruth[i] > depth_groundtruth[k]) gt_transitions(i,k) = 1;
					if (depth_groundtruth[i] < depth_groundtruth[k]) gt_transitions(i,k) = -1;
					if (depth_groundtruth[i] == depth_groundtruth[k]) gt_transitions(i,k) = 0;
				}
			}

			//std::cout << result_transitions << std::endl << "-------------------" << std::endl;
			//std::cout << gt_transitions << std::endl << "-------------------" << std::endl;

			float64 tp = 0;
			float64 ip = 0;
			float64 fp = 0;
			float64 fn = 0;

			math::Matrix consistent_detections(N2,N2);
			math::Matrix inconsistent_detections(N2,N2);
			consistent_detections.fill(0);
			inconsistent_detections.fill(0);

			for (uint64 i = 0; i < N1; i++) {
				for (uint64 k = i+1; k < N1; k++) {
					uint64 idi = assignments[i];
					uint64 idk = assignments[k];
					float64 trans 		= result_transitions(i,k);
					float64 gt_trans 	= gt_transitions(idi,idk);
					//count oversegmentation as false positives
					if (idi == idk && trans != 0) fp++; else {
						if (trans == gt_trans) consistent_detections(idi,idk)++;
						if (trans != gt_trans) inconsistent_detections(idi,idk)++;
					}
				}
			}
			for (uint64 i = 0; i < N2; i++) {
				if (ignore[i] == true) continue;
				for (uint64 k = i+1; k < N2; k++) {
					if (ignore[k] == true) continue;
					uint64 cd = consistent_detections(i,k);
					uint64 id = inconsistent_detections(i,k);
					if (cd+id == 0) fn++; else{
						tp += 1.0*cd/(cd+id);
						ip += 1.0*id/(cd+id);
					}
				}
			}

			//std::cout << tp << " " << ip << " " << fp << " " << fn << std::endl;
			MatchingStruct m;

			if (tp+ip+fp == 0) {
				m.true_precision 			= 1;
				m.inconsistent_precision 	= 0;
			} else {
				m.true_precision 			= 	tp / (tp+ip+fp);
				m.inconsistent_precision 	= 	ip / (tp+ip+fp);
			}

			if (tp+ip+fn == 0) {
				m.true_recall 				=	0;
				m.inconsistent_recall 		=	0;
			} else {
				m.true_recall 				=	tp / (tp+ip+fn);
				m.inconsistent_recall 		=	ip / (tp+ip+fn);
			}

			return m;
		}

	};

	}
}
#endif /* GLOBAL_DEPTH_CONSISTENCY_HPP_ */
