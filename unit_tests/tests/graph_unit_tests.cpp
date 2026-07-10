#include "graphlab_unit_tests.hpp"

#include <algorithm>
#include <vector>

#include <QtTest/QtTest>

#include "graph/graph.hpp"

void GraphLabUnitTests::unweightedUndirectedGraphStoresNodesAndSymmetricEdges()
{
    graph::UnweightedUndirectedGraph graph;

    const auto first = graph.add_node({10.0, 20.0}, "A");
    const auto second = graph.add_node({30.0, 40.0}, "B");
    const auto edge = graph.add_edge(first, second);

    QCOMPARE(graph.getMapNodes().size(), std::size_t{2});
    QCOMPARE(graph.getMapEdges().size(), std::size_t{1});
    QCOMPARE(graph.getAdjList().at(first).at(edge), second);
    QCOMPARE(graph.getAdjList().at(second).at(edge), first);
}

void GraphLabUnitTests::weightedDirectedGraphStoresEdgeDirectionAndWeight()
{
    graph::WeightedDirectedGraph graph;

    const auto start = graph.add_node({0.0, 0.0}, "start");
    const auto end = graph.add_node({1.0, 1.0}, "end");
    const auto edge = graph.add_edge(start, end, 3.5);

    QCOMPARE(graph.getMapEdges().size(), std::size_t{1});
    QCOMPARE(graph.getMapEdges().at(edge).getStartId(), start);
    QCOMPARE(graph.getMapEdges().at(edge).getEndId(), end);
    QCOMPARE(graph.getMapEdges().at(edge).weight, 3.5);
    QCOMPARE(graph.getAdjList().at(start).at(edge), end);
    QVERIFY(graph.getAdjList().at(end).empty());
}

void GraphLabUnitTests::removingUndirectedNodeRemovesIncidentEdges()
{
    graph::UnweightedUndirectedGraph graph;

    const auto first = graph.add_node({0.0, 0.0}, "A");
    const auto second = graph.add_node({1.0, 1.0}, "B");
    graph.add_edge(first, second);

    graph.remove_node(second);

    QCOMPARE(graph.getMapNodes().count(second), std::size_t{0});
    QCOMPARE(graph.getAdjList().count(second), std::size_t{0});
    QVERIFY(graph.getMapEdges().empty());
    QVERIFY(graph.getAdjList().at(first).empty());
}

void GraphLabUnitTests::graphMetadataCanBeChangedAfterInsertion()
{
    graph::WeightedUndirectedGraph graph;

    const auto first = graph.add_node({0.0, 0.0}, "old");
    const auto second = graph.add_node({1.0, 1.0}, "B");
    const auto edge = graph.add_edge(first, second, 1.0);

    graph.change_node_title(first, "new");
    graph.move_node(first, {7.0, 9.0});
    graph.change_edge_weight(edge, -2.0);

    QCOMPARE(graph.getMapNodes().at(first).title, std::string("new"));
    QCOMPARE(graph.getMapNodes().at(first).pos.first, 7.0);
    QCOMPARE(graph.getMapNodes().at(first).pos.second, 9.0);
    QCOMPARE(graph.getMapEdges().at(edge).weight, -2.0);
    QVERIFY(graph.contains_negative_weight());
}

void GraphLabUnitTests::graphsReportAbsenceOfNegativeWeights()
{
    graph::WeightedUndirectedGraph graph;

    const auto first = graph.add_node({0.0, 0.0}, "A");
    const auto second = graph.add_node({1.0, 1.0}, "B");
    graph.add_edge(first, second, 2.0);

    QVERIFY(!graph.contains_negative_weight());
}

void GraphLabUnitTests::neighboursAreReturnedFromAdjacencyList()
{
    graph::UnweightedDirectedGraph graph;

    const auto start = graph.add_node({0.0, 0.0}, "start");
    const auto first = graph.add_node({1.0, 0.0}, "first");
    const auto second = graph.add_node({2.0, 0.0}, "second");
    graph.add_edge(start, first);
    graph.add_edge(start, second);

    const auto neighbours = graph.iterate_neighbours(start);

    QCOMPARE(neighbours.size(), std::size_t{2});
    QVERIFY(std::find(neighbours.begin(), neighbours.end(), first) != neighbours.end());
    QVERIFY(std::find(neighbours.begin(), neighbours.end(), second) != neighbours.end());
}

void GraphLabUnitTests::removingEdgesUpdatesGraphStorageForEachGraphType()
{
    graph::UnweightedUndirectedGraph unweightedUndirected;
    const auto uu0 = unweightedUndirected.add_node({0.0, 0.0}, "0");
    const auto uu1 = unweightedUndirected.add_node({1.0, 0.0}, "1");
    const auto uuEdge = unweightedUndirected.add_edge(uu0, uu1);
    unweightedUndirected.remove_edge(uuEdge);
    QVERIFY(unweightedUndirected.getMapEdges().empty());
    QVERIFY(unweightedUndirected.getAdjList().at(uu0).empty());
    QVERIFY(unweightedUndirected.getAdjList().at(uu1).empty());

    graph::UnweightedDirectedGraph unweightedDirected;
    const auto ud0 = unweightedDirected.add_node({0.0, 0.0}, "0");
    const auto ud1 = unweightedDirected.add_node({1.0, 0.0}, "1");
    const auto udEdge = unweightedDirected.add_edge(ud0, ud1);
    unweightedDirected.remove_edge(udEdge);
    QVERIFY(unweightedDirected.getMapEdges().empty());
    QVERIFY(unweightedDirected.getAdjList().at(ud0).empty());
    QVERIFY(unweightedDirected.getAdjList().at(ud1).empty());

    graph::WeightedDirectedGraph weightedDirected;
    const auto wd0 = weightedDirected.add_node({0.0, 0.0}, "0");
    const auto wd1 = weightedDirected.add_node({1.0, 0.0}, "1");
    const auto wdEdge = weightedDirected.add_edge(wd0, wd1, 4.0);
    weightedDirected.remove_edge(wdEdge);
    QVERIFY(weightedDirected.getMapEdges().empty());
    QVERIFY(weightedDirected.getAdjList().at(wd0).empty());
    QVERIFY(weightedDirected.getAdjList().at(wd1).empty());

    graph::WeightedUndirectedGraph weightedUndirected;
    const auto wu0 = weightedUndirected.add_node({0.0, 0.0}, "0");
    const auto wu1 = weightedUndirected.add_node({1.0, 0.0}, "1");
    const auto wuEdge = weightedUndirected.add_edge(wu0, wu1, 4.0);
    weightedUndirected.remove_edge(wuEdge);
    QVERIFY(weightedUndirected.getMapEdges().empty());
    QVERIFY(weightedUndirected.getAdjList().at(wu0).empty());
    QVERIFY(weightedUndirected.getAdjList().at(wu1).empty());
}

void GraphLabUnitTests::nodesAndEdgesCanBeConvertedToAndFromQVariant()
{
    graph::Node node(7, {12.5, 20.5}, "node");
    const auto nodeVariant = node.toVariant();

    graph::Node loadedNode(0, {0.0, 0.0}, "");
    loadedNode.fromVariant(nodeVariant);

    QCOMPARE(loadedNode.getId(), graph::node_id_t{7});
    QCOMPARE(loadedNode.pos.first, 37.5);
    QCOMPARE(loadedNode.pos.second, 45.5);
    QCOMPARE(loadedNode.title, std::string("node"));

    graph::Edge edge(3, 7, 9, 2.5);
    const auto edgeVariant = edge.toVariant();

    graph::Edge loadedEdge(0, 0, 0);
    loadedEdge.fromVariant(edgeVariant);

    QCOMPARE(loadedEdge.getId(), graph::edge_id_t{3});
    QCOMPARE(loadedEdge.getStartId(), graph::node_id_t{7});
    QCOMPARE(loadedEdge.getEndId(), graph::node_id_t{9});
    QCOMPARE(loadedEdge.weight, 2.5);
}

void GraphLabUnitTests::graphVariantContainsAdjacencyEdgeEntries()
{
    graph::WeightedDirectedGraph graph;
    const auto start = graph.add_node({0.0, 0.0}, "start");
    const auto end = graph.add_node({1.0, 1.0}, "end");
    const auto edge = graph.add_edge(start, end, 6.0);

    const auto variantMap = graph.toVariant().toMap();
    const auto adjacencyList = variantMap["adjacency_list"].toList();

    bool foundStartNode = false;
    bool foundEdgeEntry = false;

    for (const auto& nodeVariant : adjacencyList) {
        const auto nodeMap = nodeVariant.toMap();
        if (nodeMap["node_id"].toUInt() != start) {
            continue;
        }

        foundStartNode = true;
        const auto edges = nodeMap["edges"].toList();
        for (const auto& edgeVariant : edges) {
            const auto edgeMap = edgeVariant.toMap();
            if (edgeMap["edge_id"].toUInt() == edge &&
                edgeMap["end_id"].toUInt() == end) {
                foundEdgeEntry = true;
            }
        }
    }

    QVERIFY(foundStartNode);
    QVERIFY(foundEdgeEntry);
}

void GraphLabUnitTests::negativeCycleDetectionDistinguishesCyclicAndAcyclicGraphs()
{
    graph::WeightedDirectedGraph cyclic;
    const auto c0 = cyclic.add_node({0.0, 0.0}, "0");
    const auto c1 = cyclic.add_node({1.0, 0.0}, "1");
    const auto c2 = cyclic.add_node({2.0, 0.0}, "2");
    cyclic.add_edge(c0, c1, 1.0);
    cyclic.add_edge(c1, c2, -3.0);
    cyclic.add_edge(c2, c0, 1.0);

    graph::WeightedDirectedGraph acyclic;
    const auto a0 = acyclic.add_node({0.0, 0.0}, "0");
    const auto a1 = acyclic.add_node({1.0, 0.0}, "1");
    const auto a2 = acyclic.add_node({2.0, 0.0}, "2");
    acyclic.add_edge(a0, a1, 1.0);
    acyclic.add_edge(a1, a2, 2.0);

    QVERIFY(cyclic.contains_negative_cucles());
    QVERIFY(!acyclic.contains_negative_cucles());
}

void GraphLabUnitTests::graphCanBeRoundTrippedThroughQVariant()
{
    graph::WeightedUndirectedGraph original;
    const auto first = original.add_node({1.0, 2.0}, "first");
    const auto second = original.add_node({3.0, 4.0}, "second");
    const auto edge = original.add_edge(first, second, 7.0);

    graph::WeightedUndirectedGraph loaded;
    loaded.fromVariant(original.toVariant());

    QCOMPARE(loaded.getMapNodes().size(), std::size_t{2});
    QCOMPARE(loaded.getMapEdges().size(), std::size_t{1});
    QCOMPARE(loaded.getAdjList().at(first).at(edge), second);
    QCOMPARE(loaded.getAdjList().at(second).at(edge), first);
    QCOMPARE(loaded.getMapEdges().at(edge).weight, 7.0);
}
