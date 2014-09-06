/*
 * image_signal.hpp
 *
 *  Created on: Jul 25, 2012
 *      Author: guillem
 */

#ifndef IMAGE_SIGNAL_HPP_
#define IMAGE_SIGNAL_HPP_

#include <opencv2/opencv.hpp>

#include <imageplus/core/signal.hpp>
#include <imageplus/core/colorspaces.hpp>

#include <boost/filesystem.hpp>

namespace imageplus {

	//! Class ImageSignal, base class for all the images
	template<typename channel_type, uint64 channels>
	class ImageSignal : public Signal<int64, channel_type, 2, channels> {

	public:

		//! base class type
		typedef Signal<int64, channel_type, 2, channels> 													BaseClassType;

		//!Vector representing the values of a coord
		typedef typename BaseClassType::coord_type															coord_type;

		//! Vector representing the values of a pixel
		typedef typename BaseClassType::value_type															value_type;

		//! value of type of the data of the pixel (int,float...)
		typedef typename BaseClassType::value_data_type														value_data_type;

		//! value of type of the data of the pixel (uint64)
		typedef typename BaseClassType::coord_data_type														coord_data_type;

		//!Vector representing the values of a coord
		typedef typename BaseClassType::coord_float_type													coord_float_type;

		//! Vector representing the values of a pixel
		typedef typename BaseClassType::value_float_type													value_float_type;

		//! Type representing a channel
		typedef Eigen::Matrix<value_data_type, Eigen::Dynamic, Eigen::Dynamic>								ChannelMatrixType;

		//! Type used for strides
		typedef Eigen::Stride<Eigen::Dynamic, Eigen::Dynamic>												StrideType;

		//! Type representing a view of a channel
		typedef Eigen::Map<ChannelMatrixType,0, StrideType>													ChannelType;

		//! number of channels of the image
		static const int64 num_channels = channels;

		//! Default constructor
		ImageSignal() : BaseClassType() {
		}

		//! Default constructor
		//! \param[in] sx: size x
		//! \param[in] sy: size y
		ImageSignal(uint64 sx, uint64 sy) : BaseClassType(coord_type(sx,sy)) {
			_sx = sx;
			_sy = sy;
			_color_space = ColorSpaceRGB;
		}

		//! constructor with sizes
		//! \param[in] t : time span of the video
		ImageSignal(const coord_type& size) : BaseClassType(size)  {
			_sx = size(0);
			_sy = size(1);
			_color_space = ColorSpaceRGB;
		}

		//! constructor with sizes
		//! \param[in] t : time span of the video
		ImageSignal(const ImageSignal& copy) : BaseClassType(const_cast<ImageSignal&>(copy)), _color_space(copy._color_space) , _sx(copy._sx), _sy(copy._sy) {
		}

		//! constructor with a pointer
		//! \param[in] sx : width
		//! \param[in] sy : height
		//! \param[in] sx : buffer to the data
		//! \param[in] color_space : color space of the data
		ImageSignal(uint64 sx, uint64 sy, value_data_type* data, ColorSpaceType color_space) : BaseClassType(coord_type(sx,sy),data) {
			_sx = sx;
			_sy = sy;
			_color_space = color_space;
		}

		//! Returns the x-size of the image
		//! \return columns of the image
		inline uint64 size_x() {
			return _sx;
		}

		//! Returns the y-size of the image
		//! \return rows of the image
		inline uint64 size_y() {
			return _sy;
		}

		//! Returns the color space of the signal
		ColorSpaceType color_space() {
			return _color_space;
		}

		//! Sets the color space variable (IT DOES NOT CHANGE THE PIXEL value)
		//! \param[in] color_space: space
		void set_color_space(ColorSpaceType color_space) {
			_color_space = color_space;
		}

		ChannelType channel(uint64 channel) {
			value_data_type* data = BaseClassType::data();
			return ChannelType(data+channel,_sx,_sy, StrideType(_sx*channels,channels));
		}


		void read(std::string path) {
			if ( !boost::filesystem::exists(path) ) {
			  std::cerr << "Could not read file: " << path << std::endl;
			  throw;
			}
			cv::Mat img;
			if (channels > 1) {
				img = cv::imread(path, CV_LOAD_IMAGE_COLOR);
			} else {
				img = cv::imread(path, CV_LOAD_IMAGE_GRAYSCALE);
			}

			_sx = img.cols;
			_sy = img.rows;

			//uint64 size = _sx*_sy*num_channels*sizeof(value_data_type);

			// Add a reference counter since now we are responsible of the image
			BaseClassType::init_data(coord_type(_sx,_sy));

			value_data_type* data = BaseClassType::data();

			// export_to transfer the data from img to buffer
			for(uint64 i = 0; i < _sy; i++)
			{
				uint8* m = img.ptr<uint8>(i);
			    for(uint64 j = 0; j < _sx*num_channels; j++)
			        (*data++) += static_cast<value_data_type>(m[j]);
			}

			_color_space = ColorSpaceRGB;
		}

		void write(std::string path) {
			if (_color_space != ColorSpaceRGB) {
				ImagePlusError("Cannot save a different colorspace other than RGB");
				return;
			}

			// import_from transfer the data from buffer to img
			int type = CV_MAKETYPE(cv::DataType<value_data_type>::depth, channels);

			cv::Mat img(_sy,_sx,type,BaseClassType::data());
			cv::Mat output;

			img.convertTo(output,CV_8UC3);

			if (!cv::imwrite(path,img))
				std::cerr << "Could not write image" << std::endl;
				//throw;
		}

		value_float_type interpolate_value(float64 x, float64 y) {
			return interpolate_value(coord_float_type(x,y));
		}

		//! Provides a method for bilinear interpolation
		value_float_type interpolate_value(coord_float_type v) {
			value_float_type p = value_float_type::Zero();

			//Check for bounds
			const coord_type& lower = this->lower_point();
			const coord_type& upper = this->upper_point();

			coord_float_type a = v - lower.template cast<float64>();
			coord_float_type b = upper.template cast<float64>() - v;

			// Check if the interpolating point is outside
			if (a.minCoeff() < 0 || b.minCoeff() < 0) {
				return p;
			}
			// Coordinates of the four neighboring pixels (x1,y1) (x1,y2) (x2,y2) (x2,y1) and the third coordinate is frame

			uint64 x1 = (int)v(0);
			uint64 x2 = x1+1;
			uint64 y1 = (int)v(1);
			uint64 y2  = y1+1;

			// Linear interpolation

			value_float_type v1,v2,v3,v4;
			v1 = this->operator()(x1,y1).template cast<float64>();
			if (x2 >= _sx) v2 = v1; else v2 = this->operator()(x2,y1).template cast<float64>();
			if (y2 >= _sy) v3 = v1; else v3 = this->operator()(x1,y2).template cast<float64>();
			if (x2 >= _sx && y2 >= _sy) v4 = v1; else {
				if (x2 >= _sx) v4 = v2; else
					if (y2 >= _sy) v4 = v3; else v4 = this->operator()(x2,y2).template cast<float64>();
			}

			double xp = 1-(v(0)-x1);
			double yp = 1-(v(1)-y1);

			p = xp*yp*v1 + (1-xp)*yp*v2 + xp*(1-yp)*v3 + (1-xp)*(1-yp)*v4;

			return p;
		}

		//! Iterator definitions
	public:


	protected:

		//! color space
		ColorSpaceType _color_space;

		//! size x
		uint64 _sx;

		//! size y
		uint64 _sy;
	};

}


#endif /* IMAGE_SIGNAL_HPP_ */
