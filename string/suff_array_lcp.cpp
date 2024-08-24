#include <iostream>
#include <stdio.h>
#include <vector>

using namespace std;

typedef long long ll;

void recalc(int n,
            vector <int>& suff_array,
            vector <int>& color,
            int len) {
    int max_color = 0;
    for (auto x: color) max_color = max(x, max_color);

    vector <vector <pair <int, int> > > counting_array(max_color + 1);

    for (auto j: suff_array) {
        int i = (j - len + n) % n;
        counting_array[color[i]].push_back({i, color[j]});
    }

    vector <int> new_suff_array;

    int color_cnt = 0;
    for (auto block: counting_array) {
        if (block.empty()) continue;

        int last_color2 = block[0].second;
        for (auto p: block) {
            int i = p.first;
            int color2 = p.second;

            color_cnt += (color2 != last_color2);
            last_color2 = color2;

            new_suff_array.push_back(i);
            color[i] = color_cnt;
        }

        color_cnt++;
    }

    swap(suff_array, new_suff_array);
}

vector <int> getSuffixArray(const string& t) {
    string s = t;
    s += '#';

    int n = s.size();

    vector <vector <int> > counting_array(256);

    for (int i = 0; i < n; i++) {
        counting_array[s[i]].push_back(i);
    }

    vector <int> color(n);
    vector <int> suff_array;

    int color_cnt = 0;
    for (auto block: counting_array) {
        if (block.empty()) continue;

        for (int i: block) {
            suff_array.push_back(i);
            color[i] = color_cnt;
        }
        color_cnt++;
    }

    for (int len = 1; len < n; len *= 2) {
        recalc(n, suff_array, color, len);
    }

    suff_array.erase(suff_array.begin());

    return suff_array;
}

void solve() {
    string s;
    cin >> s;

    int n = s.size();

    vector <int> suff_array = getSuffixArray(s);
    vector <int> pos(n);

    for (int i = 0; i < n; i++) {
        pos[suff_array[i]] = i;
    }

    vector <int> lcp(n);

    int l = 0;

    for (int i = 0; i < n; i++) {
        if (pos[i] == n - 1) continue;

        int j = suff_array[pos[i] + 1];

        int idx1 = i + l;
        int idx2 = j + l;

        while (idx1 < n && idx2 < n && s[idx1] == s[idx2]) {
            l++;
            idx1++; idx2++;
        }

        lcp[pos[i]] = l;

        if (l) l--;
    }

    ll ans = n - suff_array[0];
    for (int j = 0; j + 1 < n; j++) {
        ans += n - suff_array[j + 1];
        ans -= lcp[j];
    }

    cout << ans << endl;
}

int main() {
    solve();
}
