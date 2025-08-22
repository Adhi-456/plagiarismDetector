
#include <bits/stdc++.h>
#include "third_party/httplib.h"
#include "third_party/json.hpp"
using json = nlohmann::json;
using namespace std;

// Rabin-Karp Rolling Hash for k-word shingles
struct RabinKarp {
    const long long base = 257;
    const long long mod = 1e9 + 7;

    long long hashString(const vector<string> &words, int start, int k) {
        long long h = 0;
        for (int i = 0; i < k; i++) {
            for (char c : words[start+i]) {
                h = (h * base + c) % mod;
            }
            h = (h * base + '|') % mod; // separator
        }
        return h;
    }

    unordered_map<long long, vector<int>> getHashes(const vector<string> &words, int k) {
        unordered_map<long long, vector<int>> hashes;
        if ((int)words.size() < k) return hashes;
        for (int i = 0; i <= (int)words.size()-k; i++) {
            long long h = hashString(words,i,k);
            hashes[h].push_back(i);
        }
        return hashes;
    }
};

vector<string> tokenize(const string &text) {
    vector<string> tokens;
    string word;
    for (char c : text) {
        if (isalnum(c)) word.push_back(tolower(c));
        else if (!word.empty()) {
            tokens.push_back(word);
            word.clear();
        }
    }
    if (!word.empty()) tokens.push_back(word);
    return tokens;
}

json checkPlagiarism(vector<pair<string,string>> docs, int k) {
    RabinKarp rk;
    json result;

    for (int i = 0; i < docs.size(); i++) {
        for (int j = i+1; j < docs.size(); j++) {
            auto words1 = tokenize(docs[i].second);
            auto words2 = tokenize(docs[j].second);

            // ✅ Auto-adjust k so it never exceeds min doc length
            int effK = min({k, (int)words1.size(), (int)words2.size()});

            // If one doc is empty, similarity = 0
            if (effK == 0) {
                result["pairs"].push_back({
                    {"doc1", docs[i].first},
                    {"doc2", docs[j].first},
                    {"similarity", words1 == words2 ? 1.0 : 0.0}
                });
                continue;
            }

            auto h1 = rk.getHashes(words1, effK);
            auto h2 = rk.getHashes(words2, effK);

            int match = 0;
            unordered_set<long long> unique;
            for (auto &kv : h1) unique.insert(kv.first);
            for (auto &kv : h2) if (unique.count(kv.first)) match++;

            double sim = (2.0*match)/(h1.size()+h2.size());
            result["pairs"].push_back({
                {"doc1", docs[i].first},
                {"doc2", docs[j].first},
                {"similarity", sim}
            });
        }
    }
    return result;
}

int main() {
    httplib::Server svr;

    // 1) Add CORS headers
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Headers", "Content-Type"},
        {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"}
    });

    // 2) Handle preflight (OPTIONS) requests
    svr.Options(R"(/.*)", [](const httplib::Request&, httplib::Response& res) {
        res.status = 204; // No Content
    });

    // 3) Health check
    svr.Get("/", [](const httplib::Request&, httplib::Response &res) {
        res.set_content("✅ Plagiarism Checker API is running", "text/plain");
    });

    // 4) Plagiarism check API
    svr.Post("/check", [](const httplib::Request &req, httplib::Response &res) {
        auto body = json::parse(req.body);
        int k = body["k"];
        vector<pair<string,string>> docs;
        for (auto &d : body["documents"]) {
            docs.push_back({d["id"], d["text"]});
        }
        auto result = checkPlagiarism(docs, k);
        res.set_content(result.dump(), "application/json");
    });

    cout << "Server running on http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);
}


// g++ -std=c++17 -O2 -pthread -D_WIN32_WINNT=0x0A00 server.cpp -o server -lws2_32
//Artificial intelligence is transforming the world of technology rapidly
//The world of technology is rapidly transforming because of artificial intelligence
//k=3