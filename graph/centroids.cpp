#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <map>

using namespace std;

struct Node {
    map <int, int> closest;
    int p = 0;
    int color = 0;

    vector <int> up_c;
    vector <int> up_dist;
};

void dfs_set_size(int u,
                 vector <vector <int> >& g,
                 vector <int>& used,
                 vector <Node>& nodes,
                 vector <int>& cur_sz,
                 int p = -1) {
    cur_sz[u] = 1;
    for (int v: g[u]) {
        if (used[v] || v == p) continue;
        dfs_set_size(v, g, used, nodes, cur_sz, u);
        cur_sz[u] += cur_sz[v];
    }
}

pair <int, int> dfs_find_centroid(int u,
                      vector <vector <int> >& g,
                      vector <int>& used,
                      vector <Node>& nodes,
                      vector <int>& cur_sz,
                      int n,
                      int dist = 0,
                      int p = -1) {

    int nxt_v = -1;

    for (int v: g[u]) {
        if (used[v] || v == p) continue;

        if (cur_sz[v] * 2 >= n) {
            nxt_v = v;
        }
    }

    if (nxt_v == -1) {
        return {u, dist};
    }

    return dfs_find_centroid(nxt_v, g, used, nodes, cur_sz, n, dist + 1, u);
}

void dfs_set_centroid_dist(int u,
                           vector <vector <int> >& g,
                           vector <int>& used,
                           vector <Node>& nodes,
                           int c,
                           int depth = 0,
                           int p = -1) {
    int cur_color = nodes[u].color;
    map <int, int>& closest = nodes[c].closest;
    if (closest.find(cur_color) == closest.end()) {
        closest[cur_color] = depth;
    } else {
        closest[cur_color] = min(depth, closest[cur_color]);
    }
    // cout << u << endl;
    nodes[u].up_c.push_back(c);
    nodes[u].up_dist.push_back(depth);
    for (int v: g[u]) {
        if (used[v] || v == p) continue;

        dfs_set_centroid_dist(v, g, used, nodes, c, depth + 1, u);
    }
}

pair <int, int> dfs_centroid(int u,
                 vector <vector <int> >& g,
                 vector <vector <int> >& gc,
                 vector <int>& used,
                 vector <Node>& nodes,
                 vector <int>& cur_sz) {
    dfs_set_size(u, g, used, nodes, cur_sz);
    int n = cur_sz[u];
    auto [c, dist] = dfs_find_centroid(u, g, used, nodes, cur_sz, n, 1);

    dfs_set_centroid_dist(c, g, used, nodes, c);
    // dfs_get_centroid_ans(c, g, used, nodes, 0, nodes[c].closest);

    used[c] = true;

    for (int v: g[c]) {
        if (used[v]) continue;
        auto [vc, dist] = dfs_centroid(v, g, gc, used, nodes, cur_sz);
        gc[c].push_back(vc);
    }

    return {c, dist};
}

void solve() {
    int n;
    cin >> n;
    vector <vector <int> > g(n);
    vector <int> used(n, false);

    vector <Node> nodes(n);

    for (int i = 1; i < n; i++) {
        int p;
        cin >> p;
        nodes[i].p = p;
        g[i].push_back(p);
        g[p].push_back(i);
    }

    for (int i = 0; i < n; i++) {
        cin >> nodes[i].color;
    }

    vector <vector <int> > gc(n);

    vector <int> cur_sz(n);
    auto [root, dist] = dfs_centroid(0, g, gc, used, nodes, cur_sz);

    int q;
    cin >> q;

    while (q--) {
        int u, color;
        cin >> u >> color;

        int ans = 1e9;

        vector <int>& up_c = nodes[u].up_c;
        vector <int>& up_dist = nodes[u].up_dist;

        for (int i = 0; i < up_c.size(); i++) {
            int c = up_c[i];
            int dist = up_dist[i];

            if (nodes[c].closest.find(color) != nodes[c].closest.end()) {
                ans = min(ans, dist + nodes[c].closest[color]);
            }
        }

        if (ans > n) {
            cout << "-1\n";
        } else {
            cout << ans << "\n";
        }
    }
}

int main() {
    solve();
}
