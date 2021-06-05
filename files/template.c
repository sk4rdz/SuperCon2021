#include <stdio.h>
#include <sys/time.h>

/**
 * 答えが YES のとき yn = 1、 NO のとき yn = 0 で呼び出す。
 * 整数 k は搭載する命令の個数を表す。
 * vs は搭載する命令の情報を持った長さ k のint型配列。
 */
void output(int yn, int k, int *vs) {
  if (!yn) {
    printf("NO\n");
  } else {
    printf("YES\n");
    printf("%d\n", k);
    for (int j = 0; j < k; j++) {
      if (j != 0) printf(" ");
      printf("%d", vs[j]);
    }
    printf("\n");
  }
  fflush(stdout);
}

double get_elapsed_time(struct timeval *begin, struct timeval *end) {
  return (end->tv_sec - begin->tv_sec) * 1000 + (end->tv_usec - begin->tv_usec) / 1000.0;
}

int main() {
  struct timeval t1, t2;
  gettimeofday(&t1, NULL);

  /**
   * ここにプログラム(*)を書く
   */

  gettimeofday(&t2, NULL);
  double t = get_elapsed_time(&t1, &t2);
  // t にプログラム(*) の部分の実行時間（ミリ秒）が代入される
  return 0;
}