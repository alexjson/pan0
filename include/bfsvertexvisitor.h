#ifndef BFSVERTEXVISITOR_H
#define BFSVERTEXVISITOR_H
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <pan0stitcher.h>

using namespace boost;

typedef adjacency_list<vecS, vecS, undirectedS> UndirectedGraph;
typedef boost::graph_traits<UndirectedGraph>::vertex_descriptor MyVertex;
typedef boost::graph_traits<UndirectedGraph>::edge_descriptor MyEdge;
typedef std::pair<int, int> Edge;

class Pan0Stitcher;

class BFSVertexVisitor : public boost::default_bfs_visitor {
public:
	void setPan0Stitcher(Pan0Stitcher* stitcher);
    void discover_vertex(MyVertex v, UndirectedGraph g);

private:
	Pan0Stitcher* stitcher_;
};


#endif // BFSVERTEXVISITOR_H


