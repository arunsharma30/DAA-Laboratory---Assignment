
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>
using namespace std;

const long long INF = numeric_limits<long long>::max() / 4;

struct Node {
    vector<vector<long long>> matrix;
    vector<int> path;
    vector<bool> visited;
    long long bound;
    long long actualCost;
};

struct Compare {
    bool operator()(const Node& a, const Node& b) const {
        return a.bound > b.bound;
    }
};

class TSPBranchBound {
private:
    int n;
    vector<vector<long long>> cost;

    long long reduceMatrix(vector<vector<long long>>& matrix) {
        long long reductionCost = 0;

        // Row reduction
        for (int i = 0; i < n; i++) {
            long long minimum = INF;

            for (int j = 0; j < n; j++)
                minimum = min(minimum, matrix[i][j]);

            if (minimum != INF && minimum > 0) {
                reductionCost += minimum;

                for (int j = 0; j < n; j++) {
                    if (matrix[i][j] != INF)
                        matrix[i][j] -= minimum;
                }
            }
        }

        // Column reduction
        for (int j = 0; j < n; j++) {
            long long minimum = INF;

            for (int i = 0; i < n; i++)
                minimum = min(minimum, matrix[i][j]);

            if (minimum != INF && minimum > 0) {
                reductionCost += minimum;

                for (int i = 0; i < n; i++) {
                    if (matrix[i][j] != INF)
                        matrix[i][j] -= minimum;
                }
            }
        }

        return reductionCost;
    }

public:
    TSPBranchBound(int vertices, vector<vector<long long>> matrix) {
        n = vertices;
        cost = matrix;
    }

    void solve(int start) {
        if (start < 0 || start >= n) {
            cout << "Invalid starting city!\n";
            return;
        }

        if (n < 2) {
            cout << "At least 2 cities are required.\n";
            return;
        }

        // Diagonal entries are not valid travel edges
        for (int i = 0; i < n; i++)
            cost[i][i] = INF;

        vector<vector<long long>> reduced = cost;
        long long initialBound = reduceMatrix(reduced);

        Node root;
        root.matrix = reduced;
        root.path = {start};
        root.visited.assign(n, false);
        root.visited[start] = true;
        root.bound = initialBound;
        root.actualCost = 0;

        priority_queue<Node, vector<Node>, Compare> pq;
        pq.push(root);

        long long bestCost = INF;
        vector<int> bestPath;

        while (!pq.empty()) {
            Node current = pq.top();
            pq.pop();

            if (current.bound >= bestCost)
                continue;

            int u = current.path.back();

            // Only one city remains to be visited
            if ((int)current.path.size() == n - 1) {
                for (int v = 0; v < n; v++) {
                    if (current.visited[v])
                        continue;

                    if (cost[u][v] == INF ||
                        cost[v][start] == INF)
                        continue;

                    long long total =
                        current.actualCost +
                        cost[u][v] +
                        cost[v][start];

                    if (total < bestCost) {
                        bestCost = total;
                        bestPath = current.path;
                        bestPath.push_back(v);
                        bestPath.push_back(start);
                    }
                }

                continue;
            }

            // Expand the current node
            for (int v = 0; v < n; v++) {
                if (current.visited[v] || cost[u][v] == INF)
                    continue;

                long long edgeCost = current.matrix[u][v];

                if (edgeCost == INF)
                    continue;

                Node child = current;

                child.actualCost += cost[u][v];
                child.path.push_back(v);
                child.visited[v] = true;

                // Prevent leaving the current city again
                for (int j = 0; j < n; j++)
                    child.matrix[u][j] = INF;

                // Prevent entering the selected city again
                for (int i = 0; i < n; i++)
                    child.matrix[i][v] = INF;

                // Do not return to the starting city too early
                child.matrix[v][start] = INF;

                long long reduction =
                    reduceMatrix(child.matrix);

                child.bound =
                    current.bound + edgeCost + reduction;

                if (child.bound < bestCost)
                    pq.push(child);
            }
        }

        cout << "\n===== TSP OPTIMAL SOLUTION =====\n";

        if (bestPath.empty()) {
            cout << "No valid Hamiltonian cycle exists.\n";
            return;
        }

        cout << "Minimum Tour Cost: " << bestCost << '\n';
        cout << "Optimal Tour: ";

        for (int i = 0; i < (int)bestPath.size(); i++) {
            cout << bestPath[i];

            if (i + 1 < (int)bestPath.size())
                cout << " -> ";
        }

        cout << '\n';
    }
};

int main() {
    int n, start;

    cout << "===== SWIFTSHIP TSP BRANCH AND BOUND =====\n";

    cout << "Enter number of cities: ";
    cin >> n;

    if (!cin || n < 2) {
        cout << "Invalid number of cities!\n";
        return 1;
    }

    vector<vector<long long>> matrix(
        n, vector<long long>(n));

    cout << "\nEnter the cost matrix (" << n << " x " << n << "):\n";
    cout << "Enter -1 if a direct route does not exist.\n";

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cin >> matrix[i][j];

            if (!cin) {
                cout << "Invalid matrix input!\n";
                return 1;
            }

            if (i != j && matrix[i][j] < -1) {
                cout << "Costs must be non-negative, or -1 for no route.\n";
                return 1;
            }

            if (i != j && matrix[i][j] == -1)
                matrix[i][j] = INF;
        }
    }

    cout << "\nEnter starting city (0 to " << n - 1 << "): ";
    cin >> start;

    if (!cin || start < 0 || start >= n) {
        cout << "Invalid starting city!\n";
        return 1;
    }

    TSPBranchBound tsp(n, matrix);
    tsp.solve(start);

    return 0;
}