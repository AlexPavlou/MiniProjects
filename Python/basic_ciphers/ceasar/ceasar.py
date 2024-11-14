shift = input("Enter a character shift: ")
symbols = """ !@#$%^&*()-_+=]}{[\|";:/?.>,<~`'"""
while True:
    try:
        shift = int(shift)
        break
    except:
        print("Wrong, must be int")
        shift = input("Enter a character shift: ")
if shift > 26:
    shift = shift % 26
string = input("Enter your string: ")
s = ""
for i in string.upper():
    if i not in symbols:
        if ord(i) + shift > 90:
            s += chr(ord(i) + shift - 26)
        else:
            s += chr(ord(i) + shift)
    else:
        s += i
print(s)
