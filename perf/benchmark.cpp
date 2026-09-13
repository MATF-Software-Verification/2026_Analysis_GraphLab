#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "algorithm/Dijkstra/dijkstra.hpp"
#include "algorithm/Floyd_Warshall/floyd_warshall.hpp"
#include "graph/graph.hpp"

namespace {

std::unique_ptr<graph::WeightedDirectedGraph> makeGraph(std::size_t nodeCount)
{
    auto graph = std::make_unique<graph::WeightedDirectedGraph>();

    for (std::size_t i = 0; i < nodeCount; ++i) {
        graph->add_node({static_cast<double>(i), 0.0}, std::to_string(i));
    }

    for (std::size_t i = 0; i + 1 < nodeCount; ++i) {
        graph->add_edge(i, i + 1, 1.0);
        if (i + 7 < nodeCount) {
            graph->add_edge(i, i + 7, 3.0);
        }
        if (i + 31 < nodeCount) {
            graph->add_edge(i, i + 31, 8.0);
        }
    }

    return graph;
}

std::uint64_t runDijkstra(std::size_t nodeCount, unsigned repetitions)
{
    const auto graph = makeGraph(nodeCount);
    std::uint64_t checksum = 0;

    for (unsigned i = 0; i < repetitions; ++i) {
        algorithm::Dijkstra dijkstra(*graph);
        const auto result = dijkstra.run({0, static_cast<graph::node_id_t>(nodeCount - 1)});
        checksum += result.first.size();
        checksum += static_cast<std::uint64_t>(result.second);
    }

    return checksum;
}

std::uint64_t runFloydWarshall(std::size_t nodeCount, unsigned repetitions)
{
    const auto graph = makeGraph(nodeCount);
    std::uint64_t checksum = 0;

    for (unsigned i = 0; i < repetitions; ++i) {
        algorithm::FloydWarshall floydWarshall(*graph);
        const auto result = floydWarshall.run(std::nullopt);
        checksum += static_cast<std::uint64_t>(result.first.front().back());
        checksum += result.second.front().back().size();
    }

    return checksum;
}

std::size_t parseSize(const char *value)
{
    const auto parsed = std::stoul(value);
    if (parsed < 2) {
        throw std::invalid_argument("node count must be at least 2");
    }
    return parsed;
}

unsigned parseRepetitions(const char *value)
{
    const auto parsed = std::stoul(value);
    if (parsed == 0) {
        throw std::invalid_argument("repetitions must be positive");
    }
    return static_cast<unsigned>(parsed);
}

} // namespace

int main(int argc, char **argv)
{
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0]
                  << " <dijkstra|floyd-warshall> <node-count> <repetitions>\n";
        return EXIT_FAILURE;
    }

    try {
        const std::string workload = argv[1];
        const auto nodeCount = parseSize(argv[2]);
        const auto repetitions = parseRepetitions(argv[3]);
        const auto started = std::chrono::steady_clock::now();

        std::uint64_t checksum = 0;
        if (workload == "dijkstra") {
            checksum = runDijkstra(nodeCount, repetitions);
        } else if (workload == "floyd-warshall") {
            checksum = runFloydWarshall(nodeCount, repetitions);
        } else {
            throw std::invalid_argument("unknown workload: " + workload);
        }

        const auto elapsed = std::chrono::duration<double>(
            std::chrono::steady_clock::now() - started);

        std::cout << "workload=" << workload << '\n'
                  << "nodes=" << nodeCount << '\n'
                  << "repetitions=" << repetitions << '\n'
                  << "checksum=" << checksum << '\n'
                  << "elapsed_seconds=" << elapsed.count() << '\n';
    } catch (const std::exception &error) {
        std::cerr << "Benchmark error: " << error.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
