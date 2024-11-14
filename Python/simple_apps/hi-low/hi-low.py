import random


def check_int(n):
    while type(n) != int:
        try:
            n = int(n)
            break
        except:
            n = input("Your Guess Must Be A Positive Integer.\nTry Again: ")
    return n


ans = random.randint(1, 100)
user = input("Enter Your Guess: ")
user = check_int(user)
tries = 1
while user != ans and tries <= 5:
    if user > ans:
        print("Too High!")
    else:
        print("Too Low!")
    user = input("Enter Your Guess: ")
    user = check_int(user)
    tries += 1
if user == ans:
    print("You Guessed Correctly In: ", tries, " Tries")
else:
    print("You failed to find the number 6 times, it was: " + str(ans))
