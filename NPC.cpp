#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <queue>
#include <string>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>
#include <sys/time.h>

using namespace std;
using pi = pair<int, int>;
using ti = tuple<int, int, int>;
using vi = vector<int>;
using vvi = vector<vi>;
using Order = vector<pair<char, int>>;

#define rep(i, n) for(int i = 0; i < (int)(n); i++)
#define all(a) (a).begin(), (a).end()
#define len(a) ((int)(a).size())

class XorShift {
    unsigned x, y, z, w; 
public:    
    XorShift() {
        x = 123456789;
        y = 362436069;
        z = 521288629;
        w = 88675123;
    }
    inline unsigned next() {
        unsigned t = x ^ (x << 11);
        x = y; y = z; z = w;
        return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
    }
    unsigned nextInt(int n) {
        return next() % n;
    }
    unsigned nextInt(int l, int r) {
        return l + (next() % (r - l));
    }
    double nextDouble() {
        return next() / 4294967295.0;
    }
};

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

template<typename T>
class BIT {
    int N;
    vector<T> data;
public:
    BIT(int n) : N(n+1), data(N) {}
    T sum(int r) {
        T res = 0;
        for (int i = r; i > 0; i -= i & -i) {
            res += data[i];
        }
        return res;
    }
    T sum() { return sum(N-1); }
    T sum(int l, int r) {
        return sum(r) - sum(l);
    }
    void add(int idx, T x = 1) {
        for(int i = idx+1; i < N; i += i & -i) {
            data[i] += x;
        }
    }
    int lower_bound(T val) {
        int x = 0, r = 1;
        while (r < N) r = r << 1;
        for (int k = r; k > 0; k = k >> 1) {
            if (x + k < N && data[x + k] < val) {
                val -= data[x + k];
                x += k;
            }
        }
        return x;
    }
};

class Graph {
    int h, w, n;
    vector<vector<pi>> g;
public:
    Graph(int h, int w) : h(h), w(w), n(h*w), g(n) {}
    
    void add(int sy, int sx, int ty, int tx, int order) {
        g[sy*w+sx].push_back({ty*w+tx, order});
    }
    vector<pi> &nxt(int v) {
        return g[v];
    }
    pi decomp(int v) {
        return {v / w, v % w};
    }
};

inline double get_elapsed_time(struct timeval *begin, struct timeval *end) {
  return (end->tv_sec - begin->tv_sec) * 1000 + (end->tv_usec - begin->tv_usec) / 1000.0;
}
struct timeval t1, t2;

// vector内の任意の要素をO(1)で削除できる（並び順はめちゃくちゃになる）
template<typename T>
inline void fast_erase(vector<T> &a, const int i) {
    swap(a[i], a.back());
    a.pop_back();
}

template<typename T>
inline bool is_exists(const unordered_set<T> &s, const T val) {
    return s.find(val) != s.end();
}

vector<pi> dir = {{-1, 0}, {0, -1}, {1, 0}, {0, 1}};
map<char, pi> c2dir =
    {{'U', {-1, 0}}, {'L', {0, -1}},
     {'D', {1, 0}}, {'R', {0, 1}}};

constexpr double TIME_LIM = 9400.0;
constexpr double OUTPUT_TIME1 =  8000.0; //時間超過時の保険用

XorShift rnd;

int N, M;
vvi board;
vector<Order> orders;

int find_reachable(vvi &r) {
    queue<pi> q;
    r[0][0] = 1;
    int num = 1;
    q.push({0, 0});
    while (!q.empty()) {
        auto [y, x] = q.front(); q.pop();
        for (auto [dy, dx]: dir) {
            int ny = y+dy, nx = x+dx;
            if (ny < 0 or nx < 0 or ny >= N or nx >= N or board[ny][nx] or r[ny][nx]) continue;
            r[ny][nx] = 1;
            num++;
            q.push({ny, nx});
        }
    }
    return num;
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

bool yesno;
unordered_set<int> answer;

void output() {
    if (yesno) {
        cout << "YES" << "\n";
        cout << len(answer) << "\n";
        vi ans(all(answer));
        sort(all(ans));
        rep(i, len(ans)) {
            if (i != 0) cout << " ";
            cout << ans[i]+1;
        }
        cout << endl;
    } else {
        cout << "NO" << endl;
    }
}

//辺の候補の中から既に使った色の物を優先してランダムに選ぶ
pi random_choice(vvi &cands, BIT<int> &clen, unordered_set<int> &used_c, int &used_sum) {
    int color = -1, index = -1;
    
    if (used_sum != 0) {
        int idx = rnd.nextInt(used_sum);
        for (int c: used_c) {
            int ln = len(cands[c]);
            if (idx >= ln) {
                idx -= ln;
            } else {
                color = c, index = idx;
                break;
            }
        }
        used_sum--;
    } else {
        int sm = clen.sum();
        if (sm == 0) return {-1, -1};
        int idx = rnd.nextInt(sm);
        int c = clen.lower_bound(idx+1);
        idx -= clen.sum(c);
        color = c, index = idx;
    }
    int nv = cands[color][index];
    fast_erase(cands[color], index);
    clen.add(color, -1);
    return {nv, color};
}

// 以下の問題を解きたい
/*
頂点数P(P <= N^2), 辺数Q(Q <= P*M)の有向グラフが与えられる．
任意の頂点から伸びる辺には、それぞれ異なる色k(1 <= k <= 2000)が付いている．
頂点0から辺を辿って指定された全ての頂点に辿り着けるグラフが存在するか判定し、
存在する場合は "YES" と出力、さらにそのうち最も使用する色の数が出来る限り少ないものを構築し、その色の組み合わせを出力せよ． 存在しない場合は "NO" と出力せよ．
*/
void find(const int n, const int r_num, const int c_num, Graph &g, unordered_set<int> &ans) {
    int bscore = M;
    
    int step = 0;
    bool out1_flag = false;
    
    while (true) {
        vi visited(n);
        visited[0] = 1;
        int visited_num = 1;
        vvi cands(c_num);
        BIT<int> cands_len(c_num);
        unordered_set<int> used_c;
        int used_sum = 0;
        
        for (auto [nv, k]: g.nxt(0)) {
            if (!visited[nv]) {
                cands[k].push_back(nv);
                cands_len.add(k);
            }
        }
        while (visited_num < r_num) {
            pi res;
            res = random_choice(cands, cands_len, used_c, used_sum);
            if (res.first == -1) break;
            auto [v, k] = res;
            
            if (visited[v]) continue;
            
            visited[v] = 1;
            visited_num++;
            if (!is_exists(used_c, k)) {
                used_c.insert(k);
                used_sum += len(cands[k]);
            }
            
            for (auto [nv, k]: g.nxt(v)) {
                if (!visited[nv]) {
                    cands[k].push_back(nv);
                    cands_len.add(k);
                    if (is_exists(used_c, k)) {
                        used_sum++;
                    }
                }
            }
        }
        
        int score = len(used_c);
        
        if (bscore > score) {
            bscore = score;
            ans = used_c;
        }
        
        gettimeofday(&t2, NULL);
        if (step == 0) output();
        double time = get_elapsed_time(&t1, &t2);
        if (time >= TIME_LIM) break;
        if (!out1_flag && time >= OUTPUT_TIME1) {
            output(); out1_flag = true;
        }
        step++;
    }
}

void solve() {
    vvi reachable = vvi(N, vi(N));
    int reachable_num = find_reachable(reachable);
    
    Cumsum2d cs(N, N);
    cs.build(board);
    
    //グラフ構築
    Graph graph = Graph(N, N);
    rep(i, N) rep(j, N) {
        if (!reachable[i][j]) continue;
        rep(k, M) {
            Order &order = orders[k];
            auto [ni, nj] = check_order(i, j, order, cs);
            if (ni != -1) {
                graph.add(i, j, ni, nj, k);
            }
        }
    }
    
    vvi reach(N, vi(N));
    reach[0][0] = 1;
    queue<int> q; q.push(0);
    
    //BFS (yes/no判定)
    while (!q.empty()) {
        auto v = q.front(); q.pop();
        for (auto [nv, k]: graph.nxt(v)) {
            auto [ny, nx] = graph.decomp(nv);
            if (reach[ny][nx]) continue;
            reach[ny][nx] = 1;
            q.push(nv);
        }
    }
    
    yesno = true;
    rep(i, N) {
        rep(j, N) {
            if (reachable[i][j] != reach[i][j]) {
                yesno = false;
                break;
            }
        }
        if (!yesno) return;
    }
    
    find(N*N, reachable_num, M, graph, answer);
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

int main() {
    gettimeofday(&t1, NULL);
    cin.tie(0);
    ios::sync_with_stdio(false);
    input(); solve(); output();
    return 0;
}
