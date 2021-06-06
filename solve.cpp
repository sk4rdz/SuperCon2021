#pragma region my_template
#include <bits/stdc++.h>

using namespace std;
using ll = long long;
using pi = pair<int, int>;
using pl = pair<ll, ll>;
using vi = vector<int>;
using vvi = vector<vi>;
using vl = vector<ll>;
using vvl = vector<vl>;

#define range(i, l, r) for(int i = (int)(l); i < (int)(r); i++)
#define rrange(i, l, r) for(int i = (int)(r)-1; i >= (int)(l); i--)
#define rep(i, n) range(i, 0, n)
#define rrep(i, n) rrange(i, 0, n)
#define len(a) ((int)(a).size())
#define all(a) (a).begin(), (a).end()
#define rall(a) (a).rbegin(), (a).rend()
#define uni(a) (a).erase(unique(all(a)), (a).end());

constexpr int INF = 1e9;
constexpr ll LINF = 1e18;

template<typename T>
istream &operator >> (istream &in, vector<T> &a){
    for(T &x: a) in >> x;
    return in;
}
template<typename T, typename U>
istream &operator >> (istream &in, pair<T, U> &a){
    in >> a.first >> a.second;
    return in;
}
template<typename T>
ostream &operator << (ostream &out, const vector<T> &a) {
    rep(i, len(a)) out << a[i] << (i == len(a)-1 ? "" : " ");
    return out;
}
template<typename T, typename U>
ostream &operator << (ostream &out, const pair<T, U> &a){
    out << a.first << " " << a.second;
    return out;
}
inline void print() { cout << "\n"; }
template <typename T, typename ...U>
inline void print(const T &t, const U &...u) {
    cout << t;
    if (sizeof...(u)) cout << " ";
    print(u...);
}
template<typename T> inline bool chmax(T &a, T b) { if (a < b) { a = b; return 1; } return 0; }
template<typename T> inline bool chmin(T &a, T b) { if (a > b) { a = b; return 1; } return 0; }
#pragma endregion

struct Graph {
    int h, w, n;
    vector<vector<pi>> g;
    Graph(int h, int w) : h(h), w(w), n(h*w) {
        g = vector<vector<pi>>(n);
    }
    void add(int sy, int sx, int ty, int tx) {
        g[sy*w+sx].push_back({ty, tx});
    }
    vector<pi> &get(int y, int x) {
        return g[y*w+x];
    }
};

map<char, pi> DIR =
    {{'U', {-1, 0}}, {'D', {1, 0}},
     {'L', {0, -1}}, {'R', {0, 1}}};

int N, M;
vvi board;
vector<string> S;

pi check_order(int y, int x, string &order) {
    if (board[y][x]) return {-1, -1};
    for (char d: order) {
        auto [dy, dx] = DIR[d];
        y += dy, x += dx;
        if (y < 0 or x < 0 or y >= N or x >= N) return {-1, -1};
        if (board[y][x]) return {-1, -1};
    }
    return {y, x};
}

void solve() {
    Graph graph = Graph(N, N);
    
    rep(i, N) rep(j, N) {
        for (string &order: S) {
            auto [ni, nj] = check_order(i, j, order);
            if (ni != -1) graph.add(i, j, ni, nj);
        }
    }
    /*
    rep(i, N) rep(j, N) {
        print(i, j);
        print(graph.get(i, j));
        print("---");
    }
    */
}

void input() {
    cin >> N;
    board = vvi(N, vi(N));
    rep(i, N) {
        int y, x;
        cin >> y >> x;
        board[y-1][x-1] = 1;
    }
    cin >> M;
    S = vector<string>(M);
    cin >> S;
}

int main() {
    cin.tie(0);
    ios::sync_with_stdio(false);
    input();
    solve();
    return 0;
}
