import random, getpass, os, base64, string, time, re


# encrypts string with b64
def encrypt(s):
    return base64.b64encode(s.encode()).decode()


# decrypts said string using base64
def decrypt(s):
    return base64.b64decode(s).decode()


# shows all passwords saved, decoding each line and printing it after #+'num'
def show():
    f = open(".pass", "r")
    lines = f.read().splitlines()
    f.close()
    num = 1
    string = ""
    for i in lines[1:]:
        for m in i.split():
            string += decrypt(m) + " "
        print("#" + str(num) + ": " + string)
        string = ""
        num += 1


if os.path.exists(".pass"):
    f = open(".pass", "r")
    master = decrypt(f.readline())
    f.close()
else:
    f = open(".pass", "w")
    while True:
        master = getpass.getpass(prompt="Set your master password: ")
        # The master password must have at least one{uppercase,lowercase,symbol,digit} and be over 6 letters long
        if re.match(
            "^(?=.*[a-z])(?=.*[A-Z])(?=.*\d)(?=.*[@$!%*?&])[A-Za-z\d@$!%*?&]{6,}$",
            master,
        ):
            f.write(encrypt(master))
            break
        print("Your new master password must: ")
        if len(master) < 6:
            print("\tbe at least 6 characters long")
        if not re.match(".*[a-z].*", master):
            print("\tcontain at least 1 uppercase Character")
        if not re.match(".*[A-Z].*", master):
            print("\tcontain at least 1 lowercase character")
        if not re.match("^[a-zA-Z0-9 ]*$", master):
            print("\tcontain at least 1 symbol")
        if not re.match(".*[0-9].*", master):
            print("\tContain at least 1 digit")
        continue
    f.close()

if os.path.exists(".timer"):
    f = open(".timer", "r")
    a = f.readline()
    words = a.split()
    over = words[0]
    left = int(words[1])
    f.close()
else:
    f = open(".timer", "w")
    f.write("True 0")
    over = True
    left = 0
    f.close()

tries = 3
while True:
    # if the user terminated the program early without having waited for his cooldown to complete, the program will continue to count down
    if over == "True":
        while True:
            if tries != 0:
                pw = getpass.getpass(prompt="Input the master password: ")
                if pw == master:
                    break
                else:
                    tries -= 1
                    if tries > 0:
                        print("Try again, you have " + str(tries) + " tries left")
                    else:
                        print("Try again, you have no tries left")
            # if the user is out of tries, he must wait 15 seconds to try again
            else:
                sec = 15
                f = open(".timer", "w")
                while sec > 0:
                    print("You can try again in : " + str(sec) + " seconds")
                    f = open(".timer", "w")
                    f.write("False " + str(sec))
                    f.close()
                    time.sleep(1)
                    sec -= 1
                    tries = 3
                f = open(".timer", "w")
                f.write("True 0")
                f.close()
        break
    else:
        while left > 0:
            print("You can try again in : " + str(left) + " seconds")
            f = open(".timer", "w")
            # writes 'False', meaning that the countdown is not over, and the 'left' int shows the time left to complete the cooldown
            f.write("False " + str(left))
            f.close()
            time.sleep(1)
            left -= 1
        f = open(".timer", "w")
        f.write("True 0")
        f.close()
        over = "True"
        continue

while True:
    # receives user's desired action and checks for its integrity
    while True:
        action = input(
            "Do you want to:\n1) View your passwords\n2) Add a password\n3) Edit password information\n4) Delete a password\n5) Change master password\n6) Clear Screen\n7) Quit program (7 or q)\nYour Choice: "
        )
        try:
            if action == "q":
                break
            action = int(action)
            if action < 1 or action > 6:
                raise Exception
            break
        except:
            print("Your choice must be a positive integer from 1 to 7, or q.")
    # calls show() function which shows all passwords but decryptd
    if action == 1:
        show()
    # either creates a custom-length password with the user's choice of character combinations, or receives a custom password
    elif action == 2:
        while True:
            ch = input(
                "Do you want to:\n1) Manually add a password\n2) Automatically create a password\nYour choice: "
            )
            try:
                ch = int(ch)
                if ch < 1 or ch > 2:
                    raise Exception
                break
            except:
                print("Your choice must be a positive integer from 1 to 2")
        if ch == 1:
            p = input("Enter your password: ")
        else:
            while True:
                length = input("Password length: ")
                try:
                    length = int(length)
                    if length <= 0:
                        raise Exception
                        continue
                    break
                except:
                    print("The desired password length must be a positive integer")
            print(
                "You can either choose 'a' (or press enter), or otherwise type out the representative numbers"
            )
            while True:
                choices = input(
                    "Should your password have:\n1) Lowercase characters\n2) Uppercase characters\n3) Symbols/Punctuation\n4) Digits\na) All of the above\nChoice: "
                )
                if "a" in choices and choices != "a":
                    print(
                        "You can either choose a), or otherwise type out the representative numbers"
                    )
                    continue
                break
            bank = []
            if choices == "":
                bank += (
                    string.ascii_lowercase
                    + string.ascii_uppercase
                    + string.digits
                    + string.punctuation
                )
            elif choices == "a":
                bank += (
                    string.ascii_lowercase
                    + string.ascii_uppercase
                    + string.digits
                    + string.punctuation
                )
            elif "1" in choices:
                bank += string.ascii_lowercase
            elif "2" in choices:
                bank += string.ascii_uppercase
            elif "3" in choices:
                bank += string.punctuation
            elif "4" in choices:
                bank += string.digits
            p = ""
            for i in range(0, length):
                p += random.choice(bank)
            print("Your password is: " + p)
        # lets the user choose whether to include a site and e-mail/username
        site = input("Should a site be included (y or n)? ")
        mail = input("Should a username/e-mail be included (y or n)? ")
        if site.startswith("y"):
            site = True
        else:
            site = False
        if mail.startswith("y"):
            mail = True
        else:
            mail = False
        if mail and site:
            mail = input("Enter your username/e-mail: ")
            site = input("Enter the website: ")
            f = open(".pass", "a")
            f.write(
                "\n"
                + encrypt(
                    "Site: " + site + " - User/Mail: " + mail + " - Password: " + p
                )
            )
            f.close()
        elif mail:
            mail = input("Enter your username/e-mail: ")
            f = open(".pass", "a")
            f.write("\n" + encrypt("User/Mail :" + mail + " - Password: " + p))
            f.close()
        elif site:
            site = input("Enter the website: ")
            f = open(".pass", "a")
            f.write("\n" + encrypt("Site: " + site + " - Password:" + p))
            f.close()
        else:
            f = open(".pass", "a")
            f.write("\n" + encrypt("Password: " + p))
            f.close()
    # lets the user edit the line(s) the user wishes
    elif action == 3:
        f = open(".pass", "r")
        l = f.read().splitlines()
        length = 0
        for i in l:
            length += 1
        num = 1
        show()
        x = input(
            "What line(s) do you want to edit (e.g. Line: 3,1 or Line: 4)?\nLine: "
        )
        # splits 'x' on ',', so what is left is just the number of the lines that the user wants to edit
        x = x.split(",")
        f.close()
        f = open(".pass", "w")
        f.write(l[0])
        for i in l[1:]:
            # if the current line number is in the x array of the numbers the user wants to edit, it prints the decrypted value of that line and then lets him input a new value
            if str(num) in x:
                print(decrypt(i))
                new_value = input("New value: ")
                f.write("\n" + encrypt(new_value))
            else:
                f.write("\n" + i)
            num += 1
        f.close()
        print("Passwords file after editing lines " + str(x) + ": ")
        show()
    # as with action no. 3, but this time the program simply does not write the lines whose number is not in those that the user wants to delete
    elif action == 4:
        f = open(".pass", "r")
        l = f.read().splitlines()
        length = 0
        for i in l:
            length += 1
        f.close()
        num = 1
        show()
        x = input("What line(s) should be deleted (e.g. Line: 1,2 or Line: 3)?\nLine: ")
        x = x.split(",")
        with open(".pass", "w") as f:
            f.write(l[0])
            while num < length:
                if str(num) not in x:
                    f.write("\n" + l[num])
                num += 1
        print("Password file after deleting line " + str(x).strip("'[]'") + ": ")
        show()
    # lets the user change the masterpassword if it matches certain criteria, and prints relevant messages, letting him know what is wrong with the one he entered
    elif action == 5:
        while True:
            pw = getpass.getpass(prompt="Set your master password: ")
            if pw != master and re.match(
                "^(?=.*[a-z])(?=.*[A-Z])(?=.*\d)(?=.*[@$!%*?&])[A-Za-z\d@$!%*?&]{6,}$",
                pw,
            ):
                f = open(".pass", "r")
                lines = f.read().splitlines()
                f.close()
                f = open(".pass", "w")
                f.write(encrypt(pw))
                for i in lines[1:]:
                    f.write("\n" + i)
                f.close()
                break
            print("Your new master password must: ")
            if len(pw) < 6:
                print("\tbe at least 6 characters long")
            if pw == master:
                print("\tbe the same as the last master password")
            if not re.match(".*[a-z].*", pw):
                print("\tcontain at least 1 uppercase character")
            if not re.match(".*[A-Z].*", pw):
                print("\tcontain at least 1 lowercase character")
            if not re.match("^[a-zA-Z0-9 ]*$", pw):
                print("\tcontain at least 1 symbol")
            if not re.match(".*[0-9].*", pw):
                print("\tcontain at least 1 digit")
            continue
    elif action == 6:
        # clears screen
        os.system("clear")
    else:
        # stops while loop, thus terminating the program
        print("Program will be terminated.")
        break
