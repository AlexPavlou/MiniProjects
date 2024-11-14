"""This app receives two dates, and calculates the time that will pass until the second date."""
from datetime import date


def check_date(date):
    """check if the date that the program received as input is valid"""
    for i in date:
        i = int(i)
        if i <= 0 and i != int(date[-1]):
            raise Exception


date1 = str(date.today()).split("-")
# d: current year
# e: current month
# f: current day
print(date1)
d = int(date1[2])
e = int(date1[1])
f = int(date1[0])

response = input("Enter today as the date to count from? ")
if response.startswith("y") or response.startswith("Y"):
    x = d
    y = e
    z = f
else:
    date1 = input("Enter the date you wish to count from (format: Day-Month-Year): ")
    date1 = date1.split("-")
    x = int(date1[0])
    y = int(date1[1])
    z = int(date1[2])
    check_date(date1)

response = input("Enter today as the date to count to? ")
if response.startswith("y") or response.startswith("Y"):
    a = d
    b = e
    c = f
else:
    date2 = input("Enter the date you wish to count to (format: Day-Month-Year): ")
    date2 = date2.split("-")
    a = int(date2[0])
    b = int(date2[1])
    c = int(date2[2])
    check_date(date2)
    if c < z or (c == z and b < y) or (c == z and b == y and a < x):
        raise Exception

if f < z or (f == z and y < e) or (f == z and y == e and x < d):
    future = False
else:
    future = True

a_dict = {}
if a >= x:
    a_dict["days"] = a - x
else:
    a_dict["days"] = 30 - (x - a)

if y <= b:
    a_dict["months"] = b - y
else:
    a_dict["months"] = 12 - (y - b)

a_dict["years"] = c - z

if a_dict["days"] != 0 or a_dict["months"] != 0 or a_dict["years"] != 0:
    if future:
        print("It will be in.. ")
    else:
        print("It has been.. ")
b = 0
y = 0
for value in a_dict.items():
    if str(value) != "0":
        b = 1 + b
for key, value in a_dict.items():
    y = y + 1
    if str(value) != "0":
        if y == b:
            print("and ", str(value), " ", key)
        else:
            print(str(value), " ", key, " ")
