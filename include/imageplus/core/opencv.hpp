/*
 * opencv.hpp
 *
 *  Created on: Dec 10, 2012
 *      Author: guillem
 */

#ifndef OPENCV_HPP_
#define OPENCV_HPP_

#include <opencv2/opencv.hpp>

namespace imageplus {

	template<class Signal>
	cv::Mat to_opencv(Signal& s) {
		int type = CV_MAKETYPE(cv::DataType<typename Signal::value_data_type>::depth, s.value_dimensions);

		int sizes[s.coord_dimensions];

		if (s.coord_dimensions == 1) sizes[0] = s.sizes()(0);
		if (s.coord_dimensions == 2) {
			sizes[0] = s.sizes()(1);
			sizes[1] = s.sizes()(0);
		}
		if (s.coord_dimensions == 3) {
			sizes[0] = s.sizes()(1);
			sizes[1] = s.sizes()(0);
			sizes[2] = s.sizes()(2);
		}

		cv::Mat out(s.coord_dimensions, sizes, type, s.data());

		return out;
	}

	/*template<class Signal>
	Signal to_imageplus(cv::Mat& m) {
		//return Signal(m,);
	}*/

}


#endif /* OPENCV_HPP_ */
