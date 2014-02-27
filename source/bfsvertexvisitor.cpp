#include <bfsvertexvisitor.h>


void BFSVertexVisitor::setPan0Stitcher(Pan0Stitcher *stitcher) {
    stitcher_ = stitcher;
};


void BFSVertexVisitor::discover_vertex(MyVertex v, UndirectedGraph g) {
    //Kalla på funktion från stitcher här, markera noder som stitchade. Gör koll i stitcher
    stitcher_->add(v);
    std::cout << v << std::endl;
}