//
//  lsc_evaluation.c
//  prc
//
//  Created by Guillem Palou Visa on 06/09/14.
//  Copyright (c) 2014 Guillem Palou Visa. All rights reserved.
//

#include <stdio.h>

/*
 * fg_pr.cpp
 *
 *  Created on: Apr 17, 2013
 *      Author: gpalou
 */

#include <imageplus/core/image_signal.hpp>
#include <imageplus/segmentation/partition/partition.hpp>
#include <imageplus/segmentation/visualization/false_color.hpp>
#include <imageplus/semantic_segmentation/evaluation/local_semantic_consistency.hpp>

#include <boost/filesystem.hpp>
#include <iomanip>
#include <iostream>
#include <string>

using namespace imageplus;

#define uint64 imageplus::uint64
#define int64 imageplus::int64

typedef ImageSignal<float64,3> 	ImageType;

int main(int argc, char *argv[]) {
	std::string partition_path 		= argv[1];
	std::string groundtruth_path 	= argv[2];
    
	ImageType img, gt;
	img.read(partition_path);
	gt.read(groundtruth_path);
    
	semantic_segmentation::SemanticMatcher<ImageType> fg;
	semantic_segmentation::MatchingStruct m = fg.match_contours(img,gt);
    
	std::cout << m.true_precision << " " << m.inconsistent_precision << " " << m.true_recall << " " << m.inconsistent_recall << std::endl;
}
