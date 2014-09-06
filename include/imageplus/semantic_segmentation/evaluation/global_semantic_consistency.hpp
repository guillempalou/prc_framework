//
//  global_semantic_consistency.h
//  prc
//
//  Created by Guillem Palou Visa on 06/09/14.
//  Copyright (c) 2014 Guillem Palou Visa. All rights reserved.
//

#ifndef prc_global_semantic_consistency_h
#define prc_global_semantic_consistency_h

#include <imageplus/core/image_signal.hpp>
#include <imageplus/segmentation/partition/partition.hpp>

#include <set>
#include <queue>
#include <algorithm>
#include <map>


namespace imageplus {
	namespace semantic_segmentation {
        
        template<class ImageModel>
        class GlobalSemanticConsistency {
            
            typedef ImageModel ImageType;
            
            std::map<uint64, uint64> index;
            
        public:
            
            struct MatchingStruct {
                float64 true_precision;
                float64 true_recall;
                float64 inconsistent_precision;
                float64 inconsistent_recall;
                
                MatchingStruct() : true_precision(0), true_recall(0), inconsistent_precision(0), inconsistent_recall(0) {
                }
            } ;
            
            uint64 label_index(typename ImageType::value_type label) {
                uint64 l = label(0)*256*256 + label(1)*256 + label(2);
                if (index.find(l) == index.end()) {
                    return (index[l] = index.size());
                } else {
                    return index[l];
                }
            }
            
            
            MatchingStruct evaluate(ImageType& result, ImageType& gt) {
                
                segmentation::Partition<float64,2> p_result(result.sizes());
                segmentation::Partition<float64,2> p_groundtruth(result.sizes());
                
                p_result.set_flatzone_labels<Connectivity2D4>(result);
                p_groundtruth.set_flatzone_labels<Connectivity2D4>(gt);
                
                uint64 N1 = p_result.max_label();
                uint64 N2 = p_groundtruth.max_label();
                
                std::vector<uint64> label_result(N1);
                std::vector<uint64> label_groundtruth(N2);
                
                // compute jaccard index
                math::Matrix intersection(N1,N2); intersection.fill(0);
                
                math::Vector area1(N1); area1.fill(0);
                math::Vector area2(N2); area2.fill(0);
                
                std::vector<bool> ignore(N2,false);
                
                std::map<uint64, uint64> categories;

                // used for the hash
                typename ImageType::value_float_type idx(1000000, 1000, 1);
                
                
                for (typename ImageType::iterator p = result.begin(); p != result.end(); ++p) {
                    typename ImageType::coord_type c = p.pos();
                    
                    uint64 l1 = p_result(c)(0)-1;
                    uint64 l2 = p_groundtruth(c)(0)-1;
                    
                    // compute category labels
                    uint64 c1 = idx.transpose() * result(c);
                    uint64 c2 = idx.transpose() * gt(c);
                    
                    // add the categories
                    categories[l1] = c1;
                    categories[l2] = c2;
                    
                    intersection(l1,l2)++;
                    area2(l2)++;
                    area1(l1)++;
                }
                
                std::cout << "Find assignments done " << std::endl;
                
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
                
                
                float64 tp = 0;
                float64 ip = 0;
                float64 fp = 0;
                float64 fn = 0;
                
                math::Vector consistent_detections(N2);
                math::Vector inconsistent_detections(N2);
                consistent_detections.fill(0);
                inconsistent_detections.fill(0);
                
                for (uint64 i = 0; i < N1; i++) {
                    uint64 j = assignments[i];
                    uint64 result_category = categories[i];
                    uint64 gt_category = categories[j];
                    if (result_category == gt_category) {
                        consistent_detections(j)++;
                    } else {
                        inconsistent_detections(j)++;
                    }
                }
                

                for (uint64 i = 0; i < N2; i++) {
                    if (ignore[i] == true) continue;
                    uint64 cd = consistent_detections(i);
                    uint64 id = inconsistent_detections(i);
                    
                    if (cd+id == 0) fn++; else{
                        tp += 1.0*cd/(cd+id);
                        ip += 1.0*id/(cd+id);
                    }
                }
                
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


#endif
