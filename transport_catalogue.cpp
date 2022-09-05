#include "transport_catalogue.h"
#include "stat_reader.h"
#include "geo.h"

namespace tcatalogue {
    using namespace reader;
    using namespace printer;
    void TransportCatalogue::FillCatalogue(const vector<string> &in_queries) {
        vector<ReadedStop> stops;
        vector<tuple<string_view, vector<string_view>>> buses;
        size_t last = 0;
        for (string_view line: in_queries) {
            if (line.substr(0, 3) == "Bus") {
                buses.push_back(ParseBus(line));
            } else if (line.substr(0, 4) == "Stop") {
                stops.push_back(ParseStop(line));
            } else {
                throw std::invalid_argument("Unknown command"s);
            }
        }
        // Заполняем остановки
        for (const auto &stop: stops) {
            Stop s = {string(stop.name), stop.latitude, stop.longitude};
            stops_.insert(stops_.end(), s);
            last = stops_.size() - 1;
            stopname_to_stop_[string_view(stops_[last].name)] = &stops_[last];
        }
        // Заполняем маршруты
        for (const auto &bus: buses) {
            auto stop_list = get<1>(bus);
            vector<Stop *> v;
            v.reserve(stop_list.size());
            for (string_view strv: stop_list) {
                v.push_back(stopname_to_stop_[strv]);
            }
            buses_.insert(buses_.end(), {string(get<0>(bus)), v});
            last = buses_.size() - 1;
            busname_to_bus_[string_view(buses_[last].name)] = &buses_[last];
            for (string_view strv: stop_list) {
                stopname_to_buses_[strv].insert(&buses_[last]);
            }
        }
        // Заполняем расстояния
        Stop *s1;
        Stop *s2;
        for (const auto &stop: stops) {
            for (auto temp_stop: stop.distances) {
                s1 = stopname_to_stop_[stop.name];
                s2 = stopname_to_stop_[temp_stop.first];
                distances_[pair(s1, s2)] = temp_stop.second;
            }
        }
    }

    TransportCatalogue::TransportCatalogue(istream &in) {
        vector<string> input_lines = ReadNLines(in);
        vector<string> db_requests = ReadNLines(in);
        FillCatalogue(input_lines);
        ProcessRequests(db_requests);
    }

    vector<Stop *> TransportCatalogue::GetStopsByBus(const string &busname) const {
        if (busname_to_bus_.count(string_view(busname)) == 0) {
            return {};
        }
        return busname_to_bus_.at(string_view(busname))->route;
    }

    unordered_set<Bus *> TransportCatalogue::GetBusesByStop(const string &stopname) const {
        if (stopname_to_buses_.count(string_view(stopname)) == 0) {
            return {};
        }
        return stopname_to_buses_.at(string_view(stopname));
    }

    vector<string> TransportCatalogue::GetStopStats(const string &stopname) {
        vector<string> b;
        auto buses = GetBusesByStop(stopname);
        b.reserve(buses.size());
        for (const auto bus: buses) {
            b.push_back(bus->name);
        }
        sort(b.begin(), b.end());
        return b;
    }

    tuple<int, int, double, double> TransportCatalogue::GetBusStats(const string &busname) {
        auto stops = GetStopsByBus(busname);
        size_t len = stops.size();
        if (len == 0) {
            return {0, 0, 0, 0};
        }
        auto temp = stops;
        sort(temp.begin(), temp.end());
        auto last = unique(temp.begin(), temp.end());
        size_t uniq_num = distance(temp.begin(), last);
        double geo_dist = 0;
        double dist = 0;
        for (int i = 0; i < len - 1; i++) {
            geo_dist += ComputeDistance(stops[i]->point, stops[i + 1]->point);
            if (distances_.find(pair(stops[i], stops[i + 1])) != distances_.end()) {
                dist += distances_.at(pair(stops[i], stops[i + 1]));
            } else if (distances_.find(pair(stops[i + 1], stops[i])) != distances_.end()) {
                dist += distances_.at(pair(stops[i + 1], stops[i]));
            } else {
                dist += ComputeDistance(stops[i]->point, stops[i + 1]->point);
            }
        }
        double curvature = dist / geo_dist;
        return {len, uniq_num, dist, curvature};
    }

    void TransportCatalogue::ProcessRequests(const vector<string> &requests) {
        for (const string &req: requests) {
            if (req.substr(0, 3) == "Bus") {
                auto busname = req.substr(4);
                auto[stops_num, unique_stops, length, curvature] = GetBusStats(busname);
                PrintBus(busname, stops_num, unique_stops, length, curvature);
            }
            if (req.substr(0, 4) == "Stop") {
                auto stopname = req.substr(5);
                if (stopname_to_stop_.find(stopname) == stopname_to_stop_.end()) {
                    cout << "Stop " << stopname << ": not found" << endl;
                } else {
                    auto buses = GetStopStats(stopname);
                    PrintRoutes(stopname, buses);
                }
            }
        }
    }
}