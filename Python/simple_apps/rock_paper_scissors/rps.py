import random

# Asks the user to choose either rock, paper or scissors and compares his choice with a randomly picked choice of the robot. It then decides who won,lost, or ended up in a tie

actions = ["quit", "rock", "scissors", "paper"]
print("Possible actions: " + str(actions))
user = " "
T = 0
W = 0
L = 0
while True:
    user = input("Enter Your Choice: ")
    if user not in actions:
        print("Your choice must be either rock, paper or scissors.")
        continue
    break
while user != "quit":
    en = random.choice(actions[1:])
    print(en)
    if en == user:
        print("It's A Tie!")
        T = T + 1
    elif (
        (en == "rock" and user == "paper")
        or (en == "paper" and user == "scissors")
        or (en == "scissors" and user == "rock")
    ):
        print("The enemy chose " + en + " you have won!")
        W = W + 1
    elif (
        (en == "paper" and user == "rock")
        or (en == "scissors" and user == "paper")
        or (en == "rock" and user == "scissors")
    ):
        print("The enemy chose " + en + " you have lost.")
        L = L + 1
    while True:
        user = input("Enter Your Choice: ")
        if user not in actions:
            print("Your choice must be either rock, paper or scissors.")
            continue
        break
print("stats:\nWins: " + str(W) + "\nLoses: " + str(L) + "\nTies: " + str(T))
print(
    "Total Score (Wins*2+Ties) = " + str(W) + " * 2 + " + str(T) + "= " + str(W * 2 + T)
)
