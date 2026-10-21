
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
using namespace std;

struct Item {
    string name;
    int weight;
    int utility;
    int priority;
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

    cout << "\nEnter item details (Name Weight Utility Priority):\n";

    for (int i = 0; i < N; i++) {
        cout << "Item " << i + 1 << ": ";

        cin >> items[i].name
            >> items[i].weight
            >> items[i].utility
            >> items[i].priority;

        if (!cin || items[i].weight <= 0 ||
            items[i].utility < 0 || items[i].priority <= 0) {
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

    // DP table for 0/1 Knapsack
    vector<vector<int>> dp(N + 1, vector<int>(W + 1, 0));

    for (int i = 1; i <= N; i++) {
        for (int w = 0; w <= W; w++) {
            // Do not select the current item
            dp[i][w] = dp[i - 1][w];

            // Select the current item if it fits
            if (items[i - 1].weight <= w) {
                dp[i][w] = max(
                    dp[i][w],
                    items[i - 1].utility +
                    dp[i - 1][w - items[i - 1].weight]
                );
            }
        }
    }

    // Recover the selected items
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

    // Calculate total weight and utility
    int totalWeight = 0;
    int totalUtility = 0;

    for (int index : selected) {
        totalWeight += items[index].weight;
        totalUtility += items[index].utility;
    }

    // Display the allocation results
    cout << "\n===== OPTIMAL RELIEF ALLOCATION =====\n";

    cout << left
         << setw(15) << "Item"
         << setw(15) << "Weight (kg)"
         << setw(12) << "Utility"
         << "Priority\n";

    cout << string(52, '-') << '\n';

    for (int index : selected) {
        cout << left
             << setw(15) << items[index].name
             << setw(15) << items[index].weight
             << setw(12) << items[index].utility
             << items[index].priority << '\n';
    }

    cout << "\nMaximum Total Utility: " << totalUtility << '\n';
    cout << "Total Weight Used: " << totalWeight << " kg\n";
    cout << "Remaining Capacity: " << W - totalWeight << " kg\n";

    cout << "\nSelected Items:\n";

    if (selected.empty()) {
        cout << "None\n";
    } else {
        for (int index : selected) {
            cout << items[index].name
                 << " | Utility: " << items[index].utility
                 << " | Priority: " << items[index].priority
                 << '\n';
        }
    }

    return 0;
}