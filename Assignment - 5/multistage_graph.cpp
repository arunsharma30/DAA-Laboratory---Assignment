
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
using namespace std;

const long long INF = numeric_limits<long long>::max() / 4;

struct Edge {
    int to;
    long long cost;
};

class MultistageGraph {
    int n, stages;
    vector<vector<Edge>> adj;
    vector<int> stage;

public:
    MultistageGraph(int vertices, int k) {
        n = vertices;
        stages = k;
        adj.resize(n);
        stage.resize(n);
    }

    void setStage(int node, int s) {
        if (node >= 0 && node < n && s >= 1 && s <= stages)
            stage[node] = s;
    }

    void addEdge(int u, int v, long long w) {
        if (u < 0 || u >= n || v < 0 || v >= n || w < 0) {
            cout << "Invalid edge!\n";
            return;
        }

        if (stage[v] <= stage[u]) {
            cout << "Edge must go to a later stage!\n";
            return;
        }

        adj[u].push_back({v, w});
    }

    void updateEdge(int u, int v, long long w) {
        if (u < 0 || u >= n || v < 0 || v >= n || w < 0) {
            cout << "Invalid update!\n";
            return;
        }

        for (auto &e : adj[u]) {
            if (e.to == v) {
                e.cost = w;
                cout << "Edge updated successfully!\n";
                return;
            }
        }

        cout << "Edge not found!\n";
    }

    pair<long long, vector<int>> shortestPath(int source, int destination) {
        if (source < 0 || source >= n ||
            destination < 0 || destination >= n ||
            stage[source] >= stage[destination]) {
            return {-1, {}};
        }

        vector<long long> cost(n, INF);
        vector<int> nextNode(n, -1);

        cost[destination] = 0;

        for (int s = stage[destination] - 1; s >= stage[source]; s--) {
            for (int u = 0; u < n; u++) {
                if (stage[u] != s)
                    continue;

                for (const auto &e : adj[u]) {
                    if (stage[e.to] > stage[destination] ||
                        cost[e.to] == INF)
                        continue;

                    long long candidate = e.cost + cost[e.to];

                    if (candidate < cost[u]) {
                        cost[u] = candidate;
                        nextNode[u] = e.to;
                    }
                }
            }
        }

        if (cost[source] == INF)
            return {-1, {}};

        vector<int> path;
        int current = source;

        while (current != -1) {
            path.push_back(current);

            if (current == destination)
                break;

            current = nextNode[current];
        }

        if (path.empty() || path.back() != destination)
            return {-1, {}};

        return {cost[source], path};
    }

    void showPath(int source, int destination) {
        auto result = shortestPath(source, destination);

        if (result.first == -1) {
            cout << "No valid path exists from "
                 << source << " to " << destination << ".\n";
            return;
        }

        cout << "Minimum delivery cost: " << result.first << '\n';
        cout << "Optimal route: ";

        for (int i = 0; i < (int)result.second.size(); i++) {
            cout << result.second[i];

            if (i + 1 < (int)result.second.size())
                cout << " -> ";
        }

        cout << '\n';
    }
};

int main() {
    int n, k, m;

    cout << "===== SWIFTCARGO MULTISTAGE ROUTING =====\n";

    cout << "Enter number of cities: ";
    cin >> n;

    cout << "Enter number of stages: ";
    cin >> k;

    if (!cin || n < 2 || k < 2 || k > n) {
        cout << "Invalid number of cities or stages!\n";
        return 1;
    }

    MultistageGraph graph(n, k);

    cout << "\nEnter the stage number for each city (1 to "
         << k << "):\n";

    for (int i = 0; i < n; i++) {
        int s;
        cout << "City " << i << ": ";
        cin >> s;

        if (!cin || s < 1 || s > k) {
            cout << "Invalid stage!\n";
            return 1;
        }

        graph.setStage(i, s);
    }

    cout << "\nEnter number of directed routes: ";
    cin >> m;

    if (!cin || m < 0) {
        cout << "Invalid number of routes!\n";
        return 1;
    }

    cout << "Enter routes (source destination cost):\n";

    for (int i = 0; i < m; i++) {
        int u, v;
        long long w;
        cin >> u >> v >> w;

        if (!cin || u < 0 || u >= n ||
            v < 0 || v >= n || w < 0) {
            cout << "Invalid route input!\n";
            return 1;
        }

        graph.addEdge(u, v, w);
    }

    int source, destination;

    cout << "\nEnter source city: ";
    cin >> source;

    cout << "Enter destination city: ";
    cin >> destination;

    cout << "\n===== SHORTEST DELIVERY ROUTE =====\n";
    graph.showPath(source, destination);

    int q;
    cout << "\nEnter number of additional delivery requests: ";
    cin >> q;

    if (!cin || q < 0) {
        cout << "Invalid number of requests!\n";
        return 1;
    }

    for (int i = 0; i < q; i++) {
        cout << "\nRequest " << i + 1 << '\n';

        cout << "Enter source and destination: ";
        cin >> source >> destination;

        graph.showPath(source, destination);
    }

    char choice;

    cout << "\nDo you want to update a route? (y/n): ";
    cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        int u, v;
        long long w;

        cout << "Enter source, destination and new cost: ";
        cin >> u >> v >> w;

        graph.updateEdge(u, v, w);

        cout << "\n===== UPDATED DELIVERY ROUTE =====\n";
        graph.showPath(source, destination);
    }

    return 0;
}