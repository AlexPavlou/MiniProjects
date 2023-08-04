import random

num = random.randint(1,100)
num2 = random.randint(0,1)
guess = input("Enter your guess: ")
while guess!="tails" and guess!="Tails" and guess!="Heads" and guess!="heads":
    print("Choose either heads or tails")
    guess = input("Enter your guess: ")
if(num2==0):
        if(num>=50):
                winning = "heads"
        else:
                winning = "tails"
else:
        if(num<=50):
                winning = "tails"
        else:
                winning = "heads"
if(guess==winning):
        print("And it is " + guess + ", you won!")
else:
        print("You have lost")
