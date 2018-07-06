    /* 
    Copyright (C) 2006 Pedro Felzenszwalb 
     
    This program is free software; you can redistribute it and/or modify 
    it under the terms of the GNU General Public License as published by 
    the Free Software Foundation; either version 2 of the License, or 
    (at your option) any later version. 
     
    This program is distributed in the hope that it will be useful, 
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
    GNU General Public License for more details. 
     
    You should have received a copy of the GNU General Public License 
    along with this program; if not, write to the Free Software 
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
    */  
      
    #ifndef SEGMENT_GRAPH  
    #define SEGMENT_GRAPH  
      
    #include <algorithm>  
    #include <cmath>  
    #include "disjoint-set.h"  
      
    // threshold function  
    #define THRESHOLD(size, c) (c/size)  
      
    typedef struct {  
        //float w;  
        double w;  
        int a, b;  
    } edge;  
      
    bool operator<(const edge &a, const edge &b) {  
        return a.w < b.w;  
    }  
      
    bool cmpEdge(const edge &edge1, const edge &edge2)  
    {  
        return (edge1.w < edge2.w);  
    }  
    /* 
    * Segment a graph 
    * 
    * Returns a disjoint-set forest representing the segmentation. 
    * 
    * num_vertices: number of vertices in graph. 
    * num_edges: number of edges in graph 
    * edges: array of edges. 
    * c: constant for treshold function. 
    */  
    universe *segment_graph(int num_vertices, int num_edges, edge *edges,  
        float c) {  
        // sort edges by weight  
        // how do you specify the weight as compared component   
        //std::sort(edges, edges + num_edges); //original  
        //std::sort(edges, edges + num_edges, cmpEdge);//Sorts the elements in the range [first,last) into ascending order.  
        std::sort(edges, edges + num_edges,   
            [](const edge &e1, const edge &e2)  
             {return e1.w < e2.w; });//lambda expression  
        // make a disjoint-set forest  
        universe *u = new universe(num_vertices);  
      
        // init thresholds  
        //float *threshold = new float[num_vertices];  
        double *threshold = new double[num_vertices];  
        for (int i = 0; i < num_vertices; i++)  
            threshold[i] = THRESHOLD(1, c); //c == k, reference to fomula (5)  
      
        // for each edge, in non-decreasing weight order...  
      
      
        for (int i = 0; i < num_edges; i++) {  
            edge *pedge = &edges[i];  
      
            // components conected by this edge  
            int a = u->find(pedge->a);  
            int b = u->find(pedge->b);  
            if (a != b) {  
      
                if ((pedge->w <= threshold[a]) &&(pedge->w <= threshold[b]))  
                //Note 1: as the edges was sorted by weight in ascending order,  
                // so so the pedge->w is minimum weight edge connecting a,and b namely Dif(a,a)   
                //Note 2: less than the min(a,b) the must less than both a and b ,reference  fomula (3)  
                {  
                    //int sa = u->size(a), sb = u->size(b);  
      
                    u->join(a, b); // the id of merged component is the a/b which is determined by thier rank   
                    a = u->find(a); // the id of merged component ,using b will produce the same result  
      
                    threshold[a] = pedge->w + THRESHOLD(u->size(a), c); // c/size(a)  
                    //Note: as the edges was sorted by weight in ascending order   
                    //,so the pedge->w is the largest weight in the new component,namely Int( Cnew) reference to fomula (1)  
                }  
            }  
        }  
        // free up  
        delete threshold;  
        return u;  
    }  
      
    #endif  
