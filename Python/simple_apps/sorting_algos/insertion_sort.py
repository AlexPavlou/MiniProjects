def sort(arr, n):
    for i in range(n):
        m = arr[i]
        ptr = i
        ptr2 = i
        for y in range(i, n):
            if m > arr[y]:
                m = arr[y]
                ptr2 = y
        arr[i], arr[ptr2] = arr[ptr2], arr[i]
    return arr


arr = [5, 2, 9, 1, 11, 8, 0, 5]
print(arr)
n = len(arr)
sort(arr, n)
print("sorted arr: " + str(arr))
