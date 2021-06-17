#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <utility>


using namespace std;


#define N_MAX 255
#define M_MAX 2021
#define INF 1001001001


// 仮定　解のYES/NOは正しい


int N, M;
bool obj[N_MAX][N_MAX];
string S[M_MAX];

string yn;
int K;
int ans[M_MAX];

int basic_ord_dist[N_MAX][N_MAX];
int dy[4] = { 0, -1, 0, 1 }, dx[4] = { -1, 0, 1, 0 };

int ans_ord_dist[N_MAX][N_MAX];
vector<pair<int, int>> G[N_MAX][N_MAX];
int csum_C[N_MAX][N_MAX], csum_R[N_MAX][N_MAX];
string T[M_MAX];


void prob_input() {
    cin >> N;
    for (int i = 0; i < N; i++) {
        int y, x;
        cin >> y >> x;
        y--, x--;
        obj[y][x] = true;
    }
    cin >> M;
    for (int i = 0; i < M; i++) {
        cin >> S[i];
    }
}

bool ans_input() {
    cin >> yn;
    if (yn == "NO") return false;
    cin >> K;
    for (int i = 0; i < K; i++) {
        cin >> ans[i];
        ans[i]--;
    }
    return true;
}

void compress() {
    for (int i = 0; i < M; i++) {
        int sz = 0;
        for (int j = 0; j < S[i].size(); j++) {
            char ch = S[i][j];
            if (sz > 1 && ch == T[i][sz - 2]) {
                int val = (T[i][sz - 1] - '0') + 1;
                T[i][sz - 1] = val + '0';
            } else {
                T[i] += ch;
                T[i] += '1';
                sz += 2;
            }
        }
    }
}

void csum_calc() {
    for (int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            csum_C[i][j] += obj[i][j];
            csum_R[i][j] += obj[i][j];
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            csum_C[i + 1][j] += csum_C[i][j];
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            csum_R[i][j + 1] += csum_R[i][j];
        }
    }
}

void G_build() {
    auto add_edge = [&](int i, int j, int ord) {
        int ci = i, cj = j;
        for (int l = 0; l < T[ord].size(); l += 2) {
            char ch = T[ord][l];
            int val = (T[ord][l + 1] - '0');
            int obj_cnt = INF;
            int ni = ci, nj = cj;
            if (ch == 'R') {
                nj += val;
                if (0 <= nj && nj < N) {
                    obj_cnt = csum_R[ci][nj] - (cj > 0 ? csum_R[ci][cj - 1] : 0);
                }
            } else if (ch == 'L') {
                nj -= val;
                if (0 <= nj && nj < N) {
                    obj_cnt = csum_R[ci][cj] - (nj > 0 ? csum_R[ci][nj - 1] : 0);
                }
            } else if (ch == 'D') {
                ni += val;
                if (0 <= ni && ni < N) {
                    obj_cnt = csum_C[ni][cj] - (ci > 0 ? csum_C[ci - 1][cj] : 0);
                }
            } else {
                ni -= val;
                if (0 <= ni && ni < N) {
                    obj_cnt = csum_C[ci][cj] - (ni > 0 ? csum_C[ni - 1][cj] : 0);
                }
            }
            if (obj_cnt != 0) return;
            ci = ni, cj = nj;
        }
        G[i][j].emplace_back(make_pair(ci, cj));
    };
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (obj[i][j] || basic_ord_dist[i][j] == INF) continue;
            for (int k = 0; k < K; k++) {
                add_edge(i, j, ans[k]);
            }
        }
    }
}

void dist_calc() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            basic_ord_dist[i][j] = ans_ord_dist[i][j] = INF;
        }
    }
    basic_ord_dist[0][0] = ans_ord_dist[0][0] = 0;
    queue<pair<int, int>> que;
    que.emplace(make_pair(0, 0));
    while (!que.empty()) {
        auto u = que.front(); que.pop();
        int cy = u.first, cx = u.second;
        for (int i = 0; i < 4; i++) {
            int ny = cy + dy[i], nx = cx + dx[i];
            if (0 <= ny && ny < N && 0 <= nx && nx < N && basic_ord_dist[ny][nx] == INF && !obj[ny][nx]) {
                basic_ord_dist[ny][nx] = basic_ord_dist[cy][cx] + 1;
                que.emplace(make_pair(ny, nx));
            }
        }
    }
    G_build();
    que.emplace(make_pair(0, 0));
    while (!que.empty()) {
        auto u = que.front(); que.pop();
        int cy = u.first, cx = u.second;
        for (auto v : G[cy][cx]) {
            int ny = v.first, nx = v.second;
            if (ans_ord_dist[ny][nx] == INF) {
                ans_ord_dist[ny][nx] = ans_ord_dist[cy][cx] + 1;
                que.emplace(make_pair(ny, nx));
            }
        }
    }
}

bool is_reachable() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (basic_ord_dist[i][j] == INF) continue;
            if (ans_ord_dist[i][j] == INF) return false;
        }
    }
    return true;
}

void ans_judge() {
    compress();
    csum_calc();
    dist_calc();
    if (is_reachable()) cout << "Answer is true" << endl;
    else cout << "Answer is false" << endl;
}

int main() {
    prob_input();
    if (!ans_input()) {
        cout << "Answer is true" << endl;
        return 0;
    }
    ans_judge();
}
