
#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <algorithm>
#include <utility>
#include <functional>
using namespace std;

class TrafficNetwork {
private:
    int V;
    vector<vector<pair<int, int>>> adj;
    vector<bool> isHospital;

public:
    // Constructor
    TrafficNetwork(int vertices) {
        V = vertices;
        adj.resize(V);
        isHospital.resize(V, false);
    }

    // Add bidirectional road
    void addEdge(int u, int v, int w) {
        if (u < 0 || u >= V || v < 0 || v >= V || w < 0) {
            cout << "Invalid road details!\n";
            return;
        }

        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }

    // Add hospital location
    void addHospital(int node) {
        if (node < 0 || node >= V) {
            cout << "Invalid hospital location!\n";
            return;
        }

        isHospital[node] = true;
    }

    // Update traffic time of a road
    void updateEdgeWeight(int u, int v, int newWeight) {
        if (u < 0 || u >= V || v < 0 || v >= V ||
            newWeight < 0) {
            cout << "Invalid road update!\n";
            return;
        }

        bool found = false;

        for (auto &edge : adj[u]) {
            if (edge.first == v) {
                edge.second = newWeight;
                found = true;
            }
        }

        for (auto &edge : adj[v]) {
            if (edge.first == u) {
                edge.second = newWeight;
                found = true;
            }
        }

        if (found)
            cout << "Traffic updated successfully!\n";
        else
            cout << "Road not found!\n";
    }

    // Dijkstra's Algorithm
    pair<long long, vector<int>>
    shortestPathToNearestHospital(int source) {

        if (source < 0 || source >= V)
            return {-1, {}};

        const long long INF = LLONG_MAX;

        vector<long long> dist(V, INF);
        vector<int> parent(V, -1);

        priority_queue<
            pair<long long, int>,
            vector<pair<long long, int>>,
            greater<pair<long long, int>>
        > pq;

        dist[source] = 0;
        pq.push({0, source});

        while (!pq.empty()) {
            long long curDist = pq.top().first;
            int u = pq.top().second;
            pq.pop();

            if (curDist > dist[u])
                continue;

            // Stop when nearest hospital is reached
            if (isHospital[u]) {
                vector<int> path;

                int v = u;

                while (v != -1) {
                    path.push_back(v);
                    v = parent[v];
                }

                reverse(path.begin(), path.end());

                return {dist[u], path};
            }

            // Explore neighboring intersections
            for (auto edge : adj[u]) {
                int v = edge.first;
                int w = edge.second;

                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    parent[v] = u;

                    pq.push({dist[v], v});
                }
            }
        }

        // No reachable hospital
        return {-1, {}};
    }
};

// Display shortest route
void displayResult(pair<long long, vector<int>> result) {

    if (result.first == -1) {
        cout << "No hospital reachable!\n";
        return;
    }

    cout << "Shortest Travel Time: "
         << result.first << " minutes\n";

    cout << "Optimal Path: ";

    for (int i = 0; i < (int)result.second.size(); i++) {
        cout << result.second[i];

        if (i != (int)result.second.size() - 1)
            cout << " -> ";
    }

    cout << endl;

    cout << "Nearest Hospital: "
         << result.second.back() << endl;
}

int main() {

    int V, E;

    cout << "===== SMART CITY AMBULANCE ROUTING SYSTEM =====\n";

    cout << "\nEnter number of intersections: ";
    cin >> V;

    if (!cin || V <= 0) {
        cout << "Invalid number of intersections!\n";
        return 1;
    }

    TrafficNetwork city(V);

    cout << "Enter number of roads: ";
    cin >> E;

    if (!cin || E < 0) {
        cout << "Invalid number of roads!\n";
        return 1;
    }

    cout << "\nEnter roads (source destination travel time):\n";
    cout << "Intersection numbers: 0 to " << V - 1 << "\n";

    for (int i = 0; i < E; i++) {
        int u, v, w;
        cin >> u >> v >> w;

        if (!cin || u < 0 || u >= V ||
            v < 0 || v >= V || w < 0) {
            cout << "Invalid road input!\n";
            return 1;
        }

        city.addEdge(u, v, w);
    }

    int numHospitals;

    cout << "\nEnter number of hospitals: ";
    cin >> numHospitals;

    if (!cin || numHospitals < 1 || numHospitals > V) {
        cout << "Invalid hospital count!\n";
        return 1;
    }

    cout << "Enter hospital locations:\n";

    for (int i = 0; i < numHospitals; i++) {
        int node;
        cin >> node;

        if (!cin || node < 0 || node >= V) {
            cout << "Invalid hospital node!\n";
            return 1;
        }

        city.addHospital(node);
    }

    int ambulanceLocation;

    cout << "\nEnter ambulance starting location: ";
    cin >> ambulanceLocation;

    if (!cin || ambulanceLocation < 0 ||
        ambulanceLocation >= V) {
        cout << "Invalid ambulance location!\n";
        return 1;
    }

    // Initial shortest path
    cout << "\n===== INITIAL SHORTEST PATH =====\n";

    auto result =
        city.shortestPathToNearestHospital(ambulanceLocation);

    displayResult(result);

    // Real-time traffic update
    int u, v, newWeight;

    cout << "\n===== REAL-TIME TRAFFIC UPDATE =====\n";

    cout << "Enter road to update (u v newTravelTime): ";
    cin >> u >> v >> newWeight;

    if (!cin) {
        cout << "Invalid update input!\n";
        return 1;
    }

    city.updateEdgeWeight(u, v, newWeight);

    // Recompute shortest path
    cout << "\n===== UPDATED SHORTEST PATH =====\n";

    auto newResult =
        city.shortestPathToNearestHospital(ambulanceLocation);

    displayResult(newResult);

    return 0;
}