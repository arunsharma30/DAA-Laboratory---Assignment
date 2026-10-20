
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <random>
#include <algorithm>
using namespace std;

struct Movie {
    string title;
    double rating;
    int releaseYear;
    long long popularity;
};

mt19937 rng(random_device{}());

// Read CSV records, including quoted fields.
bool readCSVRecord(istream& in, vector<string>& fields) {
    fields.clear();
    string field;
    char ch;
    bool quoted = false, started = false;

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

// Escape CSV fields when writing the output file.
string makeCSVRecord(const vector<string>& fields) {
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

// Three-way randomized Quick Sort: highest rating first.
void quickSort(vector<Movie>& movies, int low, int high) {
    while (low < high) {
        uniform_int_distribution<int> dist(low, high);
        double pivot = movies[dist(rng)].rating;

        int i = low, j = low, k = high;

        while (j <= k) {
            if (movies[j].rating > pivot) {
                swap(movies[i++], movies[j++]);
            } else if (movies[j].rating < pivot) {
                swap(movies[j], movies[k--]);
            } else {
                j++;
            }
        }

        // Sort the smaller partition recursively.
        if (i - low < high - k) {
            quickSort(movies, low, i - 1);
            low = k + 1;
        } else {
            quickSort(movies, k + 1, high);
            high = i - 1;
        }
    }
}

int main() {
    ifstream input("movies_dataset.csv", ios::binary);

    if (!input) {
        cerr << "Cannot open movies_dataset.csv\n";
        return 1;
    }

    vector<string> header, row;
    if (!readCSVRecord(input, header)) {
        cerr << "Input CSV is empty.\n";
        return 1;
    }

    int titleCol = -1, ratingCol = -1;
    int yearCol = -1, popularityCol = -1;

    for (int i = 0; i < (int)header.size(); i++) {
        if (header[i] == "title") titleCol = i;
        if (header[i] == "rating") ratingCol = i;
        if (header[i] == "release_year") yearCol = i;
        if (header[i] == "popularity") popularityCol = i;
    }

    if (titleCol < 0 || ratingCol < 0 ||
        yearCol < 0 || popularityCol < 0) {
        cerr << "Required columns not found in CSV header.\n";
        return 1;
    }

    vector<Movie> movies;
    int skipped = 0;

    while (readCSVRecord(input, row)) {
        if (row.size() != header.size()) {
            skipped++;
            continue;
        }

        try {
            Movie m;
            m.title = row[titleCol];
            m.rating = stod(row[ratingCol]);
            m.releaseYear = stoi(row[yearCol]);
            m.popularity = stoll(row[popularityCol]);
            movies.push_back(m);
        } catch (...) {
            skipped++;
        }
    }

    input.close();

    if (movies.empty()) {
        cerr << "No valid movies found.\n";
        return 1;
    }

    quickSort(movies, 0, (int)movies.size() - 1);

    // Create a NEW CSV file; original file remains unchanged.
    ofstream output("Sorted_Movies.csv", ios::binary);

    if (!output) {
        cerr << "Could not create Sorted_Movies.csv\n";
        return 1;
    }

    output << makeCSVRecord(header) << "\r\n";

    for (const Movie& m : movies) {
        vector<string> fields(header.size());

        fields[titleCol] = m.title;

        ostringstream rating;
        rating << fixed << setprecision(1) << m.rating;
        fields[ratingCol] = rating.str();

        fields[yearCol] = to_string(m.releaseYear);
        fields[popularityCol] = to_string(m.popularity);

        output << makeCSVRecord(fields) << "\r\n";
    }

    output.close();

    cout << "Quick Sort completed successfully!\n";
    cout << "Movies sorted: " << movies.size() << '\n';
    cout << "Invalid rows skipped: " << skipped << '\n';
    cout << "Sorted output saved to: Sorted_Movies.csv\n";

    return 0;
}