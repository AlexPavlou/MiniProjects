# recursive function to calculate the fibonacci sequence
def fib(n):
    if n == 1 or n == 0:
        return n
    return fib(n - 1) + fib(n - 2)


n = int(input("Up to which fibonacci number should this program print?\nInput: "))
for i in range(0, n):
    print(fib(i))
