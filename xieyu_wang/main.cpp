#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include "GraphGenerator.hpp"
#include "GraphGeneratorController.hpp"
#include "GraphPrinter.hpp"
#include "Logger.hpp"
#include "LoggingHelper.hpp"
#include "GraphTraversalController.hpp"
using GraphGenerationController = uni_course_cpp::GraphGenerationController;
using GraphGenerator = uni_course_cpp::GraphGenerator;
using Logger = uni_course_cpp::Logger;
using Graph = uni_course_cpp::Graph;
using Edge = uni_course_cpp::Edge;
using LoggingHelper = uni_course_cpp::LoggingHelper;
using GraphPrinter = uni_course_cpp::GraphPrinter;
using GraphTraversalController=uni_course_cpp::GraphGenerationController;
int ctrlMaxDepthEntry() {
  int maxDepth = 0;
  std::cout << "Enter Max Depth:";
  std::cin >> maxDepth;
  if (maxDepth < 0) {
    while (maxDepth < 0) {
      std::cout << "Error Max Depth must be  above or equal 0: ";
      std::cin >> maxDepth;
    }
  }
  return maxDepth;
}
int ctrlNewVertexNum() {
  int newVerticesNum = 0;
  std::cout << "Enter new vertices num:";
  std::cin >> newVerticesNum;
  if (newVerticesNum < 0) {
    while (newVerticesNum < 0) {
      std::cout << "Error new Vertices number must be above or equal 0: ";
      std::cin >> newVerticesNum;
    }
  }
  return newVerticesNum;
}
int ctrlNewGraphNum() {
  int newGraphNum = 0;
  std::cout << "Enter new Graph num:";
  std::cin >> newGraphNum;
  if (newGraphNum < 0) {
    while (newGraphNum < 0) {
      std::cout << "Error new Vertices number must be above or equal 0: ";
      std::cin >> newGraphNum;
    }
  }
  return newGraphNum;
}
int ctrlThreadNum() {
  int threadNum = 0;
  std::cout << "Enter Thread Num:";
  std::cin >> threadNum;
  if (threadNum < 0) {
    while (threadNum < 0) {
      std::cout << "Error Thread number must be above or equal 0: ";
      std::cin >> threadNum;
    }
  }
  return threadNum;
}
void writeGraphToFile(const Graph& graph, int index) {
  std::ofstream writePT;
  writePT.open("./temp/Graph_" + std::to_string(index) + ".json",
               std::ios::out);
  writePT << GraphPrinter::printGraph(graph) << std::endl;
  writePT.close();
}
std::map<int, Graph> generateGraphs(const GraphGenerator::Params& params,
                                    int graphsCount,
                                    int threadsNum) {
  auto generationController =
      GraphGenerationController(threadsNum, graphsCount, params);

  auto& logger = Logger::getLogger();

  auto graphs = std::map<int, Graph>();

  generationController.generate(
      [&logger](int index) { LoggingHelper ::logStart(logger, index); },
      [&logger, &graphs, &params](int index, Graph graph) {
        LoggingHelper ::logEnd(logger, graph, index);
        graphs.emplace(index, std::move(graph));
        writeGraphToFile(graph, index);
      });

  return graphs;
}
void traverse_graphs(std::map<int, Graph> graphs, int numThread, int numGraph) {
    auto traversal_controller = GraphGenerationController(graphs);
    auto& logger = Logger::getLogger();

    traversal_controller.traverse(
            [&logger](int index, const Graph& graph) {
                LoggingHelper ::startTravel(logger,index);
            },
            [&logger](int index, const Graph& graph, std::vector<uni_course_cpp::GraphPath> paths) {
                LoggingHelper ::endTravel(index, paths,logger);
            });
}
int main() {
  std::filesystem::create_directory("./temp");
  Logger& logger = Logger::getLogger();
  const int maxDepth = ctrlMaxDepthEntry();
  const int newVerticesNum = ctrlNewVertexNum();
  const int newGraphNum = ctrlNewGraphNum();
  const int threadNum = ctrlThreadNum();
  const GraphGenerator::Params params(maxDepth, newVerticesNum);
    traverse_graphs(generateGraphs(params, newGraphNum, threadNum),threadNum,newGraphNum);
  return 0;
}
