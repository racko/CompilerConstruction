#ifndef _RANDOM_H_
#define _RANDOM_H_

struct Ran {
  unsigned long long u, v = 4101842887655102017LL, w = 1;

  Ran(unsigned long long j) {
    u = j ^ v; int64();
    v = u; int64();
    w = v; int64();
  }

  unsigned long long int64() {
    u = u * 2862933555777941757LL + 7046029254386353087LL;
    v ^= v >> 17; v ^= v << 31; v ^= v >> 8;
    w = 4294957665U * (w & 0xffffffff) + (w >> 32);
    unsigned long long x = u ^ (u << 21); x ^= x >> 35; x ^= x << 4;
    return (x + v) ^ w;
  }

  double nextDouble() {
    return 5.42101086242752217E-20 * int64();
  }

  unsigned int nextInt() {
    return (unsigned int)(int64());
  }
};

struct Ranq1 {
  unsigned long long v = 4101842887655102017LL;

  Ranq1(unsigned long long int j) {
    v ^= j;
    v = int64();
  }

  unsigned long long int int64() {
    v ^= v >> 21; v ^= v << 35; v ^= v >> 4;
    return v * 2685821657736338717LL;
  }

  double nextDouble() {
    return 5.42101086242752217E-20 * int64();
  }

  unsigned int nextInt() {
    return (unsigned int)(int64());
  }
};

struct Ranq2 {
  unsigned long long v = 4101842887655102017LL, w = 1;

  Ranq2(unsigned long long j) {
    v ^= j;
    w = int64();
    v = int64();
  }

  unsigned long long int64() {
    v ^= v >> 17; v ^= v << 31; v ^= v >> 8;
    w = 4294957665U * (w & 0xffffffff) + (w >> 32);
    return v ^ w;
  }

  double nextDouble() {
    return 5.42101086242752217E-20 * int64();
  }

  unsigned int nextInt() {
    return (unsigned int)(int64());
  }
};

#endif
