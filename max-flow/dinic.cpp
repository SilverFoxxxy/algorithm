#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

typedef long long ll;

const ll INF = 1e18;

struct Edge {
    int v;
    ll flow;
    ll capacity;
    int back_i;
    int i;
};

ll dfs(int u,
        vector <vector <Edge> >& g,
        vector <ll>& d,
        vector <int>& edge_cnt,
        int sink,
        ll max_delta = INF) {
    if (max_delta <= 0) return 0;
    if (u == sink) return max_delta;
    ll cur_delta = 0;
    while (edge_cnt[u] < g[u].size()) {
        auto& e = g[u][edge_cnt[u]];
        if (d[e.v] != d[u] + 1 || e.capacity - e.flow <= 0) {
            edge_cnt[u]++;
            continue;
        }
        ll delta = dfs(e.v, g, d, edge_cnt, sink, min(max_delta, e.capacity - e.flow));
        if (delta > 0) {
            e.flow += delta;
            g[e.v][e.back_i].flow -= delta;
            return delta;
        }
        edge_cnt[u]++;
        if (max_delta <= 0) break;
    }
    return cur_delta;
}

void solve() {
    int n, m;
    cin >> n >> m;

    vector <vector <Edge> > g(n);
    for (int i = 0; i < m; i++) {
        int u, v, w;
        cin >> u >> v >> w;
        u--; v--;
        g[u].push_back({v, 0, w, (int)g[v].size(), i});
        g[v].push_back({u, 0, 0, (int)g[u].size() - 1, i});
    }

    int source = 0;
    int sink = n - 1;

    vector <ll> d(n + 3, 0);
    ll cur_zero_d = 1;

    vector <int> edge_cnt(n + 3, 0);
    vector <bool> used(n + 1, false);
    vector <int> used_a;

    ll max_flow = 0;
    while (true) {
        ll net_delta = 0;
        queue <int> q;
        q.push(source);
        edge_cnt[source] = 0;
        d[source] = cur_zero_d;

        bool flag = false;

        while (!q.empty() && !flag) {
            int u = q.front(); q.pop();

            for (auto e: g[u]) {
                if (d[e.v] >= cur_zero_d) continue;
                if (e.capacity - e.flow <= 0) continue;
                d[e.v] = d[u] + 1;
                if (e.v == sink) {
                    flag = true;
                    break;
                }
                q.push(e.v);
                edge_cnt[e.v] = 0;
            }
        }

        while (true) {
            ll delta = dfs(source, g, d, edge_cnt, sink);
            net_delta += delta;
            if (!delta) break;
        }

        cur_zero_d += 2 * n;

        max_flow += net_delta;
        if (!net_delta) break;
    }

    cout << max_flow << endl;
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    solve();
}
