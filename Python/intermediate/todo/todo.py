import os

# This program, in a similar fashion to ../pass, lets the user save tasks, un/mark them as done, delete them, edit them, view all of them, clear the screen or quit the program

if not os.path.exists(".todo"):
    f = open(".todo", "w")
    f.close()


def show():
    f = open(".todo", "r")
    lines = f.read().splitlines()
    f.close()
    num = 1
    string = ""
    for i in lines:
        print("#" + str(num) + ": " + i)
        num += 1


while True:
    while True:
        action = input(
            "Do you want to:\n1) View your tasks\n2) Add a task\n3) Edit task\n4) Delete a task\n5) Un/Mark task as done\n6) Clear Screen\n7) Quit program (7 or q)\nYour Choice: "
        )
        try:
            if action == "q":
                break
            action = int(action)
            if action < 1 or action > 7:
                raise Exception
            break
        except:
            print("Your choice must be a positive integer from 1 to 7, or q.")
    if action == 1:
        show()
    elif action == 2:
        new_task = input("Task: ")
        f = open(".todo", "a")
        f.write(new_task + "\n")
        f.close()
    elif action == 3:
        f = open(".todo", "r")
        l = f.read().splitlines()
        length = 0
        for i in l:
            length += 1
        num = 1
        show()
        x = input(
            "What line(s) do you want to edit (e.g. Line: 3,1 or Line: 4)?\nLine: "
        )
        x = x.split(",")
        f.close()
        f = open(".todo", "w")
        for i in l:
            if str(num) in x:
                print(i)
                new_value = input("New value: ")
                f.write(new_value + "\n")
            else:
                f.write(i + "\n")
            num += 1
        f.close()
        print("Todo file after editing lines " + str(x) + ": ")
        show()
    elif action == 4:
        f = open(".todo", "r")
        l = f.read().splitlines()
        length = 0
        for i in l:
            length += 1
        f.close()
        num = 1
        show()
        x = input("What line(s) should be deleted (e.g. Line: 1,2 or Line: 3)?\nLine: ")
        x = x.split(",")
        with open(".todo", "w") as f:
            while num <= length:
                if str(num) not in x:
                    f.write(l[num - 1] + "\n")
                num += 1
        print("Todo file after deleting line(s) " + str(x).strip("'[]'") + ": ")
        show()
    elif action == 5:
        f = open(".todo", "r")
        l = f.read().splitlines()
        length = 0
        for i in l:
            length += 1
        num = 1
        show()
        x = input(
            "What line(s) do you want to mark as done (e.g. Line: 3,1 or Line: 4)?\nLine: "
        )
        x = x.split(",")
        f.close()
        f = open(".todo", "w")
        for i in l:
            if str(num) in x:
                if i[-1] == "✓":
                    i = i.replace("✓", "")
                    f.write(i + "\n")
                elif i[-1] != "✓":
                    f.write(i + "✓" + "\n")
            else:
                f.write(i + "\n")
            num += 1
        f.close()
        print("Todo file after marking lines " + str(x) + " as done: ")
        show()
    elif action == 6:
        os.system("clear")
    else:
        print("Program will be terminated.")
        break
print("...")
