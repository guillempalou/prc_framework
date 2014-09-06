/*
 * hierachical_region_iterators.hpp
 *
 *  Created on: Jul 31, 2012
 *      Author: guillem
 */

#ifndef HIERACHICAL_REGION_ITERATORS_HPP_
#define HIERACHICAL_REGION_ITERATORS_HPP_

#include <iostream>

namespace imageplus {

	template<class RegionModel>
    class RegionIteratorBase : public std::iterator<std::forward_iterator_tag, typename RegionModel::coord_type> {

    	typedef typename RegionModel::coord_type			coord_type;
    	typedef RegionModel*								RegionPointer;
    	typedef typename RegionModel::RegionBaseType*		RegionBasePointer;
    	typedef typename RegionModel::CoordsContainerType	ContainerType;
    	typedef typename ContainerType::iterator 			container_iterator;

    public:

    	RegionIteratorBase() {
    	}

    	RegionIteratorBase(const RegionIteratorBase& copy) : _start_reg(copy._start_reg), _current_reg(copy._current_reg), _coord(copy._coord) {
    		_end = copy._end;

    	}

    	RegionIteratorBase(RegionPointer const reg, bool pos_end = false) {
    		_start_reg = reg;

    		if (pos_end == false) {
    			_current_reg = reg;
    			while (_current_reg->coordinates().size() == 0)  { _current_reg = _current_reg->child(0); }
    			_coord = ((RegionBasePointer)_current_reg)->begin();
    		}

       		_end = pos_end;
    	}

    	~RegionIteratorBase() {}

    	RegionIteratorBase& operator=(const RegionIteratorBase& other) {
    		_current_reg = other._current_reg;
    		_coord = other._coord;
    		_start_reg = other._start_reg;
    		_end = other._end;
    		return (*this);
    	}

    	bool operator==(const RegionIteratorBase& other) const {
    		if (other._end == true) {
    			return (_end && (other._start_reg == _start_reg));
    		}
    		if (_end) return false;
    		//Assumes _coord as eigen::vector
    		return ((*_coord) - (*(other._coord))).isZero();
    	}

    	bool operator!=(const RegionIteratorBase& other) const {
    		return !this->operator==(other);
    	}

    	RegionIteratorBase& operator++() {

    		if (_end == true) return (*this);

    		++_coord;


    		if (_coord == ((RegionBasePointer)_current_reg)->end()) {
    			// Find the child of the first parent with a non-visited child
    			if (_current_reg == _start_reg) _end = true;

    			while (!_end) {
       				if (!_current_reg->parent() && _current_reg->coordinates().size() != 0) // if its the root node
    					_end = true;
    				else
    					if (_current_reg->parent()->child(0) == _current_reg) {
    						_current_reg = _current_reg->parent()->child(1);
    						break;
    					} else {
    						_current_reg = _current_reg->parent();
    						if (_current_reg == _start_reg) {
    							_end = true;
    							break;
    						}
    					}
    			}
    			// Get the first non-visited leave
       			if (!_end) {
    				while (_current_reg->coordinates().size() == 0) _current_reg = _current_reg->child(0);
    				_coord = ((RegionBasePointer)_current_reg)->begin();
    			}

    		}

    		return (*this);
    	}

    	coord_type& operator*() {
    		return (*_coord);
    	}

    	const coord_type& operator*() const {
    		return (*_coord);
    	}

    private:

    	RegionPointer		_start_reg;
    	RegionPointer		_current_reg;
    	bool _end;
    	container_iterator 	_coord;
    };
   }


#endif /* HIERACHICAL_REGION_ITERATORS_HPP_ */
