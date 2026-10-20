
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
using namespace std;

struct Course {
    string name;
    vector<int> students;
    int color = -1;
    int room = -1;
};

struct Room {
    string name;
    int capacity;
};

class Timetable {
    int n;
    vector<Course> courses;
    vector<Room> rooms;
    vector<vector<int>> graph;

    vector<string> colorNames = {
        "", "BLUE", "GREEN", "RED", "ORANGE",
        "PURPLE", "CYAN", "MAGENTA", "YELLOW",
        "PINK", "BROWN", "GOLD", "TEAL"
    };

    string getColorName(int color) {
        int number = color + 1;

        if (number < (int)colorNames.size())
            return colorNames[number];

        return "COLOUR_" + to_string(number);
    }

public:
    Timetable(vector<Course> c, vector<Room> r) {
        courses = c;
        rooms = r;
        n = courses.size();
        graph.resize(n);
        buildGraph();
    }

    void buildGraph() {
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                bool conflict = false;

                for (int s : courses[i].students) {
                    if (find(courses[j].students.begin(),
                             courses[j].students.end(), s)
                        != courses[j].students.end()) {
                        conflict = true;
                        break;
                    }
                }

                if (conflict) {
                    graph[i].push_back(j);
                    graph[j].push_back(i);
                }
            }
        }
    }

    int greedyColoring() {
        vector<int> order(n);

        for (int i = 0; i < n; i++)
            order[i] = i;

        sort(order.begin(), order.end(), [&](int a, int b) {
            if (graph[a].size() != graph[b].size())
                return graph[a].size() > graph[b].size();
            return a < b;
        });

        int maxColor = -1;

        for (int u : order) {
            vector<bool> used(n, false);

            for (int v : graph[u]) {
                if (courses[v].color != -1)
                    used[courses[v].color] = true;
            }

            int color = 0;

            while (color < n && used[color])
                color++;

            courses[u].color = color;
            maxColor = max(maxColor, color);
        }

        return maxColor + 1;
    }

    bool allocateRooms() {
        for (auto &c : courses)
            c.room = -1;

        int slots = 0;

        for (const auto &c : courses)
            slots = max(slots, c.color + 1);

        vector<int> roomOrder(rooms.size());

        for (int i = 0; i < (int)rooms.size(); i++)
            roomOrder[i] = i;

        sort(roomOrder.begin(), roomOrder.end(),
             [&](int a, int b) {
                 return rooms[a].capacity < rooms[b].capacity;
             });

        for (int slot = 0; slot < slots; slot++) {
            vector<int> exams;

            for (int i = 0; i < n; i++) {
                if (courses[i].color == slot)
                    exams.push_back(i);
            }

            sort(exams.begin(), exams.end(),
                 [&](int a, int b) {
                     return courses[a].students.size() >
                            courses[b].students.size();
                 });

            vector<bool> roomUsed(rooms.size(), false);

            for (int exam : exams) {
                int required = courses[exam].students.size();
                bool assigned = false;

                for (int room : roomOrder) {
                    if (!roomUsed[room] &&
                        rooms[room].capacity >= required) {
                        courses[exam].room = room;
                        roomUsed[room] = true;
                        assigned = true;
                        break;
                    }
                }

                if (!assigned) {
                    cout << "\nRoom allocation failed for course "
                         << courses[exam].name
                         << ". Add a suitable room.\n";
                    return false;
                }
            }
        }

        return true;
    }

    void printConflicts() {
        cout << "\n===== COURSE CONFLICT GRAPH =====\n";

        for (int i = 0; i < n; i++) {
            cout << courses[i].name << ": ";

            for (int v : graph[i])
                cout << courses[v].name << " ";

            cout << '\n';
        }
    }

    void printSlotSummary(int slots) {
        cout << "\n===== EXAM TIMETABLE =====\n";
        cout << "Number of courses: " << n << '\n';
        cout << "Number of exam slots used: " << slots << "\n\n";

        for (int slot = 0; slot < slots; slot++) {
            cout << "Slot " << slot + 1 << " ["
                 << getColorName(slot) << "]: ";

            bool first = true;

            for (int i = 0; i < n; i++) {
                if (courses[i].color == slot) {
                    if (!first)
                        cout << ", ";

                    cout << courses[i].name;
                    first = false;
                }
            }

            cout << '\n';
        }
    }

    void printTimetable() {
        cout << "\n===== FINAL TIMETABLE WITH ROOMS =====\n";

        cout << left << setw(15) << "Course"
             << setw(20) << "Time Slot (Colour)"
             << setw(15) << "Room"
             << setw(10) << "Students" << '\n';

        cout << string(60, '-') << '\n';

        for (const auto &c : courses) {
            cout << left << setw(15) << c.name
                 << setw(20) <<
                    (getColorName(c.color));

            if (c.room != -1)
                cout << setw(15) << rooms[c.room].name;
            else
                cout << setw(15) << "Unassigned";

            cout << setw(10) << c.students.size() << '\n';
        }
    }

    bool checkConflicts() {
        for (int u = 0; u < n; u++) {
            for (int v : graph[u]) {
                if (courses[u].color == courses[v].color)
                    return false;
            }
        }

        return true;
    }
};

int main() {
    int n, s;

    cout << "UNIVERSITY EXAM TIMETABLE SCHEDULER\n";

    cout << "\nEnter number of courses: ";
    cin >> n;

    if (!cin || n <= 0) {
        cout << "Invalid number of courses.\n";
        return 1;
    }

    vector<Course> courses(n);

    for (int i = 0; i < n; i++) {
        cout << "Enter course name: ";
        cin >> courses[i].name;
    }

    cout << "\nEnter number of students: ";
    cin >> s;

    if (!cin || s < 0) {
        cout << "Invalid number of students.\n";
        return 1;
    }

    for (int i = 0; i < s; i++) {
        int count;

        cout << "Number of courses for student "
             << i + 1 << ": ";
        cin >> count;

        if (!cin || count < 0 || count > n) {
            cout << "Invalid course count.\n";
            return 1;
        }

        cout << "Enter course indexes (1 to " << n << "): ";

        vector<bool> seen(n, false);

        for (int j = 0; j < count; j++) {
            int course;
            cin >> course;

            if (!cin || course < 1 || course > n) {
                cout << "Invalid course index.\n";
                return 1;
            }

            if (!seen[course - 1]) {
                courses[course - 1].students.push_back(i + 1);
                seen[course - 1] = true;
            }
        }
    }

    int r;

    cout << "\nEnter number of rooms: ";
    cin >> r;

    if (!cin || r <= 0) {
        cout << "Invalid number of rooms.\n";
        return 1;
    }

    vector<Room> rooms(r);

    for (int i = 0; i < r; i++) {
        cout << "Enter room name and capacity: ";
        cin >> rooms[i].name >> rooms[i].capacity;

        if (!cin || rooms[i].capacity < 0) {
            cout << "Invalid room capacity.\n";
            return 1;
        }
    }

    Timetable timetable(courses, rooms);

    int slots = timetable.greedyColoring();

    timetable.printSlotSummary(slots);
    timetable.printConflicts();

    if (timetable.allocateRooms())
        timetable.printTimetable();

    if (timetable.checkConflicts())
        cout << "\nConflict check: PASS\n";
    else
        cout << "\nConflict check: FAIL\n";

    return 0;
}