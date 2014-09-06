//
//  local_semantic_consistency.h
//  prc
//
//  Created by Guillem Palou Visa on 06/09/14.
//  Copyright (c) 2014 Guillem Palou Visa. All rights reserved.
//

#ifndef prc_local_semantic_consistency_h
#define prc_local_semantic_consistency_h


#include <imageplus/core/image_signal.hpp>
#include <imageplus/segmentation/partition/partition.hpp>

#include <set>
#include <queue>
#include <algorithm>
#include <map>

namespace imageplus {
	namespace semantic_segmentation {
        
        struct MatchingStruct {
            float64 true_precision;
            float64 true_recall;
            float64 inconsistent_precision;
            float64 inconsistent_recall;
            
            float64 false_positives;
            float64 false_negatives;
            float64 correct_detections;
            float64 inconsistent_detections;
            
            //ImageSignal<float64,3> img_boundaries;
            
            MatchingStruct() : true_precision(0), true_recall(0), inconsistent_precision(0), inconsistent_recall(0) {
            }
        } ;
        
        struct PointInContour {
            float64 x;
            float64 y;
            
            int64 		matching_index;
            float64 	matching_cost;
            bool 		matching_inconsistence;
            
            std::pair<int, int> label_pair;
            
            uint64 contour;
            
            PointInContour() : matching_index(-1), matching_cost(1e20) {
                
            }
        };
        
        
        typedef std::vector<PointInContour> 	Contours;
        
        template<class ImageModel>
        class SemanticMatcher {
            
            typedef ImageModel	ImageType;
            
        public:
            
            SemanticMatcher() {
                current_id = 0;
            }
            
            uint64 contour_id(uint64 id1,uint64 id2) {
                float64 t = id1;
                if (id1 > id2) {
                    id1 = id2;
                    id2 = t;
                }
                if (ids.find(std::pair<uint64,uint64>(id1,id2)) == ids.end()) {
                    ids[std::pair<uint64,uint64>(id1,id2)] = current_id++;
                }
                return ids[std::pair<uint64,uint64>(id1,id2)];
            }
            
            bool bfs() {
                uint64 u = 0, v = 0, len = 0;
                std::queue<uint64> Q;
                for(uint64 i= 0; i< G.size(); i++) {
                    if(match[i]==0) {
                        dist[i] = 0;
                        Q.push(i);
                    }
                    else dist[i] = 1e10;
                }
                dist[0] = 1e10;
                while(!Q.empty()) {
                    u = Q.front(); Q.pop();
                    if(u!=0) {
                        len = G[u].size();
                        for(uint64 i=0; i<len; i++) {
                            v = G[u][i];
                            if(dist[match[v]]==1e10) {
                                dist[match[v]] = dist[u] + 1;
                                Q.push(match[v]);
                            }
                        }
                    }
                }
                return (dist[0]!=1e10);
            }
            
            bool dfs(uint64 u) {
                
                if(u!=0) {
                    for(uint64 i=0; i< G[u].size(); i++) {
                        uint64 v = G[u][i];
                        if(dist[match[v]]==dist[u]+1) {
                            if(dfs(match[v])) {
                                match[v] = u;
                                match[u] = v;
                                return true;
                            }
                        }
                    }
                    dist[u] = 1e10;
                    return false;
                }
                return true;
            }
            
            std::pair<uint64, uint64> hopcroft_karp(Contours& a, Contours& b, float64 R) {
                uint64 matching = 0;
                
                // Construct Adjacency matrix
                uint64 N1 = a.size();
                uint64 N2 = b.size();
                
                G.clear(); match.clear(); dist.clear();
                
                G.resize(N1+N2);
                match.resize(N1+N2);
                dist.resize(N1+N2);
                
                for (uint64 i = 0; i < a.size(); i++) {
                    for (uint64 k = 0; k < b.size(); k++) {
                        if (matching_cost(a[i],b[k], R) < 1e100) {
                            G[i].push_back(N1+k);
                            G[N1+k].push_back(i);
                        }
                    }
                }
                
                while(bfs()) {
                    for(uint64 i=0; i < N1+N2; i++) {
                        if(match[i]==0 && dfs(i))
                            matching++;
                    }
                }
                
                //std::cout << "Matchings " << matching << std::endl;
                uint64 tp = 1;
                
                uint64 ip = 1;
                for (uint64 i = 0; i < N1; i++) {
                    if (match[i] != 0) {
                        bool bad_match = inconsistent(a[i],b[match[i]-N1]);
                        //std::cout << "matching " << a[i].x << "," << a[i].y << " " << b[match[i]-N1].x << "," << b[match[i]-N1].y << " " << bad_match << std::endl;
                        if (!bad_match) {
                            a[i].matching_cost = 0;
                            b[match[i]-N1].matching_cost = 0;
                            tp++;
                        } else {
                            a[i].matching_cost = 10;
                            b[match[i]-N1].matching_cost = 10;
                            ip++;
                        }
                    }
                }
                //std::cout << tp << " " << ip << std::endl;
                
                return std::pair<uint64,uint64>(tp, ip);
            }
            
            Contours find_contours(ImageType& label_map) {
                
                std::set<std::pair<uint64,uint64> > done;
                
                Contours contours;
                
                // used to transform the color into ids
                typename ImageType::value_float_type idx(1000000, 1000, 1);
                for (typename ImageType::iterator p = label_map.begin(); p != label_map.end(); ++p) {
                    typedef typename ImageType::template general_adjacency_iterator<Connectivity2D2>::type  adj_iterator;
                    
                    uint64 index1 = idx.transpose() * (*p);
                    
                    adj_iterator adj		 	= label_map.template general_adjacency_begin<Connectivity2D2>(p.pos());
                    adj_iterator adj_end	 	= label_map.template general_adjacency_end<Connectivity2D2>(p.pos());
                    
                    for (; adj!=adj_end; ++adj) {
                        
                        uint64 index2 = idx.transpose() * (*adj);
                        if (index1 != index2) {
                            float64 xp = p.pos()(0);
                            float64 yp = p.pos()(1);
                            if (p.pos()(0) != adj.pos()(0)) xp+=0.5;
                            if (p.pos()(1) != adj.pos()(1)) yp+=0.5;
                            PointInContour p;
                            p.x = xp;
                            p.y = yp;
                            p.contour = contour_id(index1,index2);
                            
                            //std::cout << "contour at " << xp << " " << yp << std::endl;
                            if (xp != (int)xp) { // vertical contour
                                int32 x1 = (int)xp;
                                int32 x2 = x1+1;
                                int32 y = (int)yp;
                                
                                if (index1 != index2) {
                                    p.label_pair.first = index1;
                                    p.label_pair.second = index2;
                                }
                            }
                            if (yp != (int)yp) { // horizontal contour
                                int32 y1 = (int)yp;
                                int32 y2 = y1+1;
                                int32 x = (int)xp;
                                if (label_map(x,y1)(0) != label_map(x,y2)(0)) {
                                    p.label_pair.first = index1;
                                    p.label_pair.second = index2;
                                }
                            }
                            
                            contours.push_back(p);
                        }
                    }
                }
                
                return contours;
            }
            
            bool inconsistent(PointInContour& a, PointInContour& b) {
                return (a.label_pair.first != b.label_pair.first) || (a.label_pair.second != b.label_pair.second);
            }
            
            float64 matching_cost(PointInContour& a, PointInContour& b, float64 max_dist) {
                float64 d = std::sqrt((a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y));
                
                if (d > max_dist) return 1e200;
                
                float64 dist_cost = d/max_dist;
                
                if (inconsistent(a,b)) return 1e5 + dist_cost;
                
                return dist_cost;
            }
            
            MatchingStruct match_contours(ImageType& result,ImageType& gt) {
                
                uint64 sx = result.size_x();
                uint64 sy = result.size_y();
                
                float64 R = std::sqrt(sx*sx + sy*sy) * 0.0075;
                
                // Extract the contours and their F/G approximate orientation
                Contours contours = find_contours(result);
                Contours gt_contours = find_contours(gt);
                
                MatchingStruct m;
                
                std::pair<uint64, uint64> p = hopcroft_karp(contours, gt_contours, R);
                
                float64 tp = p.first;
                float64 ip = p.second;
                
                if (contours.size() == 0) {
                    m.true_precision = 1;
                    m.inconsistent_precision = 0;
                }
                else {
                    m.true_precision 			= tp / contours.size();
                    m.inconsistent_precision	= ip / contours.size();
                }
                
                m.true_recall				= tp / gt_contours.size();
                m.inconsistent_recall		= ip / gt_contours.size();
                
                /*ImageType img(result.size_x(), result.size_y());
                 img.channel(0).fill(128);
                 img.channel(1).fill(128);
                 img.channel(2).fill(128);
                 
                 for (uint32 i = 0; i < gt_contours.size(); i++) {
                 PointInContour p = gt_contours[i];
                 
                 uint8 r = 0, g = 0, b = 0;
                 bool found = gt_contours[i].matching_cost < 1e10;
                 bool matched = gt_contours[i].matching_cost < 1;
                 if (found == true) {
                 if (matched == true) g = 255; else r = 255;
                 } else {
                 b = 255;
                 }
                 img((uint32)p.x,(uint32)p.y) = typename ImageType::value_type(b,g,r);
                 }
                 
                 m.img_boundaries = img;
                 m.img_boundaries.set_color_space(ColorSpaceRGB);*/
                return m;
            }
            
        private:
            uint64 current_id;
            
            std::map<std::pair<uint64,uint64>, uint64> ids;
            
            std::vector<std::vector<uint64> > G;
            std::vector<uint64> match;
            std::vector<uint64> dist;
        };
        
    }
}


#endif
