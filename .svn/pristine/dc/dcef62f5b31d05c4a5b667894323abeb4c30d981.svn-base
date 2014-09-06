/*
 * numeric_types.hpp
 *
 *  Created on: Dec 2, 2011
 *      Author: guillem
 */

#ifndef MATH_TYPES_HPP_
#define MATH_TYPES_HPP_

// can be defined in user.build
#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET

#include <imageplus/core/imageplus_types.hpp>
#include <Eigen/Eigen>
#include <Eigen/Sparse>

namespace imageplus {
	namespace math 		{

			//! Typedefs mapping Eigen matrices/vectors/arrays

			//! Basic Matrix type
			typedef Eigen::Matrix< float64, Eigen::Dynamic, 1>				Vector;

			//! Basic Vector 2D type
			typedef Eigen::Vector2f											Vector2D;

			//! Basic Vector 3D type
			typedef Eigen::Vector3f											Vector3D;

			//! Basic Vector type
			typedef Eigen::Matrix< float64, Eigen::Dynamic, Eigen::Dynamic>	Matrix;

			//! Basic Array type
			typedef Eigen::Array< float64, Eigen::Dynamic, Eigen::Dynamic>	Array;

			//! Variable type to work with functions
			typedef Eigen::Matrix< float64, Eigen::Dynamic, 1> 				VariableModel;

			//! Gradient type to work with functions
			typedef Eigen::Matrix< float64, Eigen::Dynamic, 1>				GradientModel;

			//! Hessian matrix type to work with functions
			typedef Eigen::Matrix< float64, Eigen::Dynamic, Eigen::Dynamic> HessianModel;

			//! You should define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET in Eigen headers or in user.build
			//! Sparse matrix type
			typedef Eigen::SparseMatrix<float64, Eigen::RowMajor>	SparseMatrix;

			//! Sparse vector type
			typedef Eigen::SparseVector<float64>					SparseVector;
	}
}



#endif /* NUMERIC_TYPES_HPP_ */
