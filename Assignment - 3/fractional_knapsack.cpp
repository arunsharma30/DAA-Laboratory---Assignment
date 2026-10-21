
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

bool compareItems(const Item& a, const Item& b) {
    if (a.ratio != b.ratio)
        return a.ratio > b.ratio;

    return a.name < b.name;
}

int main() {
    int n;
    double W;

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

        items[i].ratio = items[i].value / items[i].weight;
    }

    cout << "\nEnter maximum weight capacity of the boat (in kg): ";
    cin >> W;

    if (!cin || W < 0) {
        cout << "Invalid boat capacity!\n";
        return 1;
    }

    // Sort by descending utility-to-weight ratio
    sort(items.begin(), items.end(), compareItems);

    cout << fixed << setprecision(2);

    cout << "\nSorted Items (by Value/Weight):\n";
    cout << left
         << setw(22) << "Item"
         << setw(10) << "Weight"
         << setw(10) << "Value"
         << setw(15) << "Value/Weight"
         << setw(15) << "Type" << '\n';

    for (const auto& item : items) {
        cout << left
             << setw(22) << item.name
             << setw(10) << item.weight
             << setw(10) << item.value
             << setw(15) << item.ratio
             << (item.divisible ? "Divisible" : "Indivisible")
             << '\n';
    }

    double remainingCapacity = W;
    double totalValue = 0;
    double totalWeight = 0;

    // Greedy selection
    for (auto& item : items) {
        if (remainingCapacity <= 0)
            break;

        if (item.weight <= remainingCapacity) {
            item.takenWeight = item.weight;
            item.obtainedValue = item.value;
        }
        else if (item.divisible) {
            item.takenWeight = remainingCapacity;
            item.obtainedValue =
                (item.takenWeight / item.weight) * item.value;
        }
        else {
            continue;
        }

        remainingCapacity -= item.takenWeight;
        totalWeight += item.takenWeight;
        totalValue += item.obtainedValue;
    }

    cout << "\nItems selected for transport:\n";

    for (const auto& item : items) {
        if (item.takenWeight > 0) {
            cout << " - " << item.name
                 << ": " << item.takenWeight << " kg"
                 << ", Utility = " << item.obtainedValue
                 << ", Type = "
                 << (item.divisible ? "Divisible" : "Indivisible")
                 << '\n';
        }
    }

    cout << "\n===== Final Report =====\n";
    cout << "Total weight carried: " << totalWeight << " kg\n";
    cout << "Total utility value carried: "
         << totalValue << " units\n";

    return 0;
}