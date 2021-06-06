#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <chrono>

#define N_MIN 2
#define N_MAX 250
#define M_MIN 1
#define M_MAX 2000
#define INF 1001001001

//**/using namespace std;

struct timer {
    int ti;
    void reset() {
        ti = std::clock();
    }
    void elapsed() const {
        std::cout << "Execution Time: " << 1.0 * (std::clock() - ti) / CLOCKS_PER_SEC << "sec" << std::endl;
    }
};

int N, M;
int y[N_MAX], x[N_MAX];
std::string S[M_MAX], T[M_MAX];

int K;
int VS[N_MAX];
bool YN;

int dy[4] = { -1, 0, 1, 0 }, dx[4] = { 0, 1, 0, -1 };
bool block[N_MAX][N_MAX];
int CS_R[N_MAX][N_MAX], CS_C[N_MAX][N_MAX];
std::pair<int, int> mv[M_MAX];
std::vector<std::pair<int, int>> G[N_MAX][N_MAX];

void input() {
    std::cin >> N;
    for (int i = 0; i < N; i++) {
        std::cin >> y[i] >> x[i];
        y[i]--, x[i]--;
        block[y[i]][x[i]] = true;
        CS_R[y[i]][x[i]]++;
        CS_C[y[i]][x[i]]++;
    }
    std::cin >> M;
    for (int i = 0; i < M; i++) {
        std::cin >> S[i];
        for (int j = 0; j < S[i].size(); j++) {
            if (S[i][j] == 'R') {
                mv[i].second++;
            } else if (S[i][j] == 'L') {
                mv[i].second--;
            } else if (S[i][j] == 'U') {
                mv[i].first--;
            } else {
                mv[i].first++;
            }
        }
    }
}

void output() {
    if (!YN) {
        std::cout << "NO" << std::endl;
        return;
    }
    std::cout << "YES\n" << K << '\n';
    for (int i = 0; i < K; i++) {
        if (i > 0) std::cout << ' ';
        std::cout << VS[i];
    }
    std::cout << std::endl;
}

void c_sum() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N - 1; j++) {
            CS_R[i][j + 1] += CS_R[i][j];
        }
    }
    for (int i = 0; i < N - 1; i++) {
        for (int j = 0; j < N; j++) {
            CS_C[i + 1][j] += CS_C[i][j];
        }
    }
}

void compress() {
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < S[i].size(); j++) {
            char c = S[i][j];
            int sz = T[i].size();
            if (sz > 1 && c == T[i][sz - 2]) {
                int val = (T[i][sz - 1] - '0') + 1;
                T[i][sz - 1] = val + '0';
            } else {
                T[i] += c;
                T[i] += '1';
            }
        }
    }
}

bool check(int i, int j, std::string t) {
    for (int k = 0; k < t.size(); k += 2) {
        int w = INF;
        int ni = i, nj = j;
        if (t[k] == 'R') {
            nj += (t[k + 1] - '0');
            if (0 <= i && i < N && 0 <= j && j < N && 0 <= nj && nj < N) {
                w = CS_R[ni][nj] - (j > 0 ? CS_R[ni][j - 1] : 0);
            }
        } else if (t[k] == 'L') {
            nj -= (t[k + 1] - '0');
            if (0 <= i && i < N && 0 <= j && j < N && 0 <= nj && nj < N) {
                w = CS_R[ni][j] - (nj > 0 ? CS_R[ni][nj - 1] : 0);
            }
        } else if (t[k] == 'U') {
            ni -= (t[k + 1] - '0');
            if (0 <= i && i < N && 0 <= j && j < N && 0 <= ni && ni < N) {
                w = CS_C[i][nj] - (ni > 0 ? CS_C[ni - 1][nj] : 0);
            }
        } else {
            ni += (t[k + 1] - '0');
            if (0 <= i && i < N && 0 <= j && j < N && 0 <= ni && ni < N) {
                w = CS_C[ni][nj] - (i > 0 ? CS_C[i - 1][nj] : 0);
            }
        }
        if (w != 0) return false;
        i = ni, j = nj;
    }

    return true;
}

std::vector<std::vector<int>> bfs(bool flag) {
    std::vector<std::vector<int>> dist(N, std::vector<int>(N, INF));
    dist[0][0] = 0;
    std::queue<std::pair<int, int>> que;
    que.emplace(std::make_pair(0, 0));
    if (!flag) {
        while (!que.empty()) {
            std::pair<int, int> p = que.front();
            que.pop();
            for (int i = 0; i < 4; i++) {
                int ny = p.first + dy[i], nx = p.second + dx[i];
                if (0 <= ny && ny < N && 0 <= nx && nx < N && !block[ny][nx] && dist[ny][nx] == INF) {
                    que.emplace(std::make_pair(ny, nx));
                    dist[ny][nx] = dist[p.first][p.second] + 1;
                }
            }
        }
    } else {
        while (!que.empty()) {
            std::pair<int, int> p = que.front();
            que.pop();
            for (auto u : G[p.first][p.second]) {
                int ny = u.first, nx = u.second;
                if (dist[ny][nx] == INF) {
                    que.emplace(std::make_pair(ny, nx));
                    dist[ny][nx] = dist[p.first][p.second] + 1;
                }
            }
        }
    }
    return dist;
}

void solve() {
    c_sum();
    compress();
    YN = true;

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (block[i][j]) continue;
            for (int k = 0; k < M; k++) {
                if (!check(i, j, T[k])) continue;
                G[i][j].emplace_back(std::make_pair(i + mv[k].first, j + mv[k].second));
            }
        }
    }
    std::vector<std::vector<int>> dist0 = bfs(0);
    std::vector<std::vector<int>> dist1 = bfs(1);
    int curr = 0, prev = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++, curr++) {
            if (dist0[i][j] == INF) continue;
            if (dist1[i][j] != INF && dist0[i][j] >= 0 && dist1[i][j] >= 0) {
                prev = curr;
            } else {
                YN = false;
            }
        }
    }

    if (YN) {
        for (int i = 0; i < M; i++) {
            VS[i] = i + 1;
        }
    }
    K = M;
}

int main() {
    //**/timer ti;

    input();
    solve();
    output();

    //**/ti.elapsed();
}
