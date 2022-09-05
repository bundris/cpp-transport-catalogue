#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <tuple>
#include <algorithm>
#include <iomanip>
#include <unordered_set>
using namespace std;

namespace tcatalogue {
    namespace printer {
        void PrintBus(const string &busname, int stops, int unique_stops, double length, double curvature);
        void PrintRoutes(const string &stopname, const vector<string> &buses);
    }
}