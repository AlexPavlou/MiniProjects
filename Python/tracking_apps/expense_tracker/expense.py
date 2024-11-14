import os
from datetime import datetime

# This program keeps track of the user's transactions and total money. datetime is used to mark each 'transaction' with a time date

if os.path.exists(".track"):
    f = open(".track", "r")
    balance = int(f.readline())
    f.close()
else:
    f = open(".track", "w")
    while True:
        balance = input("Balance: ")
        try:
            balance = int(balance)
            if balance < 0:
                raise Exception
            break
        except:
            print("Your balance must be a positive integer or 0")
    f.write(str(balance))
    f.close()

while True:
    print("Balance: " + str(balance))
    while True:
        ch = input(
            "1) Add to balance\n2) Subtract from balance\n3) Show previous transactions\nYour choice: "
        )
        try:
            ch = int(ch)
            if ch not in [1, 2, 3]:
                raise Exception
            break
        except:
            print("Your choice must be either 1,2 or 3")
    if ch == 1:
        fund = input("How much money should be added? ")
        while True:
            try:
                fund = int(fund)
                if fund <= 0:
                    raise Exception
                break
            except:
                print("Your choice must be a positive integer")
        balance += fund
        desc = input("Should a description be included? ")
        if desc.startswith("y"):
            desc = True
        else:
            desc = False
        date = str(datetime.now())
        print("Balance: " + str(balance))
        print("Date: " + date)
        f = open(".track", "a")
        f.write("\nDate: " + date + " - Added: " + str(fund))
        f.close()
        if desc:
            desc = input("Description: ")
            print("Description: " + desc)
            f = open(".track", "a")
            f.write(" - Description: " + desc)
            f.close()
    elif ch == 2:
        cost = input("What cost should be subtracted from your balance? ")
        while True:
            try:
                cost = int(cost)
                if cost <= 0:
                    raise Exception
                break
            except:
                print("The must be a positive integer")
        balance -= cost
        desc = input("Should a description be included? ")
        if desc.startswith("y"):
            desc = True
        else:
            desc = False
        date = str(datetime.now())
        print("Balance: " + str(balance))
        print("Date: " + date)
        f = open(".track", "a")
        f.write("\nDate: " + date + " - Subtracted: " + str(cost))
        f.close()
        if desc:
            desc = input("Description: ")
            print("Description: " + desc)
            f = open(".track", "a")
            f.write(" - Description: " + desc)
            f.close()
    if ch in [1, 2]:
        f = open(".track", "r")
        lines = f.read().splitlines()
        f.close()
        f = open(".track", "w")
        f.write(str(balance))
        for i in lines[1:]:
            f.write("\n" + i)
        f.close()
    elif ch == 3:
        f = open(".track", "r")
        lines = f.read().splitlines()
        f.close()
        for i in lines:
            print(i)
