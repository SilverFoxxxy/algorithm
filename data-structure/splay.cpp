#include <iostream>
#include <vector>

using namespace std;

typedef long long ll;

struct Node {
    ll x = 0;
    ll sum = 0;
    Node *left = nullptr;
    Node *right = nullptr;
    Node *parent = nullptr;

    Node(int x): x(x), sum(x) {}
};

void print(Node *t, string nd = "\n") {
    if (!t) return;

    cout << "(";
    print(t->left, "");
    cout << ") ";
    cout << t->x << ',' << t->sum << " ";
    cout << "[";
    print(t->right, "");
    cout << "] ";
    cout << nd;
}

inline Node *parent(Node *t) {
    if (!t) return nullptr;
    return t->parent;
}

inline Node *granny(Node *t) {
    return parent(parent(t));
}

inline ll get_x(Node *t) {
    if (!t) return 0;
    return t->x;
}

inline ll get_sum(Node *t) {
    if (!t) return 0;
    return t->sum;
}

inline void update(Node *t) {
    if (!t) return;
    t->sum = t->x + get_sum(t->left) + get_sum(t->right);
}

void splay(Node *t) {
    if (!t) return;

    Node *p;
    while (p = parent(t), p) {
        Node *g = granny(t);

        if (!p) return;

        bool left_p = (p->left == t);

        // zig
        if (!g) {
            if (left_p) {
                p->left = t->right;
                t->right = p;

                if (p->left) p->left->parent = p;
                t->parent = p->parent; // == nullptr
                p->parent = t;
                update(p);
                update(t);
            } else {
                p->right = t->left;
                t->left = p;

                if (p->right) p->right->parent = p;
                t->parent = p->parent; // == nullptr
                p->parent = t;
                update(p);
                update(t);
            }
            continue;
        }

        bool left_g = (g->left == p);

        // zig-zig (t <- p <- g)
        if (left_g && left_p) {
            g->left = p->right;
            if (g->left) g->left->parent = g;

            p->right = g;

            p->left = t->right;
            if (p->left) p->left->parent = p;

            t->right = p;

            t->parent = g->parent;
            p->parent = t;
            g->parent = p;

            update(g);
            update(p);
            update(t);

            if (t->parent) {
                if (t->parent->left == g) {
                    t->parent->left = t;
                } else {
                    t->parent->right = t;
                }
                // update(t->parent); - ?
            }
            continue;
        }

        // zig-zig (g -> p -> t)
        if (!left_g && !left_p) {
            g->right = p->left;
            p->left = g;
            p->right = t->left;
            t->left = p;

            t->parent = g->parent;
            p->parent = t;
            g->parent = p;
            if (g->right) g->right->parent = g;
            if (p->right) p->right->parent = p;

            update(g);
            update(p);
            update(t);

            if (t->parent) {
                if (t->parent->left == g) {
                    t->parent->left = t;
                } else {
                    t->parent->right = t;
                }
                // update(t->parent); - ?
            }
            continue;
        }

        // zig-zag
        if (!left_g && left_p) {
            g->right = t->left;
            p->left = t->right;

            if (g->right) g->right->parent = g;
            if (p->left) p->left->parent = p;

            t->left = g;
            t->right = p;

            t->parent = g->parent;
            p->parent = t;
            g->parent = t;

            update(g);
            update(p);
            update(t);

            if (t->parent) {
                if (t->parent->left == g) {
                    t->parent->left = t;
                } else {
                    t->parent->right = t;
                }
                // update(t->parent); - ?
            }
        }

        if (left_g && !left_p) {
            p->right = t->left;
            g->left = t->right;
            t->left = p;
            t->right = g;

            if (g->left) g->left->parent = g;
            if (p->right) p->right->parent = p;

            t->parent = g->parent;
            p->parent = t;
            g->parent = t;

            update(g);
            update(p);
            update(t);

            if (t->parent) {
                if (t->parent->left == g) {
                    t->parent->left = t;
                } else {
                    t->parent->right = t;
                }
                // update(t->parent); - ?
            }
        }
    }
}

Node *lower_bound(Node *t, int x) {
    Node *runner = t;
    if (!t) return nullptr;

    Node *ans = nullptr;

    while (runner) {
        if (runner->x < x) {
            runner = runner->right;
        } else {
            ans = runner;
            runner = runner->left;
        }
    }

    return ans;
}

pair <Node*, Node*> split(Node *t, int x) {
    if (!t) return {nullptr, nullptr};
    Node *fx = lower_bound(t, x);

    if (!fx) {
        return {t, nullptr};
    }

    splay(fx);

    if (fx->x > x) {
        Node *left = fx->left;
        if (left) left->parent = nullptr;
        fx->left = nullptr;
        update(fx);
        return {left, fx};
    }

    Node *right = fx->right;
    if (right) right->parent = nullptr;
    fx->right = nullptr;
    update(fx);
    return {fx, right};
}

Node *get_max(Node *t) {
    Node *runner = t;
    Node *prev = nullptr;
    while (runner) {
        prev = runner;
        runner = runner->right;
    }
    return prev;
}

Node *merge(Node *t1, Node *t2) {
    if (!t1) return t2;
    if (!t2) return t1;
    auto t1_max = get_max(t1);
    splay(t1_max);

    t1_max->right = t2;
    t2->parent = t1_max;
    update(t1_max);
    return t1_max;
}

pair <Node*, ll> get_sum(Node *t, int l, int r) {
    auto [t1, t23] = split(t, l - 1);
    auto [t2, t3] = split(t23, r);
    ll resp = get_sum(t2);
    t23 = merge(t2, t3);
    t = merge(t1, t23);
    return {t, resp};
}

Node *insert(Node *t, int x) {
    auto fx = lower_bound(t, x);

    if (!t || !fx) {
        Node *new_node = new Node(x);
        return merge(t, new_node);
    }

    if (fx->x == x) {
        splay(fx);
        return fx;
    }

    auto [t1, t2] = split(t, x);

    Node *new_node = new Node(x);

    return merge(merge(t1, new_node), t2);
}

void solve() {
    Node *root = nullptr;

    ll y = 0;
    ll MOD = 1e9;

    int n;
    cin >> n;
    for (int i = 0; i < n; i++) {
        string t;
        cin >> t;
        if (t == "+") {
            int x;
            cin >> x;
            root = insert(root, (x + y) % MOD);

            y = 0;
        } else {
            int l, r;
            cin >> l >> r;

            auto [new_root, sum] = get_sum(root, l, r);

            cout << sum << "\n";
            y = sum;
            root = new_root;
        }
    }
}

int main() {
    ios_base::sync_with_stdio(0);
    solve();
}
