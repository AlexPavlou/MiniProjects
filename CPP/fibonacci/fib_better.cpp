#include <bits/stdc++.h>

using namespace std;

const int MAXN = 1e6+5;

long long Fibonacci[MAXN];

long long fib(int n)
{
    if(n==0 or n==1) return n;
    if(Fibonacci[n] != 0) return Fibonacci[n];
    Fibonacci[n] = fib(n-1) + fib(n-2);
    return Fibonacci[n];
}

int main()
{
    printf("Enter n: ");
    int n;
    scanf("%d", &n);
    for(int i = 0; i<n; ++i)
        printf("n = %d, fib(n) = %lld\n", i, fib(i));
}
