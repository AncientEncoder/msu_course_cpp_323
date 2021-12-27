#include "GraphTraverser.hpp"
#include <algorithm>
#include <atomic>
#include <cassert>
#include <climits>
#include <functional>
#include <list>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include "GraphPath.hpp"
namespace {
const int MAX_WORKERS_COUNT = std::thread::hardware_concurrency();
using GraphPath = uni_course_cpp::GraphPath;
constexpr int MAX_DISTANCE = INT_MAX;
constexpr int UNDEFINED_ID = -1;
std::vector<int> get_path_vertex_ids(
    int target_vertex_id,
    const std::unordered_map<int, int>& relations) {
  auto current_id = target_vertex_id;
  std::list<int> vertex_ids = {current_id};
  while (relations.find(current_id) != relations.end()) {
    current_id = relations.at(current_id);
    vertex_ids.push_front(current_id);
  }
  return std::vector<int>(vertex_ids.begin(), vertex_ids.end());
}
uni_course_cpp::GraphPath get_path(
    int target_vertex_id,
    const std::unordered_map<int, int>& relations,
    Graph graph) {
  const auto vertex_ids = get_path_vertex_ids(target_vertex_id, relations);

  std::vector<int> edge_ids;
  edge_ids.reserve(vertex_ids.size() - 1);

  for (auto it = vertex_ids.cbegin(); it != (vertex_ids.cend() - 1); it++) {
    const auto from_vertex_id = *it;
    const auto to_vertex_id = *(it + 1);
    int edge_id = graph.getEdgeId(from_vertex_id, to_vertex_id);
    edge_ids.push_back(edge_id);
  }

  return GraphPath(vertex_ids, edge_ids);
}
}  // namespace
using uni_course_cpp::GraphPath;
namespace uni_course_cpp {
GraphPath GraphTraverser::find_shortest_path(int source_vertex_id,
                                             int target_vertex_id) const {
  assert(graph_.hasVertex(source_vertex_id) &&
         "Graph doesn't have provided source vertex");
  assert(graph_.hasVertex(target_vertex_id) &&
         "Graph doesn't have provided target vertex");

  std::list<int> frontier;
  std::unordered_map<int, int> distances;
  std::unordered_map<int, int> relations;

  distances[source_vertex_id] = 0;
  frontier.push_back(source_vertex_id);

  while (frontier.size() > 0) {
    const auto active_vertex_id = frontier.front();
    frontier.pop_front();

    const auto& connections = graph_.getConnectedVertex(active_vertex_id);

    for (const auto& [edge_id, vertex_id] : connections) {
      const auto distance = distances.at(active_vertex_id) + 1;

      if (distances.find(vertex_id) != distances.end() &&
          distance >= distances.at(vertex_id)) {
        continue;
      }

      distances[vertex_id] = distance;
      relations[vertex_id] = active_vertex_id;
      frontier.push_back(vertex_id);
    }
  }

  return get_path(target_vertex_id, relations, graph_);
}
std::vector<GraphPath> GraphTraverser::find_all_paths() const {
  using JobCallback = std::function<void()>;
  auto jobs = std::list<JobCallback>();

  std::mutex jobs_mutex;
  std::mutex paths_mutex;
  std::atomic<int> finished_jobs_num = 0;
  std::atomic<bool> should_terminate = false;

  auto last_depth_vertex_ids = graph_.getVertexIdsAtDepth(graph_.getDepth());

  std::vector<GraphPath> paths;
  paths.reserve(last_depth_vertex_ids.size());

  for (const auto& vertex_id : last_depth_vertex_ids) {
    jobs.push_back(
        [&paths, &paths_mutex, &vertex_id, &finished_jobs_num, this]() {
          GraphPath path = find_shortest_path(0, vertex_id);
          {
            std::lock_guard lock(paths_mutex);
            paths.push_back(std::move(path));
          }
          finished_jobs_num++;
        });
  }

  const auto worker = [&should_terminate, &jobs_mutex, &jobs]() {
    while (true) {
      if (should_terminate) {
        return;
      }
      const auto job_optional = [&jobs,
                                 &jobs_mutex]() -> std::optional<JobCallback> {
        const std::lock_guard lock(jobs_mutex);
        if (!jobs.empty()) {
          const auto job = jobs.front();
          jobs.pop_front();
          return job;
        }
        return std::nullopt;
      }();
      if (job_optional.has_value()) {
        const auto& job = job_optional.value();
        job();
      }
    }
  };

  const auto threads_count =
      std::min(MAX_WORKERS_COUNT, (int)last_depth_vertex_ids.size());
  auto threads = std::vector<std::thread>();
  threads.reserve(threads_count);

  for (int i = 0; i < threads_count; i++) {
    threads.push_back(std::thread(worker));
  }

  while (finished_jobs_num < last_depth_vertex_ids.size()) {
  }

  should_terminate = true;
  for (auto& thread : threads) {
    thread.join();
  }

  return paths;
}

}  // namespace uni_course_cpp
