/*
 * false_color.hpp
 *
 *  Created on: Dec 18, 2012
 *      Author: gpalou
 */

#ifndef FALSE_COLOR_HPP_
#define FALSE_COLOR_HPP_

namespace imageplus {
	namespace segmentation {

			template<class Signal, class PartitionModel>
			Signal to_false_color(PartitionModel& part) {
				std::map<uint64, uint64> 		color_map;

				uint32 current_label = 0;

				for (typename PartitionModel::iterator v = part.begin(); v != part.end(); ++v) {
					uint64 label = (*v)(0);
					if (color_map.find(label) == color_map.end()) {
						color_map[label] = current_label++;
					}
				}

				std::vector<std::vector<uint8> > 	colors(current_label);

				for (uint64 i = 0; i < current_label; i++) {
					bool b = true;
					std::vector<uint8> v(3);
					do {
						b = true;
						v[0] = rand() % 256;
						v[1] = rand() % 256;
						v[2] = rand() % 256;
						for (uint64 k = 0; k < i; k++) if (colors[k] == v) { b = false; }
					} while (b == false);
					colors[i] = v;
				}

				//std::cout << "There are " << current_label << " regions " << std::endl;
				Signal 				segmented(part.sizes());

				for (typename PartitionModel::iterator v = part.begin(); v != part.end(); ++v) {
					typename Signal::coord_type pos = v.pos();
					uint64 label = (*v)(0);
					if (color_map.find(label) == color_map.end()) {
						color_map[label] = current_label++;
					}

					std::vector<uint8>& color = colors[color_map[label]];

					segmented(pos) = typename Signal::value_type(color[0],color[1],color[2]);
				}

				return segmented;
			}

			template<class PartitionModel, class Signal>
			PartitionModel to_partition(Signal& segmented) {
				std::map<uint64, uint64> 		id_map;
				PartitionModel part(segmented.sizes());

				uint64 current_label = 0;
				for (typename PartitionModel::iterator v = part.begin(); v != part.end(); ++v) {
					typename PartitionModel::coord_type pos = v.pos();
					uint64 c_id = segmented(pos)(0)*255*255 + segmented(pos)(1)*255 + segmented(pos)(2);
					if (id_map.find(c_id) == id_map.end()) {
						id_map[c_id] = current_label++;
						(*v)(0) = current_label-1;
					} else {
						(*v)(0) = id_map[c_id];
					}
				}
				part.set_max_label(current_label-1);
				return part;
			}

	}
}

#endif /* FALSE_COLOR_HPP_ */
