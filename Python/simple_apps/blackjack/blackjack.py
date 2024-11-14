import random, os

# Checks to see if .total exists, the file that holds the user's total after each bet

if os.path.exists(".total"):
    f = open(".total", "r")
    total = f.readline()
    total = int(total)
    f.close()
else:
    # it otherwise creates the file and sets the user total money to 50
    with open(".total", "w") as f:
        f.write("50")
        total = 50

# Possible value of cards, values{} is used to match the card the user received with its corresponding value
deck = ["A", "2", "3", "4", "5", "6", "7", "8", "9", "J", "Q", "K"]
values = {
    "A": 11,
    "6": 6,
    "2": 2,
    "3": 3,
    "4": 4,
    "5": 5,
    "7": 7,
    "8": 8,
    "9": 9,
    "J": 10,
    "Q": 10,
    "K": 10,
}
# starts scores, dealer has a slight advantage
user = random.randint(10, 17)
dealer = random.randint(6, 13)
# receives a bet from the user, which must be a positive integer, and not a string or negative number
while True:
    bet = input("Total: " + str(total) + "\nEnter your bet: ")
    try:
        bet = int(bet)
        if bet < 0 or bet > total:
            print(
                "Your bet must be a positive int no larger than your total of: "
                + str(total)
            )
        elif total == 0:
            ch = "n"
            while not (ch.startswith("y")) or not (ch.startswith("Y")):
                ch = input(
                    "You are out of money, do you wish to beg for it? (y or n)\nYour choice: "
                )
                if ch.startswith("y"):
                    total += random.randint(5, 30)
                    break
        else:
            break
    except:
        print(
            "Your bet must be a positive int no larger than your total of: "
            + str(total)
        )
        bet = input("Enter your bet: ")
print("User's score: " + str(user))
#   While the game is not over, it will ask the user for an action (hit,stand or give up), performs the necessary procedures and then checks if the game should be finished, and who will be the winner
while user <= 21 and dealer <= 21:
    while True:
        action = input("Will you hit, stand or give up? (h,s or g)\nYour action: ")
        if action == "s" or action == "h" or action == "g":
            break
        print("Wrong, motherfucker")
    if action == "h":
        if user <= 21:
            c = random.choice(deck)
            if c == "A":
                if user > 10:
                    user += 1
                    print("The user got an A (1 points)")
                else:
                    user += 11
                    print("The user got an A (11 points)")
            else:
                user += values[c]
                if c != "8":
                    print("The user got a " + c + " (" + str(values[c]) + " points)")
                else:
                    print("The user got an 8 (8 points)")
        if dealer <= 17:
            c = random.choice(deck)
            if c == "A":
                if dealer > 10:
                    dealer += 1
                else:
                    dealer += 11
            else:
                dealer += values[c]
    elif action == "s":
        if dealer > 17 and (dealer - user) > 5:
            print("User's score: " + str(user))
            print("Dealer's score: " + str(dealer))
            print("You lost! You have lost your bet")
            total -= bet
            print("Your new total is: " + str(total))
            f = open(".total", "w")
            f.write(str(total))
            f.close()
            exit()
            break
        else:
            c = random.choice(deck)
            if c == "A":
                if dealer > 10:
                    dealer += 1
                else:
                    dealer += 11
            else:
                dealer += values[c]
    else:
        print("You surrended and received half your bet back.")
        total -= int(bet / 2)
        print("Your new total: " + str(total))
        f = open(".total", "w")
        f.write(str(total))
        f.close()
        exit()
        break
    if user > dealer and dealer >= 17:
        if user == 21:
            print("User's score: " + str(user))
            print("Dealer's score: " + str(dealer))
            print("Blackjack! You have won 2.5 times your bet")
            total += int(bet * 2.5)
            print("Your new total is: " + str(total))
            f = open(".total", "w")
            f.write(str(total))
            f.close()
            exit()
            break
        elif user < 21:
            print("User's score: " + str(user))
            print("Dealer's score: " + str(dealer))
            print("You won! You have won double your bet")
            total += bet * 2
            print("Your new total is: " + str(total))
            f = open(".total", "w")
            f.write(str(total))
            f.close()
            exit()
            break
    elif user == dealer and dealer >= 17:
        print("User's score: " + str(user))
        print("Dealer's score: " + str(dealer))
        print("It is a tie! You keep your bet.")
        print("Your new total is: " + str(total))
        f = open(".total", "w")
        f.write(str(total))
        f.close()
        exit()
        break
    elif user < dealer and dealer >= 17 and (dealer - user) > 5:
        print("User's score: " + str(user))
        print("Dealer's score: " + str(dealer))
        print("You lost! You have lost your bet")
        total -= bet
        print("Your new total is: " + str(total))
        f = open(".total", "w")
        f.write(str(total))
        f.close()
        exit()
        break
    print("User's score: " + str(user))
if user <= 21:
    print("User's score: " + str(user))
    print("Dealer's score: " + str(dealer))
    if user == 21:
        print("Blackjack! You have won 2.5 times your bet")
        total += int(bet * 2.5)
    else:
        print("You won! You have won double your bet")
        total += bet * 2
    print("Your new total is: " + str(total))
    f = open(".total", "w")
    f.write(str(total))
    f.close()
else:
    print("User's score: " + str(user))
    print("Dealer's score: " + str(dealer))
    print("You lost! You have lost your bet")
    total -= bet
    print("Your new total is: " + str(total))
    f = open(".total", "w")
    f.write(str(total))
    f.close()
