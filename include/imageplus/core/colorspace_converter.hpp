/*
 * colorspace_converter.hpp
 *
 *  Created on: Oct 1, 2012
 *      Author: guillem
 */

#ifndef COLORSPACE_CONVERTER_HPP_
#define COLORSPACE_CONVERTER_HPP_

#include <imageplus/core/signal.hpp>
#include <imageplus/core/colorspaces.hpp>

namespace imageplus {

	template<class Signal>
	class ColorSpaceConverter {

		typedef typename Signal::value_type		value_type;

	public:

		inline void convert(Signal& s, ColorSpaceType output_color_space) {
			if (s.color_space() == output_color_space) return;


			for (typename Signal::iterator p = s.begin(); p != s.end(); ++p) {
				value_type v = convert(*p, s.color_space(), output_color_space);
				*p = v;
			}
			s.set_color_space(output_color_space);
		}

	protected:

		inline value_type convert(const value_type& v, ColorSpaceType input, ColorSpaceType output) {
			if (input == ColorSpaceRGB) {
				if (output == ColorSpaceRGB) return v;
				if (output == ColorSpaceYUV) return rgb_to_yuv(v);
				if (output == ColorSpaceLAB) return rgb_to_lab(v);
			}
			if (input == ColorSpaceYUV) {
				if (output == ColorSpaceRGB) return yuv_to_rgb(v);
				if (output == ColorSpaceYUV) return v;
				if (output == ColorSpaceLAB) return yuv_to_lab(v);
			}
			if (input == ColorSpaceLAB) {
				if (output == ColorSpaceRGB) return lab_to_rgb(v);
				if (output == ColorSpaceYUV) return lab_to_yuv(v);
				if (output == ColorSpaceLAB) return v;
			}
			return value_type::Zero();
		}

	protected:


		template<typename T>
		inline float64 f(T x, float64 Xref) {
			float64 xF = static_cast<float64>(x);
			if (xF/Xref > pow(6.0/29.0,3.0))
				return pow(xF/Xref,1.0/3.0);
			return ((1.0/3.0)*(29.0/6.0)*(29.0/6.0)*(xF/Xref) + 4.0/29.0);
		}

		inline value_type xyz_to_lab(const value_type & p) {
			value_type v;
			float64 Yn = 1.0;
			float64 Xn = 0.95046866;
			float64 Zn = 1.08882331;

			float64 L = 116 * f(p(1),Yn) - 16;
			float64 a = 500 * (f(p(0),Xn) - f(p(1),Yn));
			float64 b = 200 * (f(p(1),Yn) - f(p(2),Zn));

			v(0) = L;
			v(1) = a;
			v(2) = b;
			return v;
		}

		inline value_type lab_to_xyz(const value_type & p) {
			value_type v;
			float64 Yn = 1.0;
			float64 Xn = 0.95046866;
			float64 Zn = 1.08882331;

			float64 delta = 6.0 / 29.0;
			float64 X,Y,Z;

			float64 fy = (p(0) + 16) / 116;
			float64 fx = fy + p(1)/500;
			float64 fz = fy - p(2)/200;

			if (fx > delta) X = Xn * fx * fx * fx; else X = (fx - 16.0/116.0)*3*delta*delta*Xn;
			if (fy > delta) Y = Yn * fy * fy * fy; else Y = (fy - 16.0/116.0)*3*delta*delta*Yn;
			if (fz > delta) Z = Zn * fz * fz * fz; else Z = (fz - 16.0/116.0)*3*delta*delta*Zn;

			v(0) = X; v(1) = Y; v(2) = Z;

			return v;
		}

		inline value_type rgb_to_xyz(const value_type & p) {
			float64 lr = p(2)/255.0;
			float64 lg = p(1)/255.0;
			float64 lb = p(0)/255.0;

			if (lr < 0.03928) lr = lr / 12.92; else lr = pow((lr+0.055)/(1.055),2.4);
			if (lg < 0.03928) lg = lg / 12.92; else lg = pow((lg+0.055)/(1.055),2.4);
			if (lb < 0.03928) lb = lb / 12.92; else lb = pow((lb+0.055)/(1.055),2.4);

			float64 X = 0.412453 * lr + 0.357580 * lg + 0.180423 * lb;
			float64 Y = 0.212671 * lr + 0.715160 * lg + 0.072169 * lb;
			float64 Z = 0.019334 * lr + 0.119193 * lg + 0.950227 * lb;
			value_type v;
			v(0) = X;
			v(1) = Y;
			v(2) = Z;
			return v;
		}

		inline value_type xyz_to_rgb(const value_type & p) {

			float64 R = 3.240479 * p(0) - 1.537152 * p(1) - 0.498536 * p(2);
			float64 G =-0.969255 * p(0) + 1.875990 * p(1) + 0.041556 * p(2);
			float64 B = 0.055647 * p(0) - 0.204041 * p(1) + 1.057311 * p(2);

			if (R < 0.00304) R *= 12.92; else R = 1.055*pow(R,1/2.4) - 0.055;
			if (G < 0.00304) G *= 12.92; else G = 1.055*pow(G,1/2.4) - 0.055;
			if (B < 0.00304) B *= 12.92; else B = 1.055*pow(B,1/2.4) - 0.055;

			R *= 255; G*=255; B*=255;
			value_type v;
			v(0) = B;
			v(1) = G;
			v(2) = R;
			return v;
		}

		// Primary conversion functions

		inline value_type rgb_to_yuv(const value_type& v) {
			value_type p;
			p(0) = 0.5 + 16.0  + 1/256.0 * (   65.738  * (v(2)) +  129.057  * (v(1)) +  25.064  * (v(0)) );
			p(1) = 0.5 + 128.0 + 1/256.0 * ( - 37.945  * (v(2)) -   74.494  * (v(1)) + 112.439  * (v(0)) );
			p(2) = 0.5 + 128.0 + 1/256.0 * (  112.439  * (v(2)) -   94.154  * (v(1)) -  18.285  * (v(0)) );

			//Clip
			return p.cwiseMax(value_type(0,0,0)).cwiseMin(value_type(255,255,255));
		}

		inline value_type rgb_to_lab(const value_type& v) {
			return xyz_to_lab(rgb_to_xyz(v)) + value_type(0,128,128);
		}

		inline value_type yuv_to_rgb(const value_type& v) {
			value_type p;
			float64 y = v(0);
			float64 cb = v(1);
			float64 cr = v(2);
			p(2) = 0.5 + ( 298.082 * y                + 408.583 * cr ) / 256.0 - 222.921;
			p(1) = 0.5 + ( 298.082 * y - 100.291 * cb - 208.120 * cr ) / 256.0 + 135.576;
			p(0) = 0.5 + ( 298.082 * y + 516.412 * cb                ) / 256.0 - 276.836;

			//Clip
			return p.cwiseMax(value_type(0,0,0)).cwiseMin(value_type(255,255,255));
		}

		inline value_type yuv_to_lab(const value_type& v) {
			return rgb_to_lab(yuv_to_rgb(v));
		}

		inline value_type lab_to_rgb(const value_type& v) {
			value_type lab = v - value_type(0,128,128);
			return xyz_to_rgb(lab_to_xyz(lab));
		}

		inline value_type lab_to_yuv(const value_type& v) {
			return rgb_to_yuv(lab_to_rgb(v));
		}

	};

}


#endif /* COLORSPACE_CONVERTER_HPP_ */
