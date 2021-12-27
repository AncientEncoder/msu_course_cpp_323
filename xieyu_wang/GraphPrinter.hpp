#include "Edge.hpp"
#include "Graph.hpp"
#include "GraphPath.hpp"
namespace uni_course_cpp {
class GraphPrinter {
 public:
  static std::string printGraph(const uni_course_cpp::Graph& graph);
  static std::string printVertex(const uni_course_cpp::Vertex& vertex);
  static std::string printEdge(const uni_course_cpp::Edge& edge);
  static std::string printEdgeColor(const uni_course_cpp::Edge::Color& color);
  static std::string printPath(const GraphPath& path);
};

}  // namespace uni_course_cpp
