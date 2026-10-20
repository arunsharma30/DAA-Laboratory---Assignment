
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <string>
using namespace std;

struct Item {
    string name;
    double weight;
    double value;
    bool divisible;
    double ratio;
    double takenWeight = 0;
    double obtainedValue = 0;
};

bool compareRatio(const Item& a, const Item& b) {
    return a.ratio > b.ratio;
}

int main() {
    int n;
    double W;

    cout << "===== EMERGENCY RELIEF SUPPLY DISTRIBUTION =====\n";

    cout << "Enter number of items: ";
    cin >> n;

    if (!cin || n <= 0) {
        cout << "Invalid number of items!\n";
        return 1;
    }

    vector<Item> items(n);

    cout << "\nEnter item details:\n";
    cout << "Format: Name Weight Utility Divisible(1=Yes, 0=No)\n";

    for (int i = 0; i < n; i++) {
        cout << "\nItem " << i + 1 << ": ";
        cin >> items[i].name
            >> items[i].weight
            >> items[i].value
            >> items[i].divisible;

        if (!cin || items[i].weight <= 0 ||
            items[i].value < 0 ||
            (items[i].divisible != 0 &&
             items[i].divisible != 1)) {
            cout << "Invalid item details!\n";
            return 1;
        }

        items[i].ratio =
            items[i].value / items[i].weight;
    }

    cout << "\nEnter boat capacity (kg): ";
    cin >> W;

    if (!cin || W < 0) {
        cout << "Invalid boat capacity!\n";
        return 1;
    }

    // Sort by descending utility-to-weight ratio
    sort(items.begin(), items.end(), compareRatio);

    double remainingCapacity = W;
    double totalValue = 0;

    // Greedy Fractional Knapsack
    for (int i = 0; i < n; i++) {
        if (remainingCapacity <= 0)
            break;

        if (items[i].weight <= remainingCapacity) {
            // Take the full item
            items[i].takenWeight = items[i].weight;
            items[i].obtainedValue = items[i].value;

            remainingCapacity -= items[i].weight;
            totalValue += items[i].obtainedValue;
        }
        else if (items[i].divisible) {
            // Take only the available fraction
            items[i].takenWeight = remainingCapacity;

            double fraction =
                remainingCapacity / items[i].weight;

            items[i].obtainedValue =
                fraction * items[i].value;

            totalValue += items[i].obtainedValue;
            remainingCapacity = 0;
        }
        // If indivisible and it does not fit, skip it.
    }

    cout << fixed << setprecision(2);

    cout << "\n===== OPTIMAL RELIEF DISTRIBUTION =====\n";
    cout << left
         << setw(15) << "Item"
         << setw(12) << "Weight"
         << setw(12) << "Utility"
         << setw(15) << "Taken (kg)"
         << "Value Obtained\n";

    cout << string(66, '-') << '\n';

    for (const auto& item : items) {
        cout << left
             << setw(15) << item.name
             << setw(12) << item.weight
             << setw(12) << item.value
             << setw(15) << item.takenWeight
             << item.obtainedValue << '\n';
    }

    cout << "\nBoat Capacity: " << W << " kg\n";
    cout << "Used Capacity: " << W - remainingCapacity << " kg\n";
    cout << "Remaining Capacity: " << remainingCapacity << " kg\n";
    cout << "Maximum Total Utility: " << totalValue << '\n';

    return 0;
}