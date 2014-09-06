/*
 * hierarchical_space_partition.hpp
 *
 *  Created on: Jul 31, 2012
 *      Author: guillem
 */

#ifndef HIERARCHICAL_REGION_PARTITION_HPP_
#define HIERARCHICAL_REGION_PARTITION_HPP_

#include <vector>
#include <set>
#include <list>
#include <iterator>
#include <fstream>

#include <imageplus/core/exceptions.hpp>
#include <imageplus/core/config.hpp>

#include <imageplus/core/regions/hierarchical_region.hpp>
#include <imageplus/segmentation/partition/partition.hpp>

#include <imageplus/segmentation/io/partition2d_write.hpp>
#include <imageplus/segmentation/io/partition2d_read.hpp>

#include <imageplus/segmentation/visualization/false_color.hpp>

namespace imageplus
{
	namespace segmentation {

    //!
    //! \brief Class to handle region-oriented partitions.
    //!
    //! Class to handle region-oriented partitions, in contrast to ImagePartition, which is pixel-oriented.
    //! Briefly, Partition is a set of regions, and therefore it is more suitable for computing region descriptors
    //!  or performing any algorithm that is mainly based on scanning all the pixels of a region.
    //!
    //! This class is usually constructed from a image_concept and the pixel-based partition container related. Typically, an Image and an ImagePartition.
    //!
    //! Multiresolution partitions (BPTs) are also accepted, in the sense that there may exist two regions along with their merging (father region).
    //! In this case, the constructor also receives a Merging Sequence.
    //!
    //! \todo Implement a pixel access
    //!
    //! \author Jordi Pont Tuset <jpont@gps.tsc.upc.edu>, Guillem Palou
    //!
    //! \date 05-05-2009
    template<class RegionModel, ConnectivityType adjacency_type = neighborhood_traits<RegionModel::dimensions>::default_forward_connectivity>
    class HierarchicalRegionPartition {
    public:

        typedef RegionModel 																	RegionType;							//!< Type of regions we are working with
        typedef typename RegionType::identifier_type											identifier_type;  					//!< Type of the region identifiers
        typedef Partition<identifier_type, RegionType::dimensions>								PartitionType;		       			//!< Type of partition pixel-oriented (usually ImagePartition)
        typedef typename RegionType::coord_type 												coord_type;      					//!< Type of coords we are working with

    public:

        //!
        //! \brief Default constructor
        //!
        HierarchicalRegionPartition()
        {
        	_num_mergings = 0;
        }

        //!
        //! \brief Copy constructor
        //! \param copy: Partition to be copied
        //!
        HierarchicalRegionPartition(const HierarchicalRegionPartition& copy)
        {
        	_roots_partition  = copy.roots_partition();
        	_leaves_partition = copy.leaves_partition();
        	_curr_max_label = copy.max_label();
        	_leaves_partition.coll_vd().clear();
        	_num_mergings = 0;
        	_clear_regions();
        	_copy_regions(copy);
        }

        //! Destructor
        ~HierarchicalRegionPartition()
        {
        	_clear_regions();
        }

        //! Inits the set of regions scanning _leaves_partition
        //! This function must be implemented
        void init(PartitionType& initial_partition)
        {
        	_leaves_partition = initial_partition;
        	_roots_partition = initial_partition;
        	_num_mergings = 0;
        	_curr_max_label = 0;
        	_clear_regions();

        	// Scan all regions
        	typename PartitionType::iterator part_end = _leaves_partition.end();
        	typename PartitionType::iterator part_it = _leaves_partition.begin();

        	// Perform a relabeling
        	uint64 current_label = 0;

        	for(; part_it != part_end; ++part_it) {
        		uint64 label = (*part_it)(0);
        		if (_correspondences.find(label) == _correspondences.end()) {
        			_correspondences[label] = current_label++;
        			//std::cout << "putting correspondences " << label << " " << current_label-1 << std::endl;
        		}
        		// relabel
        		(*part_it)(0) = _correspondences[label];
        	}
        	//std::cout << current_label << std::endl;
        	_curr_max_label = current_label-1;

        	//reserve space
        	set_max_number_of_regions(2*_curr_max_label + 1);

        	for (uint64 i = 0; i < current_label; i++) {
        		RegionType* reg = new RegionType(i);
        		_regions[i] = reg;
        	}

        	part_it = _leaves_partition.begin();
        	for(; part_it != part_end; ++part_it) {
        		uint64 label = (*part_it)(0);

        		RegionType& curr_region = *_regions[label];
        		curr_region.add_coordinate(part_it.pos());
        		//std::cout << curr_region.size();
        	}

        	// Include neighbor information
        	part_it = _leaves_partition.begin();

        	typedef typename PartitionType::template general_adjacency_iterator<adjacency_type>::type  adj_iterator;

        	for(; part_it != part_end; ++part_it) {
        		adj_iterator adj		 	= _leaves_partition.template general_adjacency_begin<adjacency_type>(part_it.pos());
        		adj_iterator adj_end 		= _leaves_partition.template general_adjacency_end<adjacency_type>(part_it.pos());
        		uint64 label = (*part_it)(0);
        		for (; adj!=adj_end;++adj) {
        			uint64 label_adj = (*adj)(0);
        			if (label == label_adj) continue;
        			//std::cout << "adding relation " << label << " " << label_adj << std::endl;
        			_regions[label]->add_neighbor(_regions[label_adj]);
        		}
        	}

        }

        inline uint64 correspondence(uint64 init_partition_label) {
        	return _correspondences[init_partition_label];
        }

        //! Gets the maximum label
        //! \return the maximum label now
        identifier_type max_label() {
        	return _curr_max_label;
        }

        void set_update_partition(bool b) {
        	_update_partition = b;
        }

        void set_max_number_of_regions(uint64 num) {
        	_regions.resize(num);
        }


    // merging regions
    public:

        //! TODO
        //! Adds a new region, formed by the merging of two of the existing ones
        //!
        //! \param[in] regions : regions to be merged
        //! \param[in] father : Identifier of the new region to be created
        //RegionType& merge_regions(std::vector<identifier_type> regions, identifier_type father)
        //{
        //	return merge_regions(regions,father);
        //}

        //! TODO
        //! Adds a new region, formed by the merging of two of the existing ones
        //!
        //! \param[in] regions : regions to be merged
        //! \param[in] father : Identifier of the new region to be created
        //RegionType& merge_regions(std::vector<RegionType*> regions, identifier_type father)
        //{
        //	return merge_regions(regions,father);
        //}

        //! Adds a new region, formed by the merging of two of the existing ones
        //!
        //! \param[in] region1 : One of the regions to be merged
        //! \param[in] region2 : The other region to be merged
        //! \param[in] father : Identifier of the new region to be created
        RegionType& merge_regions(RegionType& region1, RegionType& region2, identifier_type father_label)
        {
        	/*
        	 * We can merge non-neighbor regions.
        	 */
        	if (father_label != _regions.size()-1) {
        		ImagePlusError("Index of region out of bounds");
        	}


        	_curr_max_label = std::max(_curr_max_label, father_label);

        	RegionType* father_pointer = new RegionType(father_label,region1,region2);

        	//std::cout << "merging " << father_label << " " << _curr_max_label << " " << _regions.size() << std::endl;

        	_regions[father_label] = father_pointer;

        	RegionType& parent = *_regions[father_label];

        	region1.parent(&parent);
        	region2.parent(&parent);

        	//std::cout << "father label " << region1.label() << " " << region2.label() << " " << parent.label() << std::endl;

        	//clear the neighbors and update

        	typename RegionType::neighbor_iterator n 	= region1.neighbors_begin();
        	typename RegionType::neighbor_iterator end 	= region1.neighbors_end();

        	for (; n != end; ++n) {
        		//std::cout << "From " << region1.label() << " adding neighbor " << (**n).label() << std::endl;
        		if (*n != &region2) {
        			parent.add_neighbor(*n);
        			//std::cout << "Added neighbor " << (**n).label() << std::endl;
        		}
        	}

        	n 		= region2.neighbors_begin();
        	end 	= region2.neighbors_end();
        	for (; n != end; ++n) {
        		//std::cout << "from " << region2.label() << " adding neighbor " << (**n).label() << std::endl;
        		if (*n != &region1) {
        			if (!parent.is_neighbor(*n)) { parent.add_neighbor(*n);} // std::cout << "Add Region " << parent.label() << " " << (*n)->label() << std::endl; }
        		}
        	}
        	//exit(0);
        	region1.clear_neighbors();
        	region2.clear_neighbors();

        	_num_mergings++;

        	// Generate the partition for the new created region
        	if (_update_partition) _update_roots_partition(parent);

        	return parent;
        }

        //! Adds a new region, formed by the merging of two of the exisitng ones
        //!
        //! \param[in] label1 : Label of one of the regions to be merged
        //! \param[in] label2 : Label of the other region to be merged
        //! \param[in] father : Identifier of the new region to be created
        RegionType& merge_regions(identifier_type label1, identifier_type label2, identifier_type father)
        {
        	ASSERT(_regions[label1] >= _regions.size(), "merge_regions: the label of the father was already in the partition.");
        	ASSERT(_regions[label2] >= _regions.size(), "merge_regions: the label of the father was already in the partition.");
        	ASSERT(_regions[label1].parent()==0, "merge_regions: trying to merge a region that already has a father.");
        	ASSERT(_regions[label2].parent()==0, "merge_regions: trying to merge a region that already has a father.");

        	return merge_regions(*_regions[label1], *_regions[label2], father);
        }


        //!
        //! \brief Prunes a branch of the tree, i.e., it deletes all the subregions of the region with given label
        //!
        //! \param[in] region: Region we want to prune
        void prune(RegionType& region)
        {
        	if(region.children().size()>0)
        	{
        		// Get the coordinates of the leaves and update the leaves partition
        		if (region.coordinates().size()== 0) {
        			for (typename RegionType::iterator it = region.begin(); it != region.end(); ++it) {
        				region.add_coordinate(*it);
        				//std::cout << "Filling " << (*it).transpose() << std::endl;
        				_leaves_partition(*it)(0) = region.label();
        			}
        		}

        		uint64 head = 0;
        		uint64 tail = 0;
        		std::deque<identifier_type> to_look;

        		for(std::size_t ii=0; ii<region.children().size(); ii++) {
        			to_look.push_back(region.child(ii)->label());
        			head++;
        		}
        		region.clear_children();

        		while(head != tail)
        		{
        			identifier_type curr = to_look[tail];
        			RegionType& r = *_regions[curr];

        			if(r.children().size()!=0)
        			{
        				for(std::size_t ii=0; ii< r.children().size(); ii++) {
        					to_look.push_back(r.child(ii)->label());
        					head++;
        				}
        			}

        			tail++;
        		}

        		// Erase all the regions (only if a map)
        		for (uint64 i = 0; i < to_look.size(); i++) {
        			RegionType* r = _regions[to_look[i]];
        			delete r;
        			//std::cout << "Pruning " << to_look[i] << std::endl;
        			_regions[to_look[i]] = (RegionType*)NULL;
        		}
        	}
        }

        //!
        //! \brief Prunes a branch of the tree, i.e., it deletes all the subregions of the region with given label
        //!
        //! \param[in] label: Label of the region we want to prune
        void prune(identifier_type label)
        {
        	prune(*_regions[label]);
        }

        //! Must overload this operator to retrieve a region
        //! \param[in] label: Label of the region we want to obtain
        //! \return Reference to the region with a given label
        inline RegionType& operator()(const identifier_type& label) {
        	return _region_by_label(label);
        }

        inline PartitionType& leaves_partition() {
        	return _leaves_partition;
        }

        inline PartitionType& roots_partition() {
        	return _roots_partition;
        }

    public:

        // Input/Output functions

        void save_to_files(std::string partition_path, std::string mergings_path) {
        	std::ofstream fout(partition_path.c_str(), std::fstream::binary);
        	std::ofstream fm(mergings_path.c_str(), std::fstream::binary);

        	// Save the leaves partition
        	typename PartitionType::value_data_type* data = _leaves_partition.data();

        	coord_type sizes = _leaves_partition.sizes();
        	uint64 l = sizes.prod();
        	uint64 dimensions= _leaves_partition.coord_dimensions;

        	fout.write((const char*) &dimensions, sizeof(uint64));
        	for (uint64 i = 0; i < dimensions; i++) fout.write((const char*) &sizes(i), sizeof(uint64));
        	fout.write((const char*) data, sizeof(typename PartitionType::value_data_type) * l);

        	uint64 *a  = new uint64[_num_mergings*3];
        	fm.write((const char*)&_num_mergings, sizeof(uint64));
        	uint64 i = 0;

        	//Save the merging sequence
        	non_leaves_iterator it = begin();
        	non_leaves_iterator it_end = end();
        	for(; it!=it_end; ++it) {
        		a[i++] = (*it).child(0)->label();
        		a[i++] = (*it).child(1)->label();
        		a[i++] = (*it).label();
        		//fm << (*it).child(0)->label() << "\t" << (*it).child(1)->label() << "\t" << (*it).label() << std::endl;
        	}
        	fm.write((const char*)a, sizeof(uint64)*_num_mergings*3);

        	delete a;
        	fout.close();
        	fm.close();
        }

        //! Created to offer compatibility with older PRL files
        void save_prl(std::string image_path, std::string partition_path, std::string merging_sequence_path) {

        	//save the prl file
        	PRLWriter<PartitionType> prl_writer;
        	prl_writer.write(_leaves_partition, partition_path);

        	//Save the merging sequence
        	std::ofstream fm(merging_sequence_path.c_str());
        	fm << partition_path << std::endl;
        	fm << image_path << std::endl;

        	non_leaves_iterator it = begin();
        	non_leaves_iterator it_end = end();
        	for(; it!=it_end; ++it) {
        		fm << (*it).child(0)->label() << "\t" << (*it).child(1)->label() << "\t" << (*it).label() << std::endl;
        	}
        }

        //! Loads a Hierarchical partition from a leaves partition file and a mergings file path
        //! \param[in] partition_path: leave partition path (contains correspondences and labels)
        //! \param[in]
        void load_from_files(std::string partition_path, std::string mergings_path) {

        	// Read the leaves partition
        	std::ifstream fin(partition_path.c_str(), std::fstream::binary);
        	std::ifstream fm(mergings_path.c_str(), std::fstream::binary);

        	if (!fin.is_open())
        		throw;

        	if (!fm.is_open())
        		throw;

        	uint64 dimensions;
        	coord_type sizes;

        	fin.read((char*)&dimensions, sizeof(uint64));
        	for (uint64 i = 0; i < dimensions; i++) fin.read((char*) &sizes(i), sizeof(uint64));
        	uint64 l = sizes.prod();

        	PartitionType p(sizes);
        	typename PartitionType::value_data_type* data = p.data();

        	fin.read((char*) data, sizeof(typename PartitionType::value_data_type) * l);

        	init(p);

        	// Create the leaves of the  tree, read the merging sequence and create the hierarchy
        	_update_partition = false;

        	fm.read((char*)&_num_mergings,sizeof(uint64));

        	uint64 *a;
        	a = new uint64[_num_mergings*3];

        	fm.read((char*)a, sizeof(uint64)*_num_mergings*3);

        	uint64 N = _num_mergings;
        	for (uint64 i = 0, k = 0; i < N; i++)
        	{
        		//fm >> son1 >> son2 >> father;
        		//if (fm.eof()) break;
        		uint64 son1 = a[k++];
        		uint64 son2 = a[k++];
        		uint64 father = a[k++];
        		merge_regions(son1,son2,father);
        	}

        	roots_iterator r = begin();
        	roots_iterator r_end = end();

        	for (;r!=r_end;++r){
        		_update_roots_partition(*r);
        	}

        	delete a;
        	fm.close();
        }

        void load_from_prl(std::string path) {

        	PRLReader<PartitionType> prl_reader;

        	PartitionType p = prl_reader.read(path);

        	ImageSignal<float64,3> a = segmentation::to_false_color<ImageSignal<float64,3> >(p);
        	a.write("test.png");

        	init(p);

        	std::vector<std::vector<uint64> > mergings = prl_reader.mergings();

        	// Create the leaves of the  tree, read the merging sequence and create the hierarchy
        	_update_partition = false;
        	_num_mergings = mergings.size();

        	uint64 N = _num_mergings;
        	for (uint64 i = 0; i < N; i++)
        	{
        		//fm >> son1 >> son2 >> father;
        		//if (fm.eof()) break;
        		uint64 son1 = mergings[i][0];
        		uint64 son2 = mergings[i][1];
        		uint64 father = mergings[i][2];
        		//std::cout << _correspondences[son1] << " " << _correspondences[son2]<< " " << father << std::endl;
        		_correspondences[father] = father-1;
        		merge_regions(_correspondences[son1],_correspondences[son2],father-1);
        	}

        	roots_iterator r = begin();
        	roots_iterator r_end = end();

        	for (;r!=r_end;++r){
        		_update_roots_partition(*r);
        	}
        }

    protected:

        //!
        //! \brief Returns a reference to the region with a given label
        //!
        //! \param[in] label: Label of the region we want to obtain
        //! \return Reference to the region with a given label
        inline RegionType& _region_by_label(const identifier_type& label ) throw (ImagePlusError)
        {
        	if (label >= _regions.size())
        	{
        		std::cout << "Attempting " << label << " " << _regions.size() << std::endl;
        		throw ImagePlusError("Partition: region index not found.");
        	}
        	return *_regions[label];
        }

        //!
        //! \brief Relabels the roots_partition
        //! \param[in] reg : Father region to update
        //! \return Const reference to the base partition of the tree
        void _update_roots_partition(RegionType& reg)
        {
        	typename RegionType::iterator c;
        	for (c = reg.begin(); c != reg.end(); ++c)
        	{
        		// Iterate along all the pixels and relabel
        		_roots_partition(*c)(0) = reg.label();
        	}
        }

        //! Clear and deletes all regions
        void _clear_regions()
        {
        	for (uint64 i = 0; i < _regions.size(); i++)
        		delete _regions[i];
        	_regions.clear();
        }

        //! Copies all regions
        //! \param copy: Partition to be copied
        void _copy_regions(const HierarchicalRegionPartition& copy)
        {
        	const_global_iterator glob_it = copy.begin();
        	const_global_iterator glob_it_end = copy.end();

        	// Copy the regions
        	for(; glob_it!=glob_it_end; ++glob_it)
        	{
        		const RegionType& curr_reg = *(glob_it);
        		_regions.push_back(&curr_reg);
        	}
        }

    protected:

        //! Type of map
        typedef typename std::vector<RegionModel*> map_type;

        //! Max label of regions
        identifier_type _curr_max_label;

        //! number of mergins
        uint64			_num_mergings;

        //! Map of regions
        map_type _regions;

        //! Copy of the pixel-based partition to implement the pixel access efficiently (todo)
        PartitionType _leaves_partition;
        //! Partition that contains the roots of the three
        PartitionType _roots_partition;

        //! if true updates the roots partition when there is a merging
        bool _update_partition;

        // index correspondences (must begin with 1)
        std::map<identifier_type, identifier_type> _correspondences;

    public:

        //! Class that iterates through the regions of the tree that fulfil a condition given by the functor "condition"
        //!
        //! \todo document and explain how to create new ones
        //!
        template<class condition, class iterator_type=typename map_type::iterator, class region_type=RegionType>
        class conditional_iterator : public std::iterator<std::input_iterator_tag, RegionType>
        {
        public:
        	//! Type of region
        	typedef region_type RegionType;
        protected:
        	//! Iterator pointing to the current region
        	iterator_type _map_it;

        	//! Iterator pointing to the end of the region map
        	iterator_type _map_end;

        	//! Functor that defines through which regions we will iterate
        	condition _condition;

        public:
        	//! Default constructor
        	conditional_iterator() : _map_it(), _map_end(), _condition() {}

        	//! Constructor receiving an iterator to a region, to the end, and a condition functor.
        	//! It iterates untill it finds a region fulfilling the condition
        	//!
        	//! \param[in] map_it: Iterator to the current region
        	//! \param[in] map_it_end: Iterator to the end of the region map
        	//! \param[in] cond :  Functor defining the condition to iterate (by default uses the default constructor)
        	conditional_iterator(iterator_type map_it, iterator_type map_it_end, condition cond = condition()) : _map_it(map_it), _map_end(map_it_end), _condition(cond)
        	{
        		while(_map_it!=_map_end)
        		{
        			if (*_map_it==NULL) { ++_map_it; continue; }//check if there is a valid pointer
        			if(_condition((**_map_it)))
        				break;
        			else
        				++_map_it;
        		}
        	}

        	//! Copy contructor
        	//!
        	//! \param[in] copy: Iterator to be copied
        	template<class iterator_model>
        	conditional_iterator(const iterator_model& copy)
        	{
        		_map_it = copy.map_it();
        		_map_end = copy.map_end();

        		while(_map_it!=_map_end)
        		{
        			if (*_map_it==NULL) { ++_map_it; continue; }//check if there is a valid pointer
        			if(_condition((**_map_it)))
        				break;
        			else
        				++_map_it;
        		}
        	}


        	//! map_it const accessor (for the copy constructor between different types of iterators)
        	//! \return Const reference to map_it
        	const iterator_type& map_it() const
        	{
        		return _map_it;
        	}

        	//! map_end const accessor (for the copy constructor between different types of iterators)
        	//! \return Const reference to map_end
        	const iterator_type& map_end() const
        	{
        		return _map_end;
        	}

        	//! Operator ++ to move through regions. It looks for the next region fulfilling condition
        	//! \return The iterator pointing to the new position
        	conditional_iterator& operator++()
    	            						{
        		++_map_it;

        		while(_map_it!=_map_end)
        		{
        			if (*_map_it==NULL) { ++_map_it; continue; }//check if there is a valid pointer
        			if(_condition((**_map_it)))
        				break;
        			else
        				++_map_it;
        		}

        		return *this;
    	            						}


        	//! Operator ++ to move through regions. It looks for the next region fulfilling condition
        	//! \return The iterator pointing to the new position
        	conditional_iterator& operator--()
    	            						{
        		--_map_it;

        		while(_map_it!=_map_end)
        		{
        			if (*_map_it==NULL) { ++_map_it; continue; }//check if there is a valid pointer
        			if(_condition((**_map_it)))
        				break;
        			else
        				--_map_it;
        		}

        		return *this;
    	            						}


        	/*!
        	 * \param[in] it : Iterator to be compared with
        	 *
        	 * \returns true if the iterators are equal
        	 */
        	template<class iterator_model>
        	bool operator==(const iterator_model& it)
        	{
        		return _map_it==it.map_it();
        	}

        	/*!
        	 * \param[in] it : Iterator to be compared with
        	 *
        	 * \returns true if the iterators are different
        	 */
        	template<class iterator_model>
        	bool operator!=(const iterator_model& it)
        	{
        		return _map_it!=it.map_it();
        	}

        	//! Dereference operator
        	//! \return The region that it is pointing at
        	region_type& operator*() const
        	{
        		return *(*_map_it); // dereference the pointer
        	}

        	//! Copy operator
        	//!
        	//! \param[in] other : The iterator to copy
        	//! \return Reference to this, to be able to do a=b=c
        	template<class iterator_model>
        	conditional_iterator& operator=(const iterator_model& other)
        	{
        		_map_it = other.map_it();
        		_map_end = other.map_end();

        		while(_map_it!=_map_end)
        		{
        			if (*_map_it==NULL) { ++_map_it; continue; }//check if there is a valid pointer
        			if(_condition((**_map_it)))
        				break;
        			else
        				++_map_it;
        		}

        		return *this;
        	}

        };

        //! Condition functor to iterate through all regions
        struct global
        {
        	//! Returns true, to iterate through all regions
        	//! \param[in] curr_region : region to be checked
        	//! \return True
        	bool operator()(RegionType& curr_region) const
        	{
        		return true;
        	}
        };

        //! Condition functor to iterate through leaf regions
        struct leaves
        {
        	//! Checks whether the region is a leaf, i.e., if it does not have parts
        	//! \param[in] curr_region : region to be checked
        	//! \return Whether the region is a leaf
        	bool operator()(RegionType& curr_region) const
        	{
        		return curr_region.children().size()==0;
        	}
        };

        //! Condition functor to iterate through non-leaf regions
        struct non_leaves
        {
        	//! Checks whether the region is not a leaf, i.e., if it does have parts
        	//! \param[in] curr_region : region to be checked
        	//! \return Whether the region is not a leaf
        	bool operator()(RegionType& curr_region) const
        	{
        		return curr_region.children().size()>0;
        	}
        };

        //! Condition functor to iterate through root regions
        struct roots
        {
        	//! Checks whether the region is a root, i.e., if it does not have father
        	//! \param[in] curr_region : region to be checked
        	//! \return Whether the region is a root
        	bool operator()(RegionType& curr_region) const
        	{
        		return curr_region.parent()==(RegionType*)NULL;
        	}
        };

        //! Condition functor to iterate through non-root regions
        struct non_roots
        {
        	//! Checks whether the region is not a root, i.e., if it does have father
        	//! \param[in] curr_region : region to be checked
        	//! \return Whether the region is not a root
        	bool operator()(RegionType& curr_region) const
        	{
        		return curr_region.parent()!=0;
        	}
        };

        typedef conditional_iterator<global>         global_iterator;   //!< Type to refer to iterators through all regions
        typedef conditional_iterator<leaves>         leaves_iterator;   //!< Type to refer to iterators through leaf regions
        typedef conditional_iterator<non_leaves> non_leaves_iterator;   //!< Type to refer to iterators through non-leaf regions
        typedef conditional_iterator<roots>           roots_iterator;   //!< Type to refer to iterators through root regions
        typedef conditional_iterator<non_roots>   non_roots_iterator;   //!< Type to refer to iterators through non-root regions

        typedef conditional_iterator<global, typename map_type::const_iterator, const RegionType>         const_global_iterator; //!< Type to refer to const iterators through all regions
        typedef conditional_iterator<leaves, typename map_type::const_iterator, const RegionType>         const_leaves_iterator; //!< Type to refer to const iterators through leaf regions
        typedef conditional_iterator<non_leaves, typename map_type::const_iterator, const RegionType> const_non_leaves_iterator; //!< Type to refer to const iterators through non-leaf regions
        typedef conditional_iterator<roots, typename map_type::const_iterator, const RegionType>           const_roots_iterator; //!< Type to refer to const iterators through root regions
        typedef conditional_iterator<non_roots, typename map_type::const_iterator, const RegionType>   const_non_roots_iterator; //!< Type to refer to const iterators through non-root regions

        //! \returns an iterator to the first region of the Partition.
        //! Note that it returns a global_iterator, but in the assignment to another type of iterator, the type is changed
        global_iterator begin()     { return global_iterator(_regions.begin(), _regions.end()); }

        //! \returns an iterator to the end of the Partition.
        //! Note that it returns a global_iterator, but in the assignment to another type of iterator, the type is changed
        global_iterator end()       { return global_iterator(_regions.end(), _regions.end()); }

        //! \returns an iterator to region with a given label.
        //! Note that it returns a global_iterator, but in the assignment to another type of iterator, the type is changed
        //! \param id : Identifier of the region we are looking for
        global_iterator find(identifier_type id)    { return global_iterator(_regions.find(id), _regions.end()); }



        //! \returns a const iterator to the first region of the Partition.
        //! Note that it returns a global_iterator, but in the assignment to another type of iterator, the type is changed
        const_global_iterator begin() const    { return const_global_iterator(_regions.begin(), _regions.end()); }

        //! \returns a const iterator to the end of the Partition.
        //! Note that it returns a global_iterator, but in the assignment to another type of iterator, the type is changed
        const_global_iterator end() const      { return const_global_iterator(_regions.end(), _regions.end()); }

        //! \returns a const iterator to region with a given label.
        //! Note that it returns a global_iterator, but in the assignment to another type of iterator, the type is changed
        //! \param id : Identifier of the region we are looking for
        const_global_iterator find(identifier_type id) const      { return const_global_iterator(_regions.find(id), _regions.end()); }

    };



	}
}


#endif /* HIERARCHICAL_SPACE_PARTITION_HPP_ */
