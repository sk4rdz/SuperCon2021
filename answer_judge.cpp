#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <queue>

using namespace std;

#define N_MAX 255
#define M_MAX 2021
#define INF 1001001001

// 解のYES/NOは正しいことを仮定

int N, M, K;
int y[N_MAX], x[N_MAX], ans[M_MAX];
int dist_basic_ord[N_MAX][N_MAX], cs_c[N_MAX][N_MAX], cs_r[N_MAX][N_MAX], dist_ans_ord[N_MAX][N_MAX];
bool obj[N_MAX][N_MAX];
string S[M_MAX], T[M_MAX];
vector<pair<int, int>> G[N_MAX][N_MAX];

void prob_input() {
    cin >> N;
    for (int i = 0; i < N; i++) {
        cin >> y[i] >> x[i];
        y[i]--; x[i]--;
        cs_c[y[i]][x[i]]++;
        cs_r[y[i]][x[i]]++;
        obj[y[i]][x[i]] = true;
    }
    cin >> M;
    for (int i = 0; i < M; i++) {
        cin >> S[i];
    }
}

void compress() {
    for (int i = 0; i < M; i++) {
        int sz = 0;
        for (int j = 0; j < S[i].size(); j++) {
            char c = S[i][j];
            if (sz > 0 && c == T[i][sz - 2]) {
                int val = (T[i][sz - 1] - '0') + 1;
                T[i][sz - 1] = val + '0';
            } else {
                T[i] += c;
                T[i] += '1';
                sz += 2;
            }
        }
    }
}

void csum() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cs_c[i + 1][j] += cs_c[i][j];
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cs_r[i][j + 1] += cs_r[i][j];
        }
    }
}

void bfs() {
    for (int i = 0; i < N_MAX; i++) {
        for (int j = 0; j < N_MAX; j++) {
            dist_basic_ord[i][j] = INF;
        }
    }
    dist_basic_ord[0][0] = 0;
    queue<pair<int, int>> que;
    que.emplace(make_pair(0, 0));
    int dy[4] = { -1, 0, 1, 0 }, dx[4] = { 0, 1, 0, -1 };
    while (!que.empty()) {
        auto u = que.front(); que.pop();
        int cy = u.first, cx = u.second;
        for (int i = 0; i < 4; i++) {
            int ny = cy + dy[i], nx = cx + dx[i];
            if (0 <= ny && ny < N && 0 <= nx && nx < N && dist_basic_ord[ny][nx] == INF && !obj[ny][nx]) {
                dist_basic_ord[ny][nx] = dist_basic_ord[cy][cx] + 1;
                que.emplace(make_pair(ny, nx));
            }
        }
    }
}

void add_edge(int i, int j, int k) {
    int ci = i, cj = j;
    for (int l = 0; l < T[k].size(); l += 2) {
        int ni = ci, nj = cj;
        int obj_cnt = INF;
        if (T[k][l] == 'R') {
            nj += (T[k][l + 1] - '0');
            if (0 <= nj && nj < N) {
                obj_cnt = cs_r[ci][nj] - (cj > 0 ? cs_r[ci][cj - 1] : 0);
            }
        } else if (T[k][l] == 'L') {
            nj -= (T[k][l + 1] - '0');
            if (0 <= nj && nj < N) {
                obj_cnt = cs_r[ci][cj] - (nj > 0 ? cs_r[ci][nj - 1] : 0);
            }
        } else if (T[k][l] == 'D') {
            ni += (T[k][l + 1] - '0');
            if (0 <= ni && ni < N) {
                obj_cnt = cs_c[ni][cj] - (ci > 0 ? cs_c[ci - 1][cj] : 0);
            }
        } else {
            ni -= (T[k][l + 1] - '0');
            if (0 <= ni && ni < N) {
                obj_cnt = cs_c[ci][cj] - (ni > 0 ? cs_c[ni - 1][cj] : 0);
            }
        }
        if (obj_cnt != 0) return;
        ci = ni, cj = nj;
    }
    G[i][j].emplace_back(make_pair(ci, cj));
}

bool is_reachable() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            dist_ans_ord[i][j] = INF;
        }
    }
    dist_ans_ord[0][0] = 0;
    queue<pair<int, int>> que;
    que.emplace(make_pair(0, 0));
    while (!que.empty()) {
        auto u = que.front(); que.pop();
        int cy = u.first, cx = u.second;
        for (auto v : G[cy][cx]) {
            int ny = v.first, nx = v.second;
            if (dist_ans_ord[ny][nx] == INF) {
                dist_ans_ord[ny][nx] = dist_ans_ord[cy][cx] + 1;
                que.emplace(make_pair(ny, nx));
            }
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (dist_basic_ord[i][j] != INF && dist_ans_ord[i][j] == INF) return false;
        }
    }
    return true;
}

bool ans_judge() {
    cin >> K;
    for (int i = 0; i < K; i++) {
        cin >> ans[i];
        ans[i]--;
    }
    compress();
    csum();
    bfs();
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (obj[i][j] || dist_basic_ord[i][j] == INF) continue;
            for (int k = 0; k < K; k++) {
                add_edge(i, j, ans[k]);
            }
        }
    }
    return is_reachable();
}

void ans_input() {
    string yn;
    cin >> yn;
    if (yn == "YES" && !ans_judge()) {
        cout << "Answer is false." << endl;
    } else {
        cout << "Answer is true." << endl;
    }
}

int main() {
    prob_input();
    ans_input();
}
