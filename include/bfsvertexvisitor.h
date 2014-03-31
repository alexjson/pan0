#ifndef BFSVERTEXVISITOR_H
#define BFSVERTEXVISITOR_H
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/connected_components.hpp>
#include <pan0stitcher.h>

using namespace boost;
typedef property < edge_weight_t, int >Weight;
typedef adjacency_list < listS, vecS, undirectedS,
        no_property, property < edge_weight_t, int > > Graph;
// typedef adjacency_list<vecS, vecS, undirectedS> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor MyVertex;
typedef boost::graph_traits<Graph>::edge_descriptor MyEdge;
typedef std::pair<int, int> Edge;


class Pan0Stitcher;

class BFSVertexVisitor : public boost::default_bfs_visitor {
public:
    void setPan0Stitcher(Pan0Stitcher *stitcher);
    void discover_vertex(MyVertex v, Graph g);

private:
    Pan0Stitcher *stitcher_;
};

#endif // BFSVERTEXVISITOR_H