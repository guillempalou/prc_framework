/*
 * discrete_space_neighbors.hpp
 *
 *  Created on: Jul 25, 2012
 *      Author: guillem
 */

#ifndef DISCRETE_SPACE_NEIGHBORS_HPP_
#define DISCRETE_SPACE_NEIGHBORS_HPP_

#include <boost/array.hpp>

namespace imageplus {

	//! enum that states the connectivity (only one, two and three dimensions)
	typedef enum {
		//! typical 1D connectivity
		Connectivity1D2 = 1,
		//! 4 neighbor in 2D spaces (sharing edges)
		Connectivity2D4 = 2,
		//! 8 neighbor in 2D spaces (sharing edges,corners)
		Connectivity2D8 = 3,
		//! 6 neighbor in 3D spaces (sharing faces)
		Connectivity3D6 = 4,
		//! 8 neighbor in 3D spaces (sharing faces,edges)
		Connectivity3D18 = 5,
		//! 8 neighbor in 3D spaces (sharing faces,edges,corners)
		Connectivity3D26 = 6,

		//! forward 1 neighbor in 1D
		Connectivity1D1 = 7,
		//! forward 2 neighbors in 2D
		Connectivity2D2 = 8,
		//! forward 3 neighbors in 3D
		Connectivity3D3 = 9,
		//! same plane neighbors in 3D
		Connectivity3D4 = 10,
		//! forward same plane neighbors in 3D
		Connectivity3D2 = 11
	} ConnectivityType;

	//! neighborhood traits
	template<uint64 dimensions>
	struct neighborhood_traits {
	};

	//! default neighborhood traits for 1D
	template<>
	struct neighborhood_traits<1> {
		//! default connectivity
		static const ConnectivityType default_connectivity = Connectivity1D2;
		static const ConnectivityType default_forward_connectivity = Connectivity1D1;
	};

	//! default neighborhood traits for 2D
	template<>
	struct neighborhood_traits<2> {
		static const ConnectivityType default_connectivity = Connectivity2D4;
		static const ConnectivityType default_forward_connectivity = Connectivity2D2;
	};

	//! default neighborhood traits for 3D
	template<>
	struct neighborhood_traits<3> {
		//! default connectivity
		static const ConnectivityType default_connectivity = Connectivity3D6;
		static const ConnectivityType default_forward_connectivity = Connectivity3D3;
	};

	//! Class Neighborhood
	template<typename coords_type, uint64 dimensions, ConnectivityType connectivity>
	struct Neighborhood{
	};

	//! Class Neighborhood for 1D specialization
	template<typename coords_type>
	struct Neighborhood<coords_type,1,Connectivity1D2> {

		//! type
		typedef boost::array<Eigen::Matrix<coords_type,1,1>,2> NeighborhoodType;

		//! neighbors
		NeighborhoodType neighbors;

		//!default constructor
		Neighborhood() {
			neighbors[0](0) = -1;
			neighbors[1](0) = 1;
		}
	};

	//! Class Neighborhood for 2D specialization 4 neighbors
	template<typename coords_type>
	struct Neighborhood<coords_type,2,Connectivity2D4> {

		//! type
		typedef boost::array<Eigen::Matrix<coords_type,2,1>,4> NeighborhoodType;

		//! neighbors
		NeighborhoodType neighbors;

		//!default constructor
		Neighborhood() {
			neighbors[0](0) = -1; neighbors[0](1) = 0;
			neighbors[1](0) = 0;  neighbors[1](1) = -1;
			neighbors[2](0) = 1;  neighbors[2](1) = 0;
			neighbors[3](0) = 0;  neighbors[3](1) = 1;
		}
	};

	//! Class Neighborhood for 2D specialization 8 neighbors
	template<typename coords_type>
	struct Neighborhood<coords_type,2,Connectivity2D8> {

		//! type
		typedef boost::array<Eigen::Matrix<coords_type,2,1>,8> NeighborhoodType;

		//! neighbors
		NeighborhoodType neighbors;

		//!default constructor
		Neighborhood() {
			neighbors[0](0) = -1; neighbors[0](1) = 0;
			neighbors[1](0) = 0;  neighbors[1](1) = -1;
			neighbors[2](0) = 1;  neighbors[2](1) = 0;
			neighbors[3](0) = 0;  neighbors[3](1) = 1;
			neighbors[4](0) = -1; neighbors[4](1) = -1;
			neighbors[5](0) = 1;  neighbors[5](1) = -1;
			neighbors[6](0) = -1; neighbors[6](1) = 1;
			neighbors[7](0) = 1;  neighbors[7](1) = 1;
		}
	};

	//! Class Neighborhood for 3D specialization 6 neighbors
	template<typename coords_type>
	struct Neighborhood<coords_type,3,Connectivity3D6> {

		//! type
		typedef boost::array<Eigen::Matrix<coords_type,3,1>,6> NeighborhoodType;

		//! neighbors
		NeighborhoodType neighbors;

		//!default constructor
		Neighborhood() {
			neighbors[0](0) = -1; neighbors[0](1) = 0;  neighbors[0](2) = 0;
			neighbors[1](0) = 0;  neighbors[1](1) = -1; neighbors[1](2) = 0;
			neighbors[2](0) = 0;  neighbors[2](1) = 0;  neighbors[2](2) = -1;
			neighbors[3](0) = 1;  neighbors[3](1) = 0;  neighbors[3](2) = 0;
			neighbors[4](0) = 0;  neighbors[4](1) = 1;  neighbors[4](2) = 0;
			neighbors[5](0) = 0;  neighbors[5](1) = 0;  neighbors[5](2) = 1;
		}
	};

	//! Class Neighborhood for 3D specialization 18 neighbors
	template<typename coords_type>
	struct Neighborhood<coords_type,3,Connectivity3D18> {

		//! type
		typedef boost::array<Eigen::Matrix<coords_type,3,1>,18> NeighborhoodType;

		//! neighbors
		NeighborhoodType neighbors;

		//!default constructor
		Neighborhood() {
			neighbors[0](0) = -1; neighbors[0](1) = 0;  neighbors[0](2) = 0;
			neighbors[1](0) = 0;  neighbors[1](1) = -1; neighbors[1](2) = 0;
			neighbors[2](0) = 0;  neighbors[2](1) = 0;  neighbors[2](2) = -1;
			neighbors[3](0) = 1;  neighbors[3](1) = 0;  neighbors[3](2) = 0;
			neighbors[4](0) = 0;  neighbors[4](1) = 1;  neighbors[4](2) = 0;
			neighbors[5](0) = 0;  neighbors[5](1) = 0;  neighbors[5](2) = 1;

			neighbors[6](0) =  1;  neighbors[6](1) =  1; neighbors[6](2) = 0;
			neighbors[7](0) = -1;  neighbors[7](1) = 1;  neighbors[7](2) = 0;
			neighbors[8](0) = -1;  neighbors[8](1) = -1; neighbors[8](2) = 0;
			neighbors[9](0) = 1;   neighbors[9](1) = -1; neighbors[9](2) = 0;

			neighbors[10](0) =  1;  neighbors[10](1) =  0; neighbors[10](2) = 1;
			neighbors[11](0) = -1;  neighbors[11](1) = 0;  neighbors[11](2) = 1;
			neighbors[12](0) = -1;  neighbors[12](1) = 0;  neighbors[12](2) = -1;
			neighbors[13](0) = 1;   neighbors[13](1) = 0;  neighbors[13](2) = -1;

			neighbors[14](0) = 0;  neighbors[14](1) =  1;  neighbors[14](2) = 1;
			neighbors[15](0) = 0;  neighbors[15](1) = -1;  neighbors[15](2) = 1;
			neighbors[16](0) = 0;  neighbors[16](1) = -1;  neighbors[16](2) = -1;
			neighbors[17](0) = 0;   neighbors[17](1) = 1;  neighbors[17](2) = -1;
		}
	};

	//! Class Neighborhood for 3D specialization 26 neighbors
	template<typename coords_type>
	struct Neighborhood<coords_type,3,Connectivity3D26> {

		//! type
		typedef boost::array<Eigen::Matrix<coords_type,3,1>,26> NeighborhoodType;

		//! neighbors
		NeighborhoodType neighbors;

		//!default constructor
		Neighborhood() {
			neighbors[0](0) = -1; neighbors[0](1) = 0;  neighbors[0](2) = 0;
			neighbors[1](0) = 0;  neighbors[1](1) = -1; neighbors[1](2) = 0;
			neighbors[2](0) = 0;  neighbors[2](1) = 0;  neighbors[2](2) = -1;
			neighbors[3](0) = 1;  neighbors[3](1) = 0;  neighbors[3](2) = 0;
			neighbors[4](0) = 0;  neighbors[4](1) = 1;  neighbors[4](2) = 0;
			neighbors[5](0) = 0;  neighbors[5](1) = 0;  neighbors[5](2) = 1;

			neighbors[6](0) =  1;  neighbors[6](1) =  1; neighbors[6](2) = 0;
			neighbors[7](0) = -1;  neighbors[7](1) = 1;  neighbors[7](2) = 0;
			neighbors[8](0) = -1;  neighbors[8](1) = -1; neighbors[8](2) = 0;
			neighbors[9](0) = 1;   neighbors[9](1) = -1; neighbors[9](2) = 0;

			neighbors[10](0) =  1;  neighbors[10](1) =  0; neighbors[10](2) = 1;
			neighbors[11](0) = -1;  neighbors[11](1) = 0;  neighbors[11](2) = 1;
			neighbors[12](0) = -1;  neighbors[12](1) = 0;  neighbors[12](2) = -1;
			neighbors[13](0) = 1;   neighbors[13](1) = 0;  neighbors[13](2) = -1;

			neighbors[14](0) = 0;  neighbors[14](1) =  1;  neighbors[14](2) = 1;
			neighbors[15](0) = 0;  neighbors[15](1) = -1;  neighbors[15](2) = 1;
			neighbors[16](0) = 0;  neighbors[16](1) = -1;  neighbors[16](2) = -1;
			neighbors[17](0) = 0;   neighbors[17](1) = 1;  neighbors[17](2) = -1;

			neighbors[18](0) =  1;  neighbors[18](1) =  1;  neighbors[18](2) = 1;
			neighbors[19](0) = -1;  neighbors[19](1) = 1;   neighbors[19](2) = 1;
			neighbors[20](0) = 1;   neighbors[20](1) = -1;  neighbors[20](2) = -1;
			neighbors[21](0) = -1;  neighbors[21](1) = -1;  neighbors[21](2) = -1;

			neighbors[22](0) =  1;  neighbors[22](1) =  1;  neighbors[22](2) = -1;
			neighbors[23](0) = -1;  neighbors[23](1) = 1;   neighbors[23](2) = -1;
			neighbors[24](0) = 1;   neighbors[24](1) = -1;  neighbors[24](2) = -1;
			neighbors[25](0) = -1;  neighbors[25](1) = -1;  neighbors[25](2) = -1;
		}


	};

	//! Class Neighborhood for 1D specialization 1 neighbor
	template<typename coords_type>
	struct Neighborhood<coords_type,1,Connectivity1D1> {

		//! type
		typedef boost::array<Eigen::Matrix<coords_type,1,1>,1> NeighborhoodType;

		//! neighbors
		NeighborhoodType neighbors;

		//!default constructor
		Neighborhood() {
			neighbors[0](0) = 1;
		}
	};

	//! Class Neighborhood for 2D specialization 2 neighbors
	template<typename coords_type>
	struct Neighborhood<coords_type,2,Connectivity2D2> {

		//! type
		typedef boost::array<Eigen::Matrix<coords_type,2,1>,2> NeighborhoodType;

		//! neighbors
		NeighborhoodType neighbors;

		//!default constructor
		Neighborhood() {
			neighbors[0](0) = 1;  neighbors[0](1) = 0;
			neighbors[1](0) = 0;  neighbors[1](1) = 1;
		}
	};

	//! Class Neighborhood for 3D specialization 3 neighbors
	template<typename coords_type>
	struct Neighborhood<coords_type,3,Connectivity3D3> {

		//! type
		typedef boost::array<Eigen::Matrix<coords_type,3,1>,3> NeighborhoodType;

		//! neighbors
		NeighborhoodType neighbors;

		//!default constructor
		Neighborhood() {
			neighbors[0](0) = 1;  neighbors[0](1) = 0;  neighbors[0](2) = 0;
			neighbors[1](0) = 0;  neighbors[1](1) = 1;  neighbors[1](2) = 0;
			neighbors[2](0) = 0;  neighbors[2](1) = 0;  neighbors[2](2) = 1;
		}
	};

	//! Class Neighborhood for 3D specialization with 4 neighbors on the same plane
	//! Class Neighborhood for 3D specialization 3 neighbors
	template<typename coords_type>
	struct Neighborhood<coords_type,3,Connectivity3D4> {

		//! type
		typedef boost::array<Eigen::Matrix<coords_type,3,1>,4> NeighborhoodType;

		//! neighbors
		NeighborhoodType neighbors;

		//!default constructor
		Neighborhood() {
			neighbors[0](0) = 1;  neighbors[0](1) = 0;  neighbors[0](2) = 0;
			neighbors[1](0) = 0;  neighbors[1](1) = 1;  neighbors[1](2) = 0;
			neighbors[2](0) = -1;  neighbors[2](1) = 0;  neighbors[2](2) = 0;
			neighbors[2](0) = 0;  neighbors[2](1) = -1;  neighbors[2](2) = 0;
		}
	};

	//! Class Neighborhood for 3D specialization with 4 neighbors on the same plane, forward
	template<typename coords_type>
	struct Neighborhood<coords_type,3,Connectivity3D2> {

		//! type
		typedef boost::array<Eigen::Matrix<coords_type,3,1>,2> NeighborhoodType;

		//! neighbors
		NeighborhoodType neighbors;

		//!default constructor
		Neighborhood() {
			neighbors[0](0) = 1;  neighbors[0](1) = 0;  neighbors[0](2) = 0;
			neighbors[1](0) = 0;  neighbors[1](1) = 1;  neighbors[1](2) = 0;
		}
	};

}

#endif /* DISCRETE_SPACE_NEIGHBORS_HPP_ */
