#pragma once
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <deque>
#include <string>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include "input_reader.h"
#include "stat_reader.h"
#include "geo.h"
using namespace std;

namespace tcatalogue {
    using namespace geo;
    struct Stop {
        string name;
        Coordinates point;
    };
    struct Bus {
        string name;
        vector<Stop *> route;
    };

    class TransportCatalogue {
    public:
        TransportCatalogue(istream &in);

        // Get Methods
        unordered_set<Bus *> GetBusesByStop(const string &stopname) const;
        vector<Stop *> GetStopsByBus(const string &busname) const;
        tuple<int, int, double, double> GetBusStats(const string &busname);
        vector<string> GetStopStats(const string &stopname);

        // Procedures for catalogue reading and responding
        void FillCatalogue(const vector<string> &in_queries);
        void ProcessRequests(const vector<string> &in_queries);

    private:
        class StringViewHasher {
        public:
            size_t operator()(const string_view &str) const {
                return static_cast<size_t>(hasher_(string(str)));
            }
            hash<string> hasher_;
        };
        // Hasher for directed edge of 2 vertexes (bus stops)
        class EdgeHasher {
        public:
            size_t operator()(const pair<Stop*, Stop*> &p) const {
                size_t hash_val = (size_t) p.first * 37 + (size_t) p.second * 37 * 37;
                return static_cast<size_t>(hasher_(hash_val));
            }

            hash<size_t> hasher_;
        };

        deque<Stop> stops_;
        deque<Bus> buses_;
        unordered_map<string_view, Bus*, StringViewHasher> busname_to_bus_;
        unordered_map<string_view, Stop*, StringViewHasher> stopname_to_stop_;
        unordered_map<string_view, unordered_set<Bus*>, StringViewHasher> stopname_to_buses_;
        unordered_map<pair<Stop*, Stop*>, int, EdgeHasher> distances_;
    };
}