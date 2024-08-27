#include <iostream>
#include <vector>

using namespace std;

typedef long long ll;

struct Point {
    ll x;
    ll y;

    Point() {}

    Point(const Point& A, const Point& B) {
        x = B.x - A.x;
        y = B.y - A.y;
    }
};

typedef Point Vector;

ll operator *(const Vector& A, const Vector& B) {
    return A.x * B.x + A.y * B.y;
}

ll operator %(const Vector& A, const Vector& B) {
    return A.x * B.y - A.y * B.x;
}

int sign(ll x) {
    if (x > 0) return 1;
    if (x == 0) return 0;
    return -1;
}

bool intersect(const Point& A, const Point& B,
               const Point& C, const Point& D) {
    const Vector AC(A, C);
    const Vector AB(A, B);
    const Vector AD(A, D);

    if (sign(AB % AC) * sign(AB % AD) >= 0) {
        return false;
    }

    const Vector CA(C, A);
    const Vector CD(C, D);
    const Vector CB(C, B);

    if (sign(CD % CA) * sign(CD % CB) >= 0) {
        return false;
    }

    return true;
}

struct BSPNode {
    vector <int> i;
    vector <Point>& points;
    BSPNode* left = nullptr;
    BSPNode* right = nullptr;

    int rand_j = 0;

    BSPNode(vector <int>& i, vector <Point>& points, int k = 8): i(i), points(points) {
        if (k == 0 || i.size() < 100) {
            return;
        }
        k--;

        vector <int> i_left;
        vector <int> i_right;

        int j = i[rand() % i.size()];
        rand_j = j;

        const Point& A = points[j * 2];
        const Point& B = points[j * 2 + 1];
        const Vector AB(A, B);

        for (int idx: i) {
            const Point& C = points[idx * 2];
            const Point& D = points[idx * 2 + 1];

            const Vector AC(A, C);
            const Vector AD(A, D);

            int s1 = sign(AC % AB);
            int s2 = sign(AD % AB);

            if (s1 == 0 && s2 == 0) {
                if (rand() % 2) {
                    i_left.push_back(idx);
                } else {
                    i_right.push_back(idx);
                }
                continue;
            }

            if (s1 >= 0 && s2 >= 0) {
                i_right.push_back(idx);
                continue;
            }
            if (s1 <= 0 && s2 <= 0) {
                i_left.push_back(idx);
                continue;
            }

            i_right.push_back(idx);
            i_left.push_back(idx);
        }

        left = new BSPNode(i_left, points, k);
        right = new BSPNode(i_right, points, k);
    }

    bool check_all(const Point& C, const Point& D) {
        for (int j: i) {
            const Point& A = points[j * 2];
            const Point& B = points[j * 2 + 1];
            if (intersect(A, B, C, D)) {
                return true;
            }
        }
        return false;
    }

    bool check(const Point& C, const Point& D) {
        if (!left) {
            return check_all(C, D);
        }

        const Point& A = points[rand_j * 2];
        const Point& B = points[rand_j * 2 + 1];
        const Vector AB(A, B);

        const Vector AC(A, C);
        const Vector AD(A, D);

        int s1 = sign(AC % AB);
        int s2 = sign(AD % AB);

        if (s1 == 0 && s2 == 0) {
            return left->check(C, D);
        }

        if (s1 >= 0 && s2 >= 0) {
            return right->check(C, D);
        }

        if (s1 <= 0 && s2 <= 0) {
            return left->check(C, D);
        }

        return left->check(C, D) || right->check(C, D);
    }
};

struct BSPTree {
    BSPTree() {}

    void init(vector <int>& i, vector <Point>& points) {
        root = new BSPNode(i, points);
    }

    bool check(const Point& A, const Point& B) {
        if (!root) return false;
        return root->check(A, B);
    }

    BSPNode *root = nullptr;
};

void solve() {
    int n;
    cin >> n;

    vector <Point> points(2 * n);
    for (int i = 0; i < n; i++) {
        cin >> points[i * 2].x >> points[i * 2].y;
        cin >> points[i * 2 + 1].x >> points[i * 2 + 1].y;
    }

    int block_sz = 1024;
    vector <int> ans;
    vector <int> cache;

    BSPTree tree;
    for (int block_start = 0; block_start < n; block_start += block_sz) {
        for (int i = block_start; i < min(n, block_start + block_sz); i++) {
            const Point& A = points[i * 2];
            const Point& B = points[i * 2 + 1];
            bool old_intersection = tree.check(A, B);

            if (old_intersection) {
                continue;
            }

            bool new_intersection = false;

            for (int j: cache) {
                const Point& C = points[j * 2];
                const Point& D = points[j * 2 + 1];

                if (intersect(A, B, C, D)) {
                    new_intersection = true;
                    break;
                }
            }

            if (new_intersection) continue;

            cache.push_back(i);
        }

        for (int i: cache) {
            ans.push_back(i);
        }
        cache.clear();

        tree.init(ans, points);
    }

    cout << ans.size() << "\n";
    for (int x: ans) {
        cout << x + 1 << ' ';
    } cout << "\n";
}

int main() {
    ios_base::sync_with_stdio(0);
    solve();
}
