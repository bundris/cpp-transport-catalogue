#include "input_reader.h"

namespace tcatalogue::reader {
    namespace stringProc {
        string_view Lstrip(string_view str) {
            str.remove_prefix(std::min(str.find_first_not_of(' '), str.size()));
            return str;
        }

        string_view Rstrip(string_view str) {
            int count = 0;
            for (int i = str.size() - 1; i > -1; i--) {
                if (str.at(i) == ' ') {
                    ++count;
                } else {
                    break;
                }
            }
            str.remove_suffix(count);
            return str;
        }

        string_view Strip(string_view str) {
            return Rstrip(Lstrip(str));
        }

        vector<string_view> SplitBy(string_view line, char c) {
            vector<string_view> words;
            size_t sep_pos = 0;
            while ((sep_pos = line.find(c)) != string::npos) {
                words.push_back(Strip(line.substr(0, sep_pos)));
                line = line.substr(sep_pos + 1);
            }
            words.push_back(Strip(line.substr(0, sep_pos)));
            return words;
        }
    }
    vector<string> ReadNLines(istream &in) {
        int result;
        string s;
        getline(in, s);
        result = stoi(s);
        vector<string> lines;
        lines.reserve(result);
        for (int i = 0; i < result; ++i) {
            getline(in, s);
            lines.push_back(s);
        }
        return lines;
    }

    ReadedStop ParseStop(string_view line) {
        using namespace stringProc;
        double latitude, longitude;
        vector<pair<string_view, int>> distances;
        vector<string_view> dist_lines;
        string_view temp_stop_name;
        size_t m;
        int meters;
        size_t semicol_pos = line.find(':');
        size_t comma_pos = line.find(',', semicol_pos);
        size_t comma_dist_pos = line.find(',', comma_pos + 1); // looking for 2nd ','

        string_view name = Strip(line.substr(5, semicol_pos - 5));
        latitude = stod(string(line.substr(semicol_pos + 1, comma_pos - semicol_pos - 1)));
        if (comma_dist_pos == string::npos) {
            longitude = stod(string(line.substr(comma_pos + 1)));
        } else {
            longitude = stod(string(line.substr(comma_pos + 1, comma_dist_pos)));
            string_view dist_line = line.substr(comma_dist_pos + 1);
            dist_lines = SplitBy(dist_line, ',');
            for (string_view l: dist_lines) {
                m = l.find('m');
                meters = stoi(string(l.substr(0, m)));
                temp_stop_name = Strip(l.substr(l.find("to ") + 3));
                distances.push_back({temp_stop_name, meters});
            }
        }
        return {name, latitude, longitude, distances};
    }

    tuple<string_view, vector<string_view>> ParseBus(string_view line) {
        using namespace stringProc;
        vector<string_view> stop_names;
        auto semicol_pos = line.find(':');
        string_view bus_name = Strip(line.substr(4, semicol_pos - 4));
        string_view route = line.substr(semicol_pos + 1);
        size_t separator_pos = route.find('>');
        char delim = separator_pos != string::npos ? '>' : '-';
        stop_names = SplitBy(route, delim);
        if (delim == '-') {
            for (int i = stop_names.size() - 2; i > -1; i--) {
                stop_names.push_back(stop_names[i]);
            }
        }
        return {bus_name, stop_names};
    }
}
