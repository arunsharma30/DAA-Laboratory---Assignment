
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
using namespace std;

struct Item {
    string name;
    int weight;
    int value;
};

int main() {
    int N, W;

    cout << "===== DISASTER RELIEF RESOURCE ALLOCATION =====\n";

    cout << "Enter number of items: ";
    cin >> N;

    if (!cin || N <= 0) {
        cout << "Invalid number of items!\n";
        return 1;
    }

    vector<Item> items(N);

    cout << "\nEnter item details (Name Weight Utility):\n";

    for (int i = 0; i < N; i++) {
        cout << "Item " << i + 1 << ": ";
        cin >> items[i].name
            >> items[i].weight
            >> items[i].value;

        if (!cin || items[i].weight <= 0 ||
            items[i].value < 0) {
            cout << "Invalid item details!\n";
            return 1;
        }
    }

    cout << "\nEnter truck capacity (kg): ";
    cin >> W;

    if (!cin || W < 0) {
        cout << "Invalid truck capacity!\n";
        return 1;
    }

    // DP table: dp[i][w] is the maximum utility
    // using the first i items with capacity w.
    vector<vector<int>> dp(
        N + 1, vector<int>(W + 1, 0));

    // Fill the DP table
    for (int i = 1; i <= N; i++) {
        for (int w = 0; w <= W; w++) {
            dp[i][w] = dp[i - 1][w];

            if (items[i - 1].weight <= w) {
                dp[i][w] = max(
                    dp[i][w],
                    items[i - 1].value +
                    dp[i - 1][w - items[i - 1].weight]
                );
            }
        }
    }

    int maxUtility = dp[N][W];

    // Traceback to find selected items
    vector<int> selected;
    int remainingCapacity = W;

    for (int i = N; i >= 1; i--) {
        if (dp[i][remainingCapacity] !=
            dp[i - 1][remainingCapacity]) {
            selected.push_back(i - 1);
            remainingCapacity -= items[i - 1].weight;
        }
    }

    reverse(selected.begin(), selected.end());

    int totalWeight = 0;

    cout << "\n===== OPTIMAL RELIEF ALLOCATION =====\n";
    cout << left << setw(15) << "Item"
         << setw(15) << "Weight (kg)"
         << "Utility\n";
    cout << string(40, '-') << '\n';

    for (int index : selected) {
        cout << left << setw(15) << items[index].name
             << setw(15) << items[index].weight
             << items[index].value << '\n';

        totalWeight += items[index].weight;
    }

    cout << "\nMaximum Total Utility: " << maxUtility << '\n';
    cout << "Total Weight Used: " << totalWeight << " kg\n";
    cout << "Remaining Capacity: "
         << W - totalWeight << " kg\n";

    cout << "\nSelected Items: ";
    if (selected.empty()) {
        cout << "None";
    } else {
        for (int i = 0; i < (int)selected.size(); i++) {
            cout << items[selected[i]].name;
            if (i + 1 < (int)selected.size())
                cout << ", ";
        }
    }

    cout << '\n';

    return 0;
}