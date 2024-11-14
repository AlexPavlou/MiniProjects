def sort(arr, n):
    for i in range(n):
        key = arr[i]
        j = i - 1
        while j >= 0 and key < arr[j]:
            arr[j + 1] = arr[j]
            j = j - 1
        arr[j + 1] = key
    return arr


arr = [5, 2, 9, 1, 11, 8, 0, 5]
print(arr)
n = len(arr)
sort(arr, n)
print("sorted arr: " + str(arr))
