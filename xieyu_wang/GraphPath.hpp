#pragma once

#include <vector>

namespace uni_course_cpp {
struct GraphPath {
 public:
  using Distance = int;

  GraphPath(const std::vector<int>& vertex_ids,
            const std::vector<int>& edge_ids)
      : vertex_ids_(vertex_ids), edge_ids_(edge_ids) {}

  Distance distance() const { return vertex_ids_.size() - 1; }

  const std::vector<int>& get_vertex_ids() const { return vertex_ids_; }

 private:
  std::vector<int> vertex_ids_;
  std::vector<int> edge_ids_;
};

}  // namespace uni_course_cpp
