/*
 * gdc_evaluation.cpp
 *
 *  Created on: Apr 25, 2013
 *      Author: gpalou
 */

#include <imageplus/core/image_signal.hpp>
#include <imageplus/segmentation/partition/partition.hpp>
#include <imageplus/segmentation/visualization/false_color.hpp>
#include <imageplus/monocular_depth/evaluation/global_depth_consistency.hpp>

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

	monocular_depth::GlobalDepthConsistency<ImageType> 						gdc;
	monocular_depth::GlobalDepthConsistency<ImageType>::MatchingStruct 		m = gdc.evaluate(img,gt);

	std::cout << m.true_precision << " " << m.inconsistent_precision << " " << m.true_recall << " " << m.inconsistent_recall << std::endl;

	//if (argc > 3) {
	//	std::string result_name = argv[3];
		//std::cout << result_name << std::endl;
	//	m.img_boundaries.write(result_name);
	//}
}
