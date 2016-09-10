#include <array>
#include <type_traits>
#include <iostream>

using vertex = int;

template<int N>
struct proxy {
    friend constexpr vertex parent_of(proxy<N>);
    friend constexpr vertex queue(proxy<N>);
};

template<vertex N> struct edges;

template<vertex N, vertex M>
struct set_parent_of {
    friend constexpr vertex parent_of(proxy<N>) { return M; }
};

template<int N, vertex M>
struct enqueue {
    friend constexpr vertex queue(proxy<N>) { return M; }
};

template<vertex self, vertex parent, int queue_len, vertex = parent_of(proxy<self>{})>
constexpr int visit(int) {
    return queue_len;
}

template<vertex self, vertex parent, int queue_len>
constexpr int visit(float) {
    (void) sizeof(enqueue<queue_len, self>);
    (void) sizeof(set_parent_of<self, parent>);
    return queue_len + 1;
};

template<vertex self, int pos, int queue_len>
constexpr typename std::enable_if<(pos < 0), int>::type visit_neighbours() {
    return queue_len;
}

template<vertex self, int pos, int queue_len>
constexpr typename std::enable_if<(pos >= 0), int>::type visit_neighbours() {
    constexpr const auto neighbour = edges<self>::value[pos];
    constexpr const auto new_queue_len = visit<neighbour, self, queue_len>(0);
    return visit_neighbours<self, pos - 1, new_queue_len>();
}

template<int queue_pos, int queue_len>
constexpr void process(float) { }

template<int queue_pos, int queue_len>
constexpr typename std::enable_if<(queue_pos < queue_len), void>::type process(int) {
    constexpr const auto self = queue(proxy<queue_pos>{});
    constexpr const auto new_queue_len = visit_neighbours<self, edges<self>::value.size() - 1, queue_len>(); 
    process<queue_pos + 1, new_queue_len>(0);
}

template<vertex from>
constexpr void bfs() {
    (void) sizeof(enqueue<0, from>);
    (void) sizeof(set_parent_of<from, 0>);
    process<0, 1>(0);
}

// ---------------------- EXAMPLE ----------------------

template<vertex N>
struct edges {
    static constexpr const std::array<vertex, 0> value{{ }};
};

template<>
struct edges<1> {
    static constexpr const std::array<vertex, 5> value{{5, 2, 3, 1, 7}};
};

template<>
struct edges<2> {
    static constexpr const std::array<vertex, 3> value{{3, 4, 5}};
};

template<>
struct edges<5> {
    static constexpr const std::array<vertex, 3> value{{6, 1, 7}};
};

void show_till(proxy<0>) { }

template<vertex till>
void show_till(proxy<till>) {
    show_till(proxy<till - 1>{});
    std::cout << "Parent of " << till << ": " << parent_of(proxy<till>{}) << std::endl;
}

int main() {
    bfs<1>();
    show_till(proxy<7>{});
}
