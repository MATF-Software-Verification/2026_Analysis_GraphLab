#include "graphlab_unit_tests.hpp"

#include <QTemporaryDir>
#include <QtTest/QtTest>

#include "Serialization/Serializer.hpp"
#include "graph/graph.hpp"

void GraphLabUnitTests::serializerSavesAndLoadsGraphStructureAndGraphTypeFlags()
{
    QTemporaryDir temporaryDirectory;
    QVERIFY(temporaryDirectory.isValid());

    const QString filePath = temporaryDirectory.filePath("sample.graph");

    graph::WeightedDirectedGraph original;
    const auto start = original.add_node({10.0, 20.0}, "start");
    const auto end = original.add_node({30.0, 40.0}, "end");
    original.add_edge(start, end, 4.25);

    Serializer::save(original, filePath, true, true);

    graph::WeightedDirectedGraph loaded;
    bool isWeighted = false;
    bool isDirected = false;
    Serializer::load(loaded, filePath, isWeighted, isDirected);

    QVERIFY(isWeighted);
    QVERIFY(isDirected);
    QCOMPARE(loaded.getMapNodes().size(), std::size_t{2});
    QCOMPARE(loaded.getMapEdges().size(), std::size_t{1});

    QCOMPARE(loaded.getMapNodes().at(start).title, std::string("start"));
    QCOMPARE(loaded.getMapNodes().at(end).title, std::string("end"));
    QCOMPARE(loaded.getMapEdges().begin()->second.getStartId(), start);
    QCOMPARE(loaded.getMapEdges().begin()->second.getEndId(), end);
    QCOMPARE(loaded.getMapEdges().begin()->second.weight, 4.25);
}

void GraphLabUnitTests::serializerIgnoresMissingFiles()
{
    graph::WeightedDirectedGraph loaded;
    loaded.add_node({1.0, 2.0}, "existing");

    bool isWeighted = true;
    bool isDirected = true;
    Serializer::load(loaded, "/tmp/graphlab-file-that-does-not-exist.graph", isWeighted, isDirected);

    QCOMPARE(loaded.getMapNodes().size(), std::size_t{1});
    QVERIFY(isWeighted);
    QVERIFY(isDirected);
}
