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
using Order = vector<pair<char, int>>;

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
#ifdef LOCAL
ofstream dout("./dump.txt");
ofstream dout1("./dump1.txt");
#else
ofstream dout("/dev/null");
ofstream dout1("/dev/null");
#endif
inline void dump() { dout << "\n"; }
template <typename T, typename ...U>
inline void dump(const T &t, const U &...u) {
    dout << t;
    if (sizeof...(u)) dout << " ";
    dump(u...);
}
inline void dump1() { dout1 << "\n"; }
template <typename T, typename ...U>
inline void dump1(const T &t, const U &...u) {
    dout1 << t;
    if (sizeof...(u)) dout1 << " ";
    dump1(u...);
}
template<typename T> inline bool chmax(T &a, T b) { if (a < b) { a = b; return 1; } return 0; }
template<typename T> inline bool chmin(T &a, T b) { if (a > b) { a = b; return 1; } return 0; }
#pragma endregion

class Cumsum2d {
    int h, w;
    vvi data;
public:
    Cumsum2d(int h, int w) : h(h), w(w), data(h+1, vi(w+1)) {}
  
    void build(vvi &s) {
        rep(i, h) rep(j, w) {
            data[i+1][j+1] += data[i][j+1] + data[i+1][j] - data[i][j] + s[i][j];
        }
    }
    int query(int y1, int x1, int y2, int x2) {
        return data[y2][x2] - data[y1][x2] - data[y2][x1] + data[y1][x1];
    }
};

class Graph {
    int h, w, n;
    // ny, nx, order
    vector<vector<tuple<int, int, int>>> g;
public:
    Graph(int h, int w) : h(h), w(w), n(h*w), g(n) {}
    
    void add(int sy, int sx, int ty, int tx, int order) {
        g[sy*w+sx].push_back({ty, tx, order});
    }
    vector<tuple<int, int, int>> &nxt(int y, int x) {
        return g[y*w+x];
    }
};

vector<pi> dir = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
map<char, pi> c2dir =
    {{'U', {-1, 0}}, {'L', {0, -1}},
     {'D', {1, 0}}, {'R', {0, 1}}};

int N, M;
vvi board, reach;
vector<Order> orders;

void check_reachable(vvi &r) {
    queue<pi> q;
    r[0][0] = 1;
    q.push({0, 0});
    while (!q.empty()) {
        auto [y, x] = q.front(); q.pop();
        for (auto [dy, dx]: dir) {
            int ny = y+dy, nx = x+dx;
            if (ny < 0 or nx < 0 or ny >= N or nx >= N or board[ny][nx] or r[ny][nx]) continue;
            r[ny][nx] = 1;
            q.push({ny, nx});
        }
    }
}

pi check_order(int y, int x, Order &order, Cumsum2d &cs) {
    if (board[y][x]) return {-1, -1};
    for (auto [d, val]: order) {
        auto [dy, dx] = c2dir[d];
        dy *= val, dx *= val;
        int ny = y+dy, nx = x+dx;
        if (ny < 0 or nx < 0 or ny >= N or nx >= N) return {-1, -1};
        if (cs.query(min(y, ny), min(x, nx), max(y, ny)+1, max(x, nx)+1)) return {-1, -1};
        y = ny, x = nx;
    }
    return {y, x};
}

pair<bool, vi> solve() {
    reach = vvi(N, vi(N));
    check_reachable(reach);
    
    Cumsum2d cs(N, N);
    cs.build(board);
    
    //グラフ構築
    Graph graph = Graph(N, N);
    rep(i, N) rep(j, N) {
        if (!reach[i][j]) continue;
        rep(k, M) {
            Order &order = orders[k];
            auto [ni, nj] = check_order(i, j, order, cs);
            if (ni != -1) {
                graph.add(i, j, ni, nj, k);
            }
        }
    }
    
    vi ans;
    vvi reach_all_order(N, vi(N));
    queue<pi> q;
    reach_all_order[0][0] = 1;
    q.push({0, 0});
    //BFS
    while (!q.empty()) {
        auto [y, x] = q.front(); q.pop();
        for (auto [ny, nx, k]: graph.nxt(y, x)) {
            if (reach_all_order[ny][nx]) continue;
            reach_all_order[ny][nx] = reach_all_order[y][x]+1;
            ans.push_back(k);
            q.push({ny, nx});
        }
    }
    sort(all(ans)); uni(ans);
    
    bool yesno = true;
    rep(i, N) {
        rep(j, N) {
            if (reach[i][j] and !reach_all_order[i][j]) {
                yesno = false;
                break;
            }
        }
        if (!yesno) break;
    }   
    
    //debug output
    dump(N);
    rep(i, N) dump(reach[i]);
    rep(i, N) dump(reach_all_order[i]);
    
    dump1(M, len(ans));
    rep(i, N) rep(j, N) {
        if (graph.nxt(i, j).empty()) continue;
        for (auto [ni, nj, k]: graph.nxt(i, j)) {
            dump1(i*N+j+1, ni*N+nj+1, k);
        }
    }
    
    return {yesno, ans};
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
    rep(i, M) {
        string s; cin >> s;
        Order order;
        char cur = s[0];
        int cnt = 0;
        for (char c: s) {
            if (c == cur) cnt++;
            else {
                order.push_back({cur, cnt});
                cur = c, cnt = 1;
            }
        }
        order.push_back({cur, cnt});
        orders.push_back(order);
    }
}

void output(bool yesno, vi ans) {
    cout << (yesno ? "YES" : "NO") << "\n";
    cout << len(ans) << "\n";
    for (int k: ans) cout << k << " ";
    cout << endl;
}

int main() {
    cin.tie(0);
    ios::sync_with_stdio(false);
    input();
    auto [yesno, ans] = solve();
    output(yesno, ans);
    return 0;
}
