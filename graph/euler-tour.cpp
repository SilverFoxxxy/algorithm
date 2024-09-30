#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

void euler_tour(
  int u,
  vector <vector <int> >& g,
  vector <int>& edge_cnt,
  vector <int>& path
) {
  while (edge_cnt[u] < g[u].size()) {
    int v = g[u][edge_cnt[u]];
    edge_cnt[u]++;
    euler_tour(v, g, edge_cnt, path);
  }

  path.push_back(u);
}

void solve() {
  int n, m;
  cin >> m >> n;

  vector <vector <int> > g(n);

  for (int i = 0; i < m; i++) {
    int u, v;
    cin >> u >> v;
    u--, v--;
    g[u].push_back(v);
  }

  int start = 0;

  vector <int> edge_count(n, 0);
  vector <int> path;
  euler_tour(start, g, edge_count, path);

  for (auto x: path) {
    cout << x << "\n";
  }
}

int main() {
  solve();
}
