#include "Graph.hpp"
#include "GraphPath.hpp"
#include "Logger.hpp"
namespace uni_course_cpp {
class LoggingHelper {
 public:
  static std::string getTimeByString();
  static void logStart(Logger& logger, int graphIndex);
  static void logColors(Logger& logger, Graph graph);
  static void logEnd(Logger& logger, Graph graph, int graphIndex);
  static void startTravel(Logger& logger, int index);
  static void endTravel(int index,
                        const std::vector<GraphPath>& paths,
                        Logger& logger);
};
}  // namespace uni_course_cpp
