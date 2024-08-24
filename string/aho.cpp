#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;

const int INF = 1e9;

struct Node {
    vector <Node*> to;
    vector <Node*> go;
    Node *link = nullptr;
    Node *suff_link = nullptr;
    bool is_terminal = false;
    pair <int, int> idx = {INF, -INF};
    vector <int> s_idx;

    Node() {
        to.assign(26, nullptr);
        go.assign(26, nullptr);
    }
};

class Aho {
public:
    Aho(vector <string>& dict) {
        root = new Node();
        dict_size = dict.size();

        for (int i = 0; i < dict.size(); i++) {
            AddString(dict[i], i);
        }

        CalculateLinks();
    }

    void CalculateLinks() {
        vector <Node*>& nodes = nodes_in_order;
        nodes.push_back(root);

        int idx = 0;
        while (idx < nodes.size()) {
            Node *node = nodes[idx];

            for (int c = 0; c < 26; c++) {
                if (!node->to[c]) {
                    continue;
                }
                nodes.push_back(node->to[c]);
            }
            idx++;
        }

        for (Node *u: nodes) {
            for (int c = 0; c < 26; c++) {
                Node *v = u->to[c];
                if (!v) {
                    u->go[c] = (u->link ? u->link->go[c] : root->go[c]);
                    continue;
                }

                v->link = (u->link ? u->link->go[c] : root->go[c]);
                u->go[c] = v;

                if (!v->link) {
                    continue;
                }
                if (v->link->is_terminal) {
                    v->suff_link = v->link;
                } else {
                    v->suff_link = (v->link ? v->link->suff_link : 0);
                }
            }
        }
    }

    void AddString(string& s, int i) {
        Node *runner = root;

        for (auto c_char: s) {
            int c = c_char - 'a';
            if (!runner->to[c]) {
                runner->to[c] = new Node();
            }

            runner = runner->to[c];
        }

        runner->is_terminal = 1;
        runner->s_idx.push_back(i);
    }

    void AnalizeText(string& t) {
        Node *runner = root;
        
        for (int i = 0; i < t.size(); i++) {
            int c = t[i] - 'a';
            runner = runner->go[c];
            if (runner == nullptr) runner = root;
            runner->idx.first = min(runner->idx.first, i);
            runner->idx.second = max(runner->idx.second, i);
        }
    }

    void PropagateResults() {
        for (int i = nodes_in_order.size() - 1; i >= 0; i--) {
            Node *u = nodes_in_order[i];
            if (u->idx.first == INF) continue;

            Node *v = u->suff_link;

            if (!v) continue;

            v->idx.first = min(v->idx.first, u->idx.first);
            v->idx.second = max(v->idx.second, u->idx.second);
        }
    }

    void GetAnswer(vector <pair <int, int> >& ans) {
        ans.assign(dict_size, {INF, -INF});
        for (Node *u: nodes_in_order) {
            for (int i: u->s_idx) {
                ans[i] = u->idx;
            }
        }
    }

    Node *root = nullptr;
    vector <Node*> nodes_in_order;
    int dict_size = 0;
};

void solve() {
    int n;
    cin >> n;

    vector <string> dict(n);

    for (int i = 0; i < n; i++) {
        cin >> dict[i];
    }

    Aho aho(dict);
    string t;
    cin >> t;

    aho.AnalizeText(t);
    aho.PropagateResults();

    vector <pair <int, int> > ans;
    aho.GetAnswer(ans);

    for (int i = 0; i < n; i++) {
        if (ans[i].first == INF) {
            cout << "-1 -1\n";
            continue;
        }
        cout << ans[i].first - dict[i].size() + 1 << ' ' << ans[i].second - dict[i].size() + 1 << "\n";
    }
}

int main() {
    solve();
}
