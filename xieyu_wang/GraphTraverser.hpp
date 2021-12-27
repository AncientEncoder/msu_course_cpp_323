#pragma once

#include "Graph.hpp"
#include "GraphPath.hpp"
namespace {
    using uni_course_cpp::Graph;
}
namespace uni_course_cpp {


    class GraphTraverser {
    public:
        GraphTraverser( Graph graph) : graph_(graph) {}

        GraphPath find_shortest_path(const int& source_vertex_id,
                                     const int& destination_vertex_id) const;
        std::vector<GraphPath> find_all_paths() const;

    private:
         Graph& graph_;
    };

}  // namespace uni_course_cpp