#include <iostream>
#include <stdio.h>
#include <vector>
#include <algorithm>

using namespace std;

class Tree {
public:
    Tree(int n): n(n), tree(n * 4, 0) {}

    void refresh(int node_i, int l, int r) {
        if (l == r) return;

        tree[node_i] = max(tree[node_i * 2 + 1], tree[node_i * 2 + 2]);
    }

    void add(int node_i, int l, int r, int i, int x) {
        if (i < l || r < i) {
            return;
        }

        if (l == r) {
            tree[node_i] += x;
            return;
        }

        int m = (l + r) / 2;
        add(node_i * 2 + 1, l, m, i, x);
        add(node_i * 2 + 2, m + 1, r, i, x);
        refresh(node_i, l, r);
    }

    void add(int i, int x) {
        add(0, 0, n - 1, i, x);
    }

    int get(int node_i, int l, int r, int ql, int qr) {
        if (qr < l || r < ql) {
            return 0;
        }

        if (ql <= l && r <= qr) {
            return tree[node_i];
        }

        int m = (l + r) / 2;
        int lget = get(node_i * 2 + 1, l, m, ql, qr);
        int rget = get(node_i * 2 + 2, m + 1, r, ql, qr);

        return max(lget, rget);
    }

    int get(int ql, int qr) {
        return get(0, 0, n - 1, ql, qr);
    }

    int n;
    vector <int> tree;
};

void dfs_sz(int u,
            vector <vector <int> >& g,
            vector <bool>& used,
            vector <int>& sz) {
    if (used[u]) return;
    used[u] = true;

    for (int v: g[u]) {
        if (used[v]) continue;
        dfs_sz(v, g, used, sz);
        sz[u] += sz[v];
    }

}

void dfs_depth(int u,
            vector <vector <int> >& g,
            vector <int>& sz,
            vector <bool>& used,
            vector <int>& p,
            int parent,
            vector <int>& depth,
            int d) {
    if (used[u]) return;
    used[u] = true;
    p[u] = parent;
    depth[u] = d;

    int max_v_i = -1;
    int max_sz = 0;

    for (int i = 0; i < g[u].size(); i++) {
        int v = g[u][i];
        if (used[v]) continue;
        dfs_depth(v, g, sz, used, p, u, depth, d + 1);
        if (sz[v] > max_sz) {
            max_sz = sz[v];
            max_v_i = i;
        }
    }

    if (max_v_i != -1)
        swap(g[u][0], g[u][max_v_i]);
}

void dfs_find_heavy_paths(int u,
                          vector <vector <int> >& g,
                          vector <bool>& used,
                          vector <int>& sz,
                          vector <int>& last_heavy,
                          int last_h = -1) {
    if (used[u]) return;
    last_heavy[u] = last_h;
    used[u] = true;

    for (int v: g[u]) {
        int cur_last_h = -1;
        if (2 * sz[v] >= sz[u]) {
            cur_last_h = ((last_h != -1) ? last_h : u);
        }
        dfs_find_heavy_paths(v, g, used, sz, last_heavy, cur_last_h);
    }
}

void dfs_build(int u,
               vector <vector <int> >& g,
               vector <bool>& used,
               vector <int>& tin,
               int& time,
               vector <int>& path) {
    if (used[u]) return;
    used[u] = true;

    tin[u] = time;
    time++;

    for (int v: g[u]) {
        dfs_build(v, g, used, tin, time, path);
    }
}

int lca(int u, int v, vector <vector <int> >& binup, vector <int>& depth) {
    if (depth[v] < depth[u]) {
        swap(u, v);
    }

    int h = depth[v] - depth[u];
    for (int k = 0; k < 20 && h; k++) {
        if (h % 2) {
            v = binup[k][v];
        }
        h /= 2;
    }

    if (u == v) return u;

    for (int k = 20 - 1; k >= 0; k--) {
        if (binup[k][u] != binup[k][v]) {
            u = binup[k][u];
            v = binup[k][v];
        }
    }

    return binup[0][u];
}

int get_path(int u, int v,
             vector <int>& depth,
             Tree& tree,
             vector <int>& tin,
             vector <int>& p,
             vector <int>& last_heavy,
             vector <int>& a) {
    int ans = 0;
    while (u != v) {
        int w = last_heavy[u];
        if (w == -1) {
            ans = max(ans, a[u]);
            u = p[u];
            continue;
        }
        if (depth[w] < depth[v]) {
            w = v;
        }
        int res = tree.get(tin[w], tin[u]);
        ans = max(ans, res);

        u = w;
    }

    ans = max(ans, a[u]);

    return ans;
}

void solve() {
    int n;
    cin >> n;

    vector <vector <int> > g(n);

    for (int i = 0; i < n - 1; i++) {
        int u, v;
        cin >> u >> v;
        u--;
        v--;

        g[u].push_back(v);
        g[v].push_back(u);
    }

    vector <int> sz(n, 1);
    vector <bool> used(n, false);
    dfs_sz(0, g, used, sz);

    vector <int> p(n, 0);
    vector <int> depth(n, 0);
    fill(used.begin(), used.end(), false);
    dfs_depth(0, g, sz, used, p, 0, depth, 0);

    vector <vector <int> > binup(20, vector <int> (n, 0));
    for (int i = 0; i < n; i++) {
        binup[0][i] = p[i];
    }

    for (int k = 0; k + 1 < 20; k++) {
        for (int i = 0; i < n; i++) {
            binup[k + 1][i] = binup[k][binup[k][i]];
        }
    }

    vector <int> last_heavy(n, -1);

    fill(used.begin(), used.end(), false);
    dfs_find_heavy_paths(0, g, used, sz, last_heavy);

    vector <int> tin(n, -1);

    int time = 0;
    vector <int> path(n);
    fill(used.begin(), used.end(), 0);
    dfs_build(0, g, used, tin, time, path);

    Tree tree(n);

    vector <int> radiation(n, 0);

    int q;
    cin >> q;
    while (q--) {
        string s;
        int u, v;
        cin >> s >> u >> v;

        if (s == "I") {
            u--;
            tree.add(tin[u], v);
            radiation[u] += v;
        } else {
            u--; v--;
            int w = lca(u, v, binup, depth);

            int ans1 = get_path(u, w, depth, tree, tin, p, last_heavy, radiation);
            int ans2 = get_path(v, w, depth, tree, tin, p, last_heavy, radiation);

            cout << max(ans1, ans2) << "\n";
        }
    }
}

int main() {
	ios_base::sync_with_stdio(0);
	cin.tie(0);
	cout.tie(0);
    solve();
}
