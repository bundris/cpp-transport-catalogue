#include "stat_reader.h"

namespace tcatalogue::printer {
    void PrintBus(const string &busname, int stops, int unique_stops, double length, double curvature) {
        if (stops == 0) {
            cout << "Bus "s << busname << ": not found"s << endl;
        } else {
            cout << setprecision(6) << "Bus "s << busname << ": "s
                 << stops << " stops on route, "s
                 << unique_stops << " unique stops, "s
                 << length << " route length, "s
                 << curvature << " curvature"s
                 << endl;
        }
    }

    void PrintRoutes(const string &stopname, const vector<string> &buses) {
        if (buses.empty()) {
            cout << "Stop "s << stopname << ": no buses"s << endl;
        } else {
            cout << "Stop "s << stopname << ": buses"s;
            for (const auto &bus: buses) {
                cout << " " << bus;
            }
            cout << endl;
        }
    }
}