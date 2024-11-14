# Imports roman module, it allows you to convert integers to roman numerals and vice versa
import roman

# This cipher takes the ascii value of each character in a string and converts that value to roman numbers

s = ""
ans = input("Do you want to: 1) Encode\n2) Decode?\nYour choice: ")
while True:
    try:
        ans = int(ans)
        if ans != 1 and ans != 2:
            print("Enter either 1 or 2")
            continue
        break
    except:
        print("Enter either 1 or 2")
        ans = input("Do you want to: 1) Encode\n2) Decode?\nYour choice: ")
if ans == 1:
    st = input("The string to be encrypted using the 'romancrap' cipher: ")
    for i in st:
        if i != " ":
            s += str(roman.toRoman(ord(i))) + " "
else:
    st = input("The string to be decrypted using the 'romancrap' cipher: ")
    st = st.split()
    for i in st:
        if i != " ":
            s += chr(roman.fromRoman(i)) + " "
print(s)
