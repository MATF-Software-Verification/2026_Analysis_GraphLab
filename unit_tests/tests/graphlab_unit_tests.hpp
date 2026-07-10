#pragma once

#include <QObject>

class GraphLabUnitTests : public QObject
{
    Q_OBJECT

private slots:
    void unweightedUndirectedGraphStoresNodesAndSymmetricEdges();
    void weightedDirectedGraphStoresEdgeDirectionAndWeight();
    void removingUndirectedNodeRemovesIncidentEdges();
    void graphMetadataCanBeChangedAfterInsertion();
    void graphsReportAbsenceOfNegativeWeights();
    void neighboursAreReturnedFromAdjacencyList();
    void removingEdgesUpdatesGraphStorageForEachGraphType();
    void nodesAndEdgesCanBeConvertedToAndFromQVariant();
    void graphVariantContainsAdjacencyEdgeEntries();
    void negativeCycleDetectionDistinguishesCyclicAndAcyclicGraphs();
    void bfsAndDfsVisitReachableEdgesFromSelectedStartNode();
    void dijkstraReturnsShortestPathAndRejectsNegativeWeights();
    void astarFindsExpectedPathOnSimpleWeightedGraph();
    void astarSupportsAllHeuristicModesAndReportsMissingPaths();
    void floydWarshallComputesAllPairsShortestDistances();
    void floydWarshallHandlesUndirectedGraphsAndNegativeCycleCheck();
    void primAndKruskalReturnSameMinimumSpanningTreeWeight();
    void eulerDetectsAndReturnsPathForSemiEulerianUndirectedGraphs();
    void eulerHandlesCircuitDirectedAndEmptyGraphs();
    void serializerSavesAndLoadsGraphStructureAndGraphTypeFlags();
    void graphCanBeRoundTrippedThroughQVariant();
    void serializerIgnoresMissingFiles();
    void algorithmIterationsCanMoveForwardBackwardAndReset();
    void basicGraphTraversalChecksAlwaysPass();
};
