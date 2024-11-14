ch = input(
    "Choose:\n1)Yearly Tax (U.S)\n2)Yearly Tax (GR)\n3)Rent Tax (GR)\nYour choice: "
)
while True:
    try:
        if int(ch) < 1 or int(ch) > 3:
            raise Exception
        ch = int(ch)
        break
    except:
        print("Try again, your choice must be a positive integer from 1 to 3.")
        ch = input(
            "Choose:\n1)Yearly Tax (U.S)\n2)Yearly Tax (GR)\n3)Rent Tax (GR)\nYour choice: "
        )
if ch == 1:
    # nerdwallet.com data
    ch2 = input(
        "Are you:\n1) a single filer\n2) married, filling jointly\n3) married, filling separately\n4) head of household\nYour choice: "
    )
    while not isinstance(ch2, int):
        try:
            ch2 = int(ch2)
            if ch2 <= 0 or ch2 > 4:
                print("Try again, your choice must be a positive int from 1 to 4")
                continue
            break
        except:
            print("Try again, your choice must be a positive int from 1 to 4")
            ch2 = input(
                "Are you:\n1) a single filer\n2) married, filling jointly\n3) married, filling separately\n4) head of household\nYour choice: "
            )
    income = input("What is your yearly income? ")
    tax = 0
    while not isinstance(income, int):
        try:
            income = int(income)
            if income < 0:
                print("Try again, your income must be a positive integer")
                continue
            break
        except:
            print("Incorrect value entered, your income must be a positive integer")
            income = input("What is your yearly income? ")
    ch+=1
    if ch2 == 1:
        if income <= 11000:
            tax = income * 0.1
        elif income <= 44725:
            tax = 11000 * 0.1 + (income - 110000) * 0.12
        elif income <= 95375:
            tax = 11000 * 0.1 + 33725 * 0.12 + (income - 44725) * 0.22
        elif income <= 182100:
            tax = 11000 * 0.1 + 33725 * 0.12 + 50650 * 0.22 + (income - 95375) * 0.24
        elif income <= 231250:
            tax = (
                11000 * 0.1
                + 33725 * 0.12
                + 50650 * 0.22
                + 86725 * 0.24
                + (income - 182100) * 0.32
            )
        elif income <= 578125:
            tax = (
                11000 * 0.1
                + 33725 * 0.12
                + 50650 * 0.22
                + 86725 * 0.24
                + 49150 * 0.32
                + (income - 231250) * 0.35
            )
        else:
            tax = (
                11000 * 0.1
                + 33725 * 0.12
                + 50650 * 0.22
                + 86725 * 0.24
                + 49150 * 0.32
                + 346875 * 0.35
                + (income - 578125) * 0.37
            )
    elif ch2 == 2:
        if income <= 22000:
            tax = income * 0.1
        elif income <= 89450:
            tax = 22000 * 0.1 + (income - 22000) * 0.12
        elif income <= 190750:
            tax = 2200 + 67450 * 0.12 + (income - 89450) * 0.22
        elif income <= 364200:
            tax = 2200 + 67450 * 0.12 + 101300 * 0.22 + (income - 190750) * 0.24
        elif income <= 462500:
            tax = (
                2200
                + 67450 * 0.12
                + 101300 * 0.22
                + 173449 * 0.24
                + (income - 364200) * 0.32
            )
        elif income <= 693751:
            tax = (
                2200
                + 67450 * 0.12
                + 101300 * 0.22
                + 173449 * 0.24
                + 98300 * 0.32
                + (income - 462500) * 0.35
            )
        else:
            tax = (
                2200
                + 67450 * 0.12
                + 101300 * 0.22
                + 173449 * 0.24
                + 98300 * 0.32
                + 231249 * 0.35
                + (income - 693751) * 0.37
            )
    elif ch2 == 3:
        if income <= 11000:
            tax = income * 0.1
        elif income <= 44725:
            tax = 1100 + (income - 11000) * 0.12
        elif income <= 95375:
            tax = 1100 + 33725 * 0.12 + (income - 44725) * 0.22
        elif income <= 182100:
            tax = 1100 + 33725 * 0.12 + 50649 * 0.22 + (income - 95375) * 0.24
        elif income <= 231250:
            tax = (
                1100
                + 33725 * 0.12
                + 50649 * 0.22
                + 86725 * 0.24
                + (income - 182100) * 0.32
            )
        elif income <= 346875:
            tax = (
                1100
                + 33725 * 0.12
                + 50649 * 0.22
                + 86725 * 0.24
                + 49149 * 0.32
                + (income - 231250) * 0.35
            )
        else:
            tax = (
                1100
                + 33725 * 0.12
                + 50649 * 0.22
                + 86725 * 0.24
                + 49149 * 0.32
                + 115624
                + (income - 346875) * 0.37
            )
    else:
        if income <= 15700:
            tax = income * 0.1
        elif income <= 59850:
            tax = 1570 + (income - 15700) * 0.12
        elif income <= 95350:
            tax = 1570 + 44150 * 0.12 + (income - 59850) * 0.22
        elif income <= 182100:
            tax = 1570 + 44150 * 0.12 + 35499 * 0.22 + (income - 95350) * 0.24
        elif income <= 231250:
            tax = (
                1570
                + 44150 * 0.12
                + 35499 * 0.22
                + 86749 * 0.24
                + (income - 182100) * 0.32
            )
        elif income <= 578100:
            tax = (
                1570
                + 44150 * 0.12
                + 35499 * 0.22
                + 86749 * 0.24
                + 49149 * 0.32
                + (income - 231500) * 0.35
            )
        else:
            tax = (
                1570
                + 44150 * 0.12
                + 35499 * 0.22
                + 86749 * 0.24
                + 49149 * 0.32
                + 346849 * 0.35
                + (income - 578100) * 0.37
            )
    print("You are called to pay: " + str(tax) + " in taxes")
elif ch == 2:
    tax = 0
    income = input("What is your yearly income? ")
    while not isinstance(income, int):
        try:
            income = int(income)
            if income < 0:
                print("Try again, your income must be a positive integer")
                continue
            break
        except:
            print("Incorrect value entered, your income must be a positive integer")
            income = input("What is your yearly income? ")
    t = input("Are you:\n1) Self-employed\n2) An employee\nYour choice: ")
    while not isinstance(t, int):
        try:
            t = int(t)
            if t != 1 or t != 2:
                print("Try again")
                continue
            break
        except:
            print(
                "Incorrect value entered, your income must be a positive integer, either 1 or 2"
            )
            t = input("Are you:\n1) Self-employed\n2) An employee\nYour choice: ")
    if t == 1:
        if income <= 10000:
            tax = 0
        elif income <= 13400:
            tax = (income - 10000) * 0.15
        elif income <= 23400:
            tax = 510 + (income - 13400) * 0.3
        else:
            tax = 3510 + (income - 23400) * 0.4
    else:
        if income <= 8400:
            tax = 0
        elif income <= 13400:
            tax = (income - 8400) * 0.15
        elif income <= 23400:
            tax = 750 + (income - 13400) * 0.3
        else:
            tax = 3750 + (income - 23400) * 0.4
    print("Your total tax according to the tax table for 2023 is: " + str(tax))
else:
    income = input("What is your yearly income from rentals? ")
    tax = 0
    while not isinstance(income, int):
        try:
            income = int(income)
            if income < 0:
                print("Try again, your income must be a positive integer")
                continue
            break
        except:
            print("Incorrect value entered, your income must be a positive integer")
            income = input("What is your yearly income? ")
    if income <= 12000:
        tax = income * 0.15
    elif income <= 35000:
        tax = 1800 + (income - 12000) * 0.35
    else:
        tax = 9850 + (income - 35000) * 0.45
    print("Your total tax from rentals is: " + str(tax))
