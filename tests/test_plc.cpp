#include "test_macros.h"
#include "../src/PLC.h"

TEST(test_plc_edge) {
    std::vector<uint32_t> inc_tri = {1, 2};
    PLCedge edge(10, 20, 10, 20, inc_tri, one_acute_ep);

    ASSERT_EQ(edge.ep[0], 10);
    ASSERT_EQ(edge.ep[1], 20);
    ASSERT_FALSE(edge.isFlat());
    ASSERT_FALSE(edge.isIsolated());

    PLCedge edge2(20, 10, 20, 10, inc_tri, one_acute_ep);
    ASSERT_TRUE(edge.coincident(edge2));

    ASSERT_EQ(edge.oppositeVertex(10), 20);
    ASSERT_EQ(edge.oppositeVertex(20), 10);

    ASSERT_EQ(edge.commonVertex(edge2), 10); // Could be either 10 or 20, but the current implementation returns the first match it finds.
    
    edge.replaceIncidentFace(1, 3);
    ASSERT_EQ(edge.inc_tri[0], 3);
    ASSERT_EQ(edge.inc_tri[1], 2);
}

TEST(test_plc_face) {
    PLCface face;
    ASSERT_TRUE(PLCface::isEmpty(face));
    ASSERT_TRUE(face.is_convex);
    ASSERT_TRUE(face.is_simply_connected);

    PLCedge e1(1, 2, 1, 2, {}, no_acute_ep);
    face.bounding_edges.push_back(&e1);
    ASSERT_FALSE(PLCface::isEmpty(face));
}
