#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using std::endl;
using std::to_string;
using std::vector;

using EdgeId = int;
using VertexId = int;

constexpr int START_VERTICES_NUMBER = 14;
constexpr int INVALID_ID = -1;
const std::string JSON_GRAPH_FILENAME = "graph.json";

struct Edge {
  const EdgeId id = INVALID_ID;
  const std::array<VertexId, 2> connected_vertices;

  Edge(const VertexId& start, const VertexId& end, const EdgeId& _id)
      : id(_id), connected_vertices({start, end}) {}

  std::string to_json() const {
    std::string res;
    res = "{ \"id\": ";
    res += to_string(id);
    res += ", \"vertex_ids\": [";
    res += to_string(connected_vertices[0]);
    res += ", ";
    res += to_string(connected_vertices[1]);
    res += "] }";
    return res;
  }
};

bool is_edge_id_included(const EdgeId& id, const vector<EdgeId>& edge_ids) {
  for (const auto& edge_id : edge_ids)
    if (id == edge_id)
      return true;
  return false;
}

struct Vertex {
 public:
  const VertexId id = INVALID_ID;

  explicit Vertex(const VertexId& _id) : id(_id) {}

  std::string to_json() const {
    std::string res;
    res = "{ \"id\": ";
    res += to_string(id) + ", \"edge_ids\": [";
    for (const auto& edge_id : edges_ids_) {
      res += to_string(edge_id);
      res += ", ";
    }
    res.pop_back();
    res.pop_back();
    res += "] }";
    return res;
  }

  void add_edge_id(const EdgeId& _id) {
    assert(!is_edge_id_included(_id, edges_ids_));
    edges_ids_.push_back(_id);
  }

  const vector<EdgeId>& get_edges_ids() const { return edges_ids_; }

 private:
  std::vector<EdgeId> edges_ids_;
};

class Graph {
 public:
  std::string to_json() const {
    std::string res;
    res = "{ \"vertices\": [ ";
    for (const auto& v_it : vertices_) {
      res += v_it.to_json();
      res += ", ";
    }
    res.pop_back();
    res.pop_back();
    res += " ], \"edges\": [ ";
    for (const auto& e_it : edges_) {
      res += e_it.to_json();
      res += ", ";
    }
    res.pop_back();
    res.pop_back();
    res += " ] }\n";
    return res;
  }

  void add_vertex() { vertices_.emplace_back(get_next_vertex_id()); }

  bool is_vertex_exist(const VertexId& vertex_id) const {
    for (const auto& vertex : vertices_) {
      if (vertex_id == vertex.id)
        return true;
    }
    return false;
  }

  bool is_connected(const VertexId& from_vertex_id,
                    const VertexId& to_vertex_id) const {
    assert(is_vertex_exist(from_vertex_id));
    assert(is_vertex_exist(to_vertex_id));

    const auto& from_vertex_edges_ids =
        vertices_[from_vertex_id].get_edges_ids();
    const auto& to_vertex_edges_ids = vertices_[to_vertex_id].get_edges_ids();
    for (const auto& from_vertex_edge_id : from_vertex_edges_ids) {
      for (const auto& to_vertex_edge_id : to_vertex_edges_ids) {
        if (from_vertex_edge_id == to_vertex_edge_id)
          return true;
      }
    }
    return false;
  }

  void connect_vertices(const VertexId& from_vertex_id,
                        const VertexId& to_vertex_id) {
    // check if vertices exist
    assert(is_vertex_exist(from_vertex_id));
    assert(is_vertex_exist(to_vertex_id));

    // check if they are not connected
    assert(!is_connected(from_vertex_id, to_vertex_id));

    const auto& new_edge =
        edges_.emplace_back(from_vertex_id, to_vertex_id, get_next_edge_id());

    // add information into Vertex structure
    vertices_[from_vertex_id].add_edge_id(new_edge.id);
    vertices_[to_vertex_id].add_edge_id(new_edge.id);
  }

 private:
  vector<Vertex> vertices_;
  vector<Edge> edges_;
  VertexId vertex_id_counter_ = 0;
  EdgeId edge_id_counter_ = 0;

  VertexId get_next_vertex_id() { return vertex_id_counter_++; }

  VertexId get_next_edge_id() { return edge_id_counter_++; }
};

void write_graph(const Graph& graph) {
  std::ofstream out;
  out.open(JSON_GRAPH_FILENAME, std::ofstream::out | std::ofstream::trunc);

  out << graph.to_json();

  out.close();
}

int main() {
  Graph new_graph;
  for (int i = 0; i < START_VERTICES_NUMBER; i++) {
    new_graph.add_vertex();
  }
  new_graph.connect_vertices(0, 1);
  new_graph.connect_vertices(0, 2);
  new_graph.connect_vertices(0, 3);
  new_graph.connect_vertices(1, 4);
  new_graph.connect_vertices(1, 5);
  new_graph.connect_vertices(1, 6);
  new_graph.connect_vertices(2, 7);
  new_graph.connect_vertices(2, 8);
  new_graph.connect_vertices(3, 9);
  new_graph.connect_vertices(4, 10);
  new_graph.connect_vertices(5, 10);
  new_graph.connect_vertices(6, 10);
  new_graph.connect_vertices(7, 11);
  new_graph.connect_vertices(8, 11);
  new_graph.connect_vertices(9, 12);
  new_graph.connect_vertices(10, 13);
  new_graph.connect_vertices(11, 13);
  new_graph.connect_vertices(12, 13);

  write_graph(new_graph);

  return 0;
}
