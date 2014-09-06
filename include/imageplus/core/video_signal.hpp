/*
 * video_signal.hpp
 *
 *  Created on: Jul 27, 2012
 *      Author: guillem
 */

#ifndef VIDEO_SIGNAL_HPP_
#define VIDEO_SIGNAL_HPP_

#include <opencv2/opencv.hpp>
#include <imageplus/core/signal.hpp>
#include <imageplus/core/image_signal.hpp>
#include <imageplus/core/colorspaces.hpp>

namespace imageplus {

	//! Class VideoSignal, base class for all the videos
	template<typename channel_type, uint64 channels>
	class VideoSignal : public Signal<int64, channel_type, 3, channels> {

	public:

		//! base class type
		typedef	Signal<int64, channel_type, 3, channels>			 										BaseClassType;

		//! class representing an frame
		typedef ImageSignal<channel_type,channels>															ImageType;

		//!Vector representing the values of a coord
		typedef typename BaseClassType::coord_type															coord_type;

		//! Vector representing the values of a pixel
		typedef typename BaseClassType::value_type															value_type;

		//! value of type of the data of the pixel (int,float...)
		typedef typename BaseClassType::value_data_type														value_data_type;

		//! value of type of the data of the pixel (int64)
		typedef typename BaseClassType::coord_data_type														coord_data_type;

		//! Frame iterator, typedef of a ROI iterator
		typedef typename BaseClassType::roi_iterator														frame_iterator;

		//! number of channels
		static const int64 num_channels = channels;

		//! Default constructor
		//! \param[in] sx : size x
		//! \param[in] sy : size y
		//! \param[in] t : time span of the video
		VideoSignal(uint64 sx, uint64 sy, uint64 t) : BaseClassType(coord_type(sx,sy,t)) {
			_sx = sx;
			_sy = sy;
			_time_span = t;
			_read_first_frame = false;
			_color_space = ColorSpaceRGB;
		}

		//! constructor only knowing the number of frames
		//! \param[in] t : time span of the video
		VideoSignal(uint64 t) {
			_time_span = t;
			_read_first_frame = false;
			_color_space = ColorSpaceRGB;
		}

		//! constructor only knowing the number of frames
		//! \param[in] t : time span of the video
		VideoSignal(const coord_type& sizes) : BaseClassType(sizes)  {
			_sx = sizes(0);
			_sy = sizes(1);
			_time_span = sizes(2);
			_read_first_frame = false;
			_color_space = ColorSpaceRGB;
		}

		//! constructor only knowing the number of frames
		//! \param[in] t : time span of the video
		VideoSignal(const VideoSignal& copy) : BaseClassType(const_cast<VideoSignal&>(copy))  {
			_sx = copy._sx;
			_sy = copy._sy;
			_time_span = copy._time_span;
			_read_first_frame = copy._read_first_frame;
			_color_space = copy._color_space;
		}

		//!Return a reference to an image
		//! \param[in] t : frame to retrieve
		ImageType frame(uint64 t) {
			coord_type offset(0,0,t);
			return ImageType(_sx,_sy,BaseClassType::data(offset),_color_space);
		}

		//! Returns the x-size of a frame
		//! \return columns of a frame
		uint64 size_x() {
			return _sx;
		}

		//! Returns the y-size of a frame
		//! \return rows of a frame
		uint64 size_y() {
			return _sy;
		}

		//! Returns the time span of the video
		//! \return number of frames
		uint64 time_span(){
			return _time_span;
		}

		//! Returns the time span of the video
		//! \return number of frames
		uint64 length(){
			return _time_span;
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

		//! Read frame
		void read_frame(std::string path, uint64 t) {
			cv::Mat img = cv::imread(path, CV_LOAD_IMAGE_COLOR);

			_sx = img.cols;
			_sy = img.rows;

			//uint64 size = _sx*_sy*num_channels*sizeof(value_data_type);

			// Add a reference counter since now we are responsible of the image
			if (_read_first_frame == false) {
				BaseClassType::init_data(coord_type(_sx,_sy,_time_span));
				_read_first_frame = true;
			}

			coord_type offset(0,0,t);
			value_data_type* data = BaseClassType::data(offset);

			// export_to transfer the data from img to buffer
			for(uint64 i = 0; i < _sy; i++)
			{
				uint8* m = img.ptr<uint8>(i);
				for(uint64 j = 0; j < _sx*num_channels; j++)
					(*data++) += static_cast<value_data_type>(m[j]);
			}

			_color_space = ColorSpaceRGB;
		}

		void write_frame(std::string path, uint64 t) {

			if (_color_space != ColorSpaceRGB) {
				ImagePlusError("Cannot save a different colorspace other than RGB");
				return;
			}

			// import_from transfer the data from buffer to img
			int type = CV_MAKETYPE(cv::DataType<value_data_type>::depth, channels);
			coord_type offset(0,0,t);
			value_data_type* data = BaseClassType::data(offset);
			cv::Mat img(_sy,_sx,type,data);
			cv::Mat output;

			img.convertTo(output,CV_8UC3);

			cv::imwrite(path,img);
		}

		//! Iterator definitions
	public:

		frame_iterator frame_begin(uint64 frame) {
			coord_type initial_point(0,0,frame);
			coord_type end_point(_sx-1,_sy-1,frame);
			return frame_iterator(*this, initial_point, end_point, false);
		}

		frame_iterator frame_end(uint64 frame) {
			coord_type initial_point(0,0,frame);
			coord_type end_point(_sx-1,_sy-1,frame);
			return frame_iterator(*this, initial_point, end_point, true);
		}

		frame_iterator frame_range_begin(uint64 initial_frame, uint64 end_frame) {
			coord_type initial_point(0,0,initial_frame);
			coord_type end_point(_sx-1,_sy-1,end_frame);
			return frame_iterator(*this, initial_point, end_point, false);
		}

		frame_iterator frame_range_end(uint64 initial_frame, uint64 end_frame) {
			coord_type initial_point(0,0,initial_frame);
			coord_type end_point(_sx-1,_sy-1,end_frame);
			return frame_iterator(*this, initial_point, end_point, true);
		}

	protected:

		//! color space
		ColorSpaceType _color_space;

		//! reading frames
		bool _read_first_frame;

		//! size x
		uint64 _sx;

		//! size y
		uint64 _sy;

		//! time span
		uint64 _time_span;
	};

}


#endif /* VIDEO_SIGNAL_HPP_ */
