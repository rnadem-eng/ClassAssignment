#include <algorithm>
#include <cstddef>
#include <iostream>
#include <queue>
#include <string_view>
#include <vector>

using Vertex = std::size_t;
using Graph = std::vector<std::vector<Vertex>>;

constexpr std::string_view labels = "ABCDEFGH";
constexpr Vertex A = 0, B = 1, C = 2, D = 3, E = 4, F = 5, G = 6, H = 7;

Graph buildCampus(bool closeCE = false, bool closeEF = false)
{
    Graph graph(labels.size());
    const auto addWalkway = [&graph](Vertex u, Vertex v)
    {
        graph[u].push_back(v);
        graph[v].push_back(u);
    };

    addWalkway(A, B);
    addWalkway(A, C);
    addWalkway(B, D);
    if (!closeCE)
        addWalkway(C, E);
    addWalkway(D, E);
    if (!closeEF)
        addWalkway(E, F);
    addWalkway(B, G);
    addWalkway(G, E);
    addWalkway(C, H);
    addWalkway(H, D);

    for (auto &neighbors : graph)
        std::sort(neighbors.begin(), neighbors.end());

    return graph;
}

// Call after BFS has reached goal. Follow parent links back to start.
std::vector<Vertex> rebuildRoute(const std::vector<Vertex> &parent,
                                 Vertex start, Vertex goal)
{
    std::vector<Vertex> route;
    for (Vertex current = goal; current != start; current = parent[current])
        route.push_back(current);

    route.push_back(start);
    std::reverse(route.begin(), route.end());
    return route;
}

// start and goal are valid vertex IDs. Return an empty vector if unreachable.
std::vector<Vertex> shortestRoute(const Graph &graph, Vertex start, Vertex goal)
{
    std::queue<Vertex> pending;
    std::vector<bool> seen(graph.size(), false);
    std::vector<Vertex> parent(graph.size(), graph.size());

    // Start at the first building.
    parent[start] = start;
    seen[start] = true;
    pending.push(start);

    while (!pending.empty())
    {
        Vertex current = pending.front();
        pending.pop();

        if (current == goal)
        {
            return rebuildRoute(parent, start, goal);
        }

        // Check each building connected to the current one.
        for (Vertex next : graph[current])
        {
            if (!seen[next])
            {
                seen[next] = true;
                parent[next] = current;
                pending.push(next);
            }
        }
    }

    return {}; // No route to the goal.
}

bool runCase(std::string_view name, const Graph &graph, Vertex start, Vertex goal,
             const std::vector<Vertex> &expected)
{
    const auto route = shortestRoute(graph, start, goal);
    std::cout << name << ": ";
    if (route.empty())
    {
        std::cout << "no route";
    }
    else
    {
        for (std::size_t i = 0; i < route.size(); ++i)
        {
            if (i > 0)
                std::cout << " -> ";
            std::cout << labels[route[i]];
        }
        const auto walkways = route.size() - 1;
        std::cout << " (" << walkways << (walkways == 1 ? " walkway)" : " walkways)");
    }

    const bool passed = route == expected;
    std::cout << (passed ? " [PASS]\n" : " [CHECK]\n");
    return passed;
}

int main()
{
    int failures = 0;
    failures += !runCase("All walkways open", buildCampus(), A, F, {A, C, E, F});
    failures += !runCase("C-E closed", buildCampus(true), A, F, {A, B, D, E, F});
    failures += !runCase("E-F closed", buildCampus(false, true), A, F, {});
    failures += !runCase("Already at A", buildCampus(), A, A, {A});
    return failures == 0 ? 0 : 1;
}
