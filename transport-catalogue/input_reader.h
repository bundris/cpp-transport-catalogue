#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <tuple>

using namespace std;
namespace tcatalogue::reader {
    namespace stringProc {
        string_view Lstrip(string_view str);
        string_view Rstrip(string_view str);
        string_view Strip(string_view str);
        vector<string_view> SplitBy(string_view line, char c);
    }
    struct ReadedStop {
        string_view name;
        double latitude;
        double longitude;
        vector<pair<string_view, int>> distances;
    };

    vector<string> ReadNLines(istream &in);
    ReadedStop ParseStop(string_view line);
    tuple<string_view, vector<string_view>> ParseBus(string_view line);
}