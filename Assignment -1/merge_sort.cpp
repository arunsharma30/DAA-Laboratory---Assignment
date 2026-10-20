#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
using namespace std;

struct Transaction {
    vector<string> data;
    long long timestamp;
};

// Parse dates such as 12/26/2023 12:32
// and 08-05-2023 00:00
long long parseTimestamp(const string& s) {
    int month, day, year, hour, minute;
    char sep1, sep2, colon;
    char space;

    stringstream ss(s);

    if (!(ss >> month >> sep1 >> day >> sep2
             >> year >> hour)) {
        return 0;
    }

    // Reparse reliably using separate date and time parts.
    size_t pos = s.find(' ');
    if (pos == string::npos) return 0;

    string datePart = s.substr(0, pos);
    string timePart = s.substr(pos + 1);

    char dateSep = datePart.find('/') != string::npos
                   ? '/' : '-';

    replace(datePart.begin(), datePart.end(), dateSep, ' ');

    stringstream ds(datePart);
    if (!(ds >> month >> day >> year)) return 0;

    stringstream ts(timePart);
    if (!(ts >> hour >> colon >> minute)) return 0;

    return (long long)year * 100000000LL
         + (long long)month * 1000000LL
         + (long long)day * 10000LL
         + hour * 100LL + minute;
}

// Read complete CSV records, including quoted multiline fields.
bool readCSVRecord(istream& in, vector<string>& fields) {
    fields.clear();
    string field;
    char ch;
    bool quoted = false;
    bool started = false;

    while (in.get(ch)) {
        started = true;

        if (ch == '"') {
            if (quoted && in.peek() == '"') {
                in.get(ch);
                field += '"';
            } else {
                quoted = !quoted;
            }
        } else if (ch == ',' && !quoted) {
            fields.push_back(field);
            field.clear();
        } else if ((ch == '\n' || ch == '\r') && !quoted) {
            if (ch == '\r' && in.peek() == '\n')
                in.get(ch);

            fields.push_back(field);
            return true;
        } else {
            field += ch;
        }
    }

    if (!started) return false;

    fields.push_back(field);
    return true;
}

string writeCSVRecord(const vector<string>& fields) {
    string result;

    for (size_t i = 0; i < fields.size(); i++) {
        if (i > 0) result += ',';

        bool quote = fields[i].find_first_of(",\"\r\n")
                     != string::npos;

        if (quote) result += '"';

        for (char ch : fields[i]) {
            if (ch == '"') result += '"';
            result += ch;
        }

        if (quote) result += '"';
    }

    return result;
}

// Merge two sorted sections of the array.
void merge(vector<Transaction>& a, int left,
           int mid, int right) {
    vector<Transaction> temp;
    temp.reserve(right - left + 1);

    int i = left;
    int j = mid + 1;

    while (i <= mid && j <= right) {
        if (a[i].timestamp <= a[j].timestamp) {
            temp.push_back(a[i++]);
        } else {
            temp.push_back(a[j++]);
        }
    }

    while (i <= mid) temp.push_back(a[i++]);
    while (j <= right) temp.push_back(a[j++]);

    for (int k = 0; k < (int)temp.size(); k++) {
        a[left + k] = temp[k];
    }
}

// Merge Sort: O(n log n)
void mergeSort(vector<Transaction>& a,
               int left, int right) {
    if (left >= right) return;

    int mid = left + (right - left) / 2;

    mergeSort(a, left, mid);
    mergeSort(a, mid + 1, right);
    merge(a, left, mid, right);
}

int main() {
    ifstream input("Retail_Transaction_Dataset.csv",
                   ios::binary);

    if (!input.is_open()) {
        cerr << "Error: Cannot open input CSV file.\n";
        return 1;
    }

    vector<string> header;

    if (!readCSVRecord(input, header)) {
        cerr << "Error: CSV file is empty.\n";
        return 1;
    }

    // Find TransactionDate by its column name.
    int dateIndex = -1;

    for (int i = 0; i < (int)header.size(); i++) {
        if (header[i] == "TransactionDate") {
            dateIndex = i;
            break;
        }
    }

    if (dateIndex == -1) {
        cerr << "Error: TransactionDate column not found.\n";
        return 1;
    }

    vector<Transaction> transactions;
    vector<string> row;
    int invalidDates = 0;

    while (readCSVRecord(input, row)) {
        if (row.size() != header.size()) {
            cerr << "Skipping malformed CSV record.\n";
            continue;
        }

        long long key = parseTimestamp(row[dateIndex]);

        if (key == 0) {
            invalidDates++;
            continue;
        }

        transactions.push_back({row, key});
    }

    input.close();

    if (transactions.empty()) {
        cerr << "Error: No valid transactions were read.\n";
        return 1;
    }

    mergeSort(transactions, 0,
              (int)transactions.size() - 1);

    ofstream output("Sorted_Retail_Transactions.csv",
                    ios::binary);

    if (!output.is_open()) {
        cerr << "Error: Cannot create output CSV.\n";
        return 1;
    }

    output << writeCSVRecord(header) << "\r\n";

    for (const auto& t : transactions) {
        output << writeCSVRecord(t.data) << "\r\n";
    }

    output.close();

    cout << "Merge Sort completed successfully!\n";
    cout << "Total transactions sorted: "
         << transactions.size() << '\n';
    cout << "Invalid timestamps skipped: "
         << invalidDates << '\n';
    cout << "Output: Sorted_Retail_Transactions.csv\n";

    return 0;
}