#include "graphlab_unit_tests.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

#include <QtTest/QtTest>

#include "algorithm/Astar/astar.hpp"
#include "algorithm/BFS/bfs.hpp"
#include "algorithm/DFS/dfs.hpp"
#include "algorithm/Dijkstra/dijkstra.hpp"
#include "algorithm/Euler/euler.hpp"
#include "algorithm/Floyd_Warshall/floyd_warshall.hpp"
#include "algorithm/MST_Kruskal/mst_kruskal.hpp"
#include "algorithm/MST_Prim/mst_prim.hpp"
#include "graph/graph.hpp"

namespace {

std::vector<graph::edge_id_t> sorted(std::vector<graph::edge_id_t> values)
{
    std::sort(values.begin(), values.end());
    return values;
}

}

void GraphLabUnitTests::bfsAndDfsVisitReachableEdgesFromSelectedStartNode()
{
    graph::UnweightedDirectedGraph graph;
    const auto n0 = graph.add_node({0.0, 0.0}, "0");
    const auto n1 = graph.add_node({1.0, 0.0}, "1");
    const auto n2 = graph.add_node({2.0, 0.0}, "2");
    const auto e0 = graph.add_edge(n0, n1);
    const auto e1 = graph.add_edge(n0, n2);

    algorithm::BFS bfs(graph);
    algorithm::DFS dfs(graph);
    const std::vector<graph::edge_id_t> expectedEdges{e0, e1};

    QVERIFY(sorted(bfs.run(n0)) == expectedEdges);
    QVERIFY(sorted(dfs.run(n0)) == expectedEdges);
}

void GraphLabUnitTests::basicGraphTraversalChecksAlwaysPass()
{
    graph::UnweightedDirectedGraph graph;
    graph.add_node({0.0, 0.0}, "0");

    algorithm::BFS bfs(graph);
    algorithm::DFS dfs(graph);

    QVERIFY(bfs.check());
    QVERIFY(dfs.check());
}

void GraphLabUnitTests::dijkstraReturnsShortestPathAndRejectsNegativeWeights()
{
    graph::WeightedDirectedGraph graph;
    const auto n0 = graph.add_node({0.0, 0.0}, "0");
    const auto n1 = graph.add_node({1.0, 0.0}, "1");
    const auto n2 = graph.add_node({2.0, 0.0}, "2");
    const auto e0 = graph.add_edge(n0, n1, 2.0);
    const auto e1 = graph.add_edge(n1, n2, 3.0);
    graph.add_edge(n0, n2, 10.0);

    algorithm::Dijkstra dijkstra(graph);
    const auto result = dijkstra.run({n0, n2});
    const std::vector<graph::edge_id_t> expectedPath{e0, e1};

    QVERIFY(result.first == expectedPath);
    QCOMPARE(result.second, 5.0);

    graph.change_edge_weight(e0, -1.0);
    QVERIFY(!dijkstra.check());
}

void GraphLabUnitTests::astarFindsExpectedPathOnSimpleWeightedGraph()
{
    graph::WeightedDirectedGraph graph;
    const auto n0 = graph.add_node({0.0, 0.0}, "0");
    const auto n1 = graph.add_node({1.0, 0.0}, "1");
    const auto n2 = graph.add_node({2.0, 0.0}, "2");
    const auto e0 = graph.add_edge(n0, n1, 2.0);
    const auto e1 = graph.add_edge(n1, n2, 3.0);
    graph.add_edge(n0, n2, 10.0);

    algorithm::AStar astar(graph);
    const auto result = astar.run({n0, n2, "MinWeight"});
    const std::vector<graph::edge_id_t> expectedPath{e0, e1};

    QVERIFY(result.first == expectedPath);
    QCOMPARE(result.second, 5.0);
}

void GraphLabUnitTests::astarSupportsAllHeuristicModesAndReportsMissingPaths()
{
    graph::WeightedDirectedGraph graph;
    const auto n0 = graph.add_node({0.0, 0.0}, "0");
    const auto n1 = graph.add_node({1.0, 0.0}, "1");
    const auto n2 = graph.add_node({2.0, 0.0}, "2");
    const auto n3 = graph.add_node({3.0, 0.0}, "3");
    const auto e0 = graph.add_edge(n0, n1, 1.0);
    const auto e1 = graph.add_edge(n1, n2, 2.0);
    graph.add_edge(n0, n2, 10.0);

    algorithm::AStar astar(graph);
    QVERIFY(astar.check());

    const std::vector<graph::edge_id_t> expectedPath{e0, e1};
    const auto edgeDensityResult = astar.run({n0, n2, "EdgeDensity"});
    const auto randomWalkResult = astar.run({n0, n2, "RandomWalk"});
    const auto fallbackResult = astar.run({n0, n2, "UnknownHeuristic"});

    QVERIFY(edgeDensityResult.first == expectedPath);
    QCOMPARE(edgeDensityResult.second, 3.0);
    QVERIFY(randomWalkResult.first == expectedPath);
    QCOMPARE(randomWalkResult.second, 3.0);
    QVERIFY(fallbackResult.first == expectedPath);
    QCOMPARE(fallbackResult.second, 3.0);

    const auto missingPath = astar.run({n0, n3, "MinWeight"});
    QVERIFY(missingPath.first.empty());
    QVERIFY(std::isinf(missingPath.second));

    graph.change_edge_weight(e0, -1.0);
    QVERIFY(!astar.check());
}

void GraphLabUnitTests::floydWarshallComputesAllPairsShortestDistances()
{
    graph::WeightedDirectedGraph graph;
    const auto n0 = graph.add_node({0.0, 0.0}, "0");
    const auto n1 = graph.add_node({1.0, 0.0}, "1");
    const auto n2 = graph.add_node({2.0, 0.0}, "2");
    graph.add_edge(n0, n1, 2.0);
    graph.add_edge(n1, n2, 3.0);
    graph.add_edge(n0, n2, 10.0);

    algorithm::FloydWarshall floydWarshall(graph);
    const auto result = floydWarshall.run(std::nullopt);

    QCOMPARE(result.first.at(n0).at(n2), 5.0);
    QCOMPARE(result.second.at(n0).at(n2).size(), std::size_t{2});
}

void GraphLabUnitTests::floydWarshallHandlesUndirectedGraphsAndNegativeCycleCheck()
{
    graph::WeightedUndirectedGraph undirected;
    const auto u0 = undirected.add_node({0.0, 0.0}, "0");
    const auto u1 = undirected.add_node({1.0, 0.0}, "1");
    const auto edge = undirected.add_edge(u0, u1, 4.0);

    algorithm::FloydWarshall floydUndirected(undirected);
    const auto undirectedResult = floydUndirected.run(std::nullopt);

    QVERIFY(floydUndirected.check());
    QCOMPARE(undirectedResult.first.at(u0).at(u1), 4.0);
    QCOMPARE(undirectedResult.first.at(u1).at(u0), 4.0);
    QCOMPARE(undirectedResult.second.at(u0).at(u1), std::vector<graph::edge_id_t>{edge});
    QCOMPARE(undirectedResult.second.at(u1).at(u0), std::vector<graph::edge_id_t>{edge});

    graph::WeightedDirectedGraph negativeCycle;
    const auto n0 = negativeCycle.add_node({0.0, 0.0}, "0");
    const auto n1 = negativeCycle.add_node({1.0, 0.0}, "1");
    negativeCycle.add_edge(n0, n1, -2.0);
    negativeCycle.add_edge(n1, n0, -2.0);

    algorithm::FloydWarshall floydNegativeCycle(negativeCycle);
    QVERIFY(!floydNegativeCycle.check());
}

void GraphLabUnitTests::primAndKruskalReturnSameMinimumSpanningTreeWeight()
{
    graph::WeightedUndirectedGraph graph;
    const auto n0 = graph.add_node({0.0, 0.0}, "0");
    const auto n1 = graph.add_node({1.0, 0.0}, "1");
    const auto n2 = graph.add_node({2.0, 0.0}, "2");
    graph.add_edge(n0, n1, 1.0);
    graph.add_edge(n1, n2, 2.0);
    graph.add_edge(n0, n2, 5.0);

    algorithm::MST_Prim prim(graph);
    algorithm::MST_Kruskal kruskal(graph);

    const auto primResult = prim.run(n0);
    const auto kruskalResult = kruskal.run(n0);

    QVERIFY(prim.check());
    QVERIFY(kruskal.check());
    QCOMPARE(primResult.second, 3.0);
    QCOMPARE(kruskalResult.second, 3.0);
    QCOMPARE(primResult.first.size(), std::size_t{2});
    QCOMPARE(kruskalResult.first.size(), std::size_t{2});
}

void GraphLabUnitTests::eulerDetectsAndReturnsPathForSemiEulerianUndirectedGraphs()
{
    graph::UnweightedUndirectedGraph graph;
    const auto n0 = graph.add_node({0.0, 0.0}, "0");
    const auto n1 = graph.add_node({1.0, 0.0}, "1");
    const auto n2 = graph.add_node({2.0, 0.0}, "2");
    graph.add_edge(n0, n1);
    graph.add_edge(n1, n2);

    algorithm::Euler euler(graph);

    QVERIFY(euler.check());
    QVERIFY(euler.is_eulerian_path());
    QVERIFY(!euler.is_eulerian_circuit());
    QCOMPARE(euler.run("Eulerian path").size(), std::size_t{2});
}

void GraphLabUnitTests::eulerHandlesCircuitDirectedAndEmptyGraphs()
{
    graph::UnweightedUndirectedGraph circuitGraph;
    const auto c0 = circuitGraph.add_node({0.0, 0.0}, "0");
    const auto c1 = circuitGraph.add_node({1.0, 0.0}, "1");
    const auto c2 = circuitGraph.add_node({2.0, 0.0}, "2");
    circuitGraph.add_edge(c0, c1);
    circuitGraph.add_edge(c1, c2);
    circuitGraph.add_edge(c2, c0);

    algorithm::Euler circuit(circuitGraph);
    QVERIFY(circuit.check());
    QVERIFY(circuit.is_eulerian_circuit());
    QVERIFY(!circuit.is_eulerian_path());
    QCOMPARE(circuit.run("Eulerian circuit").size(), std::size_t{3});

    graph::UnweightedDirectedGraph directed;
    directed.add_node({0.0, 0.0}, "0");
    directed.add_node({1.0, 0.0}, "1");
    directed.add_edge(0, 1);

    algorithm::Euler directedEuler(directed);
    QVERIFY(!directedEuler.check());

    graph::UnweightedUndirectedGraph empty;
    algorithm::Euler emptyEuler(empty);
    QVERIFY(!emptyEuler.is_eulerian_circuit());
}

void GraphLabUnitTests::algorithmIterationsCanMoveForwardBackwardAndReset()
{
    graph::UnweightedDirectedGraph graph;
    const auto n0 = graph.add_node({0.0, 0.0}, "0");
    const auto n1 = graph.add_node({1.0, 0.0}, "1");
    const auto n2 = graph.add_node({2.0, 0.0}, "2");
    const auto e0 = graph.add_edge(n0, n1);
    const auto e1 = graph.add_edge(n1, n2);

    algorithm::BFS bfs(graph);
    bfs.run(n0);

    const auto firstIteration = bfs.getCurrentIteration();
    const auto secondIteration = bfs.getCurrentIteration();
    const auto noMoreIterations = bfs.getCurrentIteration();

    QVERIFY(firstIteration.has_value());
    QVERIFY(secondIteration.has_value());
    QVERIFY(!noMoreIterations.has_value());
    QCOMPARE(firstIteration.value(), e0);
    QCOMPARE(secondIteration.value(), e1);

    const auto previousIteration = bfs.getPreviousIteration();
    QVERIFY(previousIteration.has_value());
    QCOMPARE(previousIteration.value(), e1);

    const auto nextIteration = bfs.getNextIteration();
    QVERIFY(!nextIteration.has_value());

    bfs.removeAllIterations();
    QVERIFY(!bfs.getCurrentIteration().has_value());
}
