import random
import time
# import sys
from os import system, name
from termcolor import colored


def print_arr(arr, x, y,flagged):
    if name == 'nt':
        _ = system('cls')
    else:
        _ = system('clear')
    dashes = x*4
    s = ''
    p = 1
    print(colored(' y', 'yellow'))
    print('      ' + '-'*dashes)
    for i in range(y):
        if p <= 9:
            s = ' ' + colored(str(p), 'red') + '    |'
        else:
            s = ' ' + colored(str(p), 'red') + '   |'
        for k in range(x):
            if arr[i][k]<10:
                if(arr[i][k]==9):
                    s += ' ' + colored('*', 'yellow') + ' |'
                elif(arr[i][k]==0):
                    s += '   |'
                elif(arr[i][k]==1):
                    s += ' ' + colored(str(arr[i][k]), 'blue') +' |'
                elif(arr[i][k]==2):
                    s += ' ' + colored(str(arr[i][k]),'green') + ' |'
                elif(arr[i][k]==3):
                    s += ' ' + colored(str(arr[i][k]), 'light_red') +' |'
                elif(arr[i][k]==4):
                    s += ' ' + colored(str(arr[i][k]), 'cyan') +' |'
                elif(arr[i][k]==5):
                    s += ' ' + colored(str(arr[i][k]), 'red') +' |'
                elif(arr[i][k]==6):
                    s += ' ' + colored(str(arr[i][k]), 'light_cyan') +' |'
                elif(arr[i][k]==7):
                    s += ' ' + colored(str(arr[i][k]), 'black', 'on_white') +' |'
                elif(arr[i][k]==8):
                    s += ' ' + colored(str(arr[i][k]), 'grey', 'on_white') +' |'
            else:
                if [i,k] in flagged:
                    s +=' ' + colored('X','yellow') +' |'
                else:
                    s +=' - |'
        print(s)
        print('      ' + '-'*dashes)
        p +=1
    s= colored(' x   ','yellow')
    for z in range(1, x+1):
        if(z <= 9):
            s += '   ' + colored(str(z), 'red')
        else:
            s += '  ' + colored(str(z), 'red')
    print(s)
    print('\n')
def isover(arr,x,y):
    i=0
    flag=True
    while i<y:
        for k in range(y):
            if (arr[i][k]>=10 and arr[i][k]!=19):
                flag=False
                break
        i+=1
    return(flag)

def decrease(arr,i,k):
    if arr[i][k]>9:
        arr[i][k]-=10

def build_map(arr,x,y,num_mines,i,k):
    randPoints = []
    excluded = [[i,k]]
    i = 0
    while i<num_mines:
        z = random.randint(0,x-1)
        k = random.randint(0,y-1)
        if [z,k] in excluded: continue
        randPoints.append([z,k])
        excluded.append([z,k])
        i += 1

    for i in range(len(randPoints)):
        z=randPoints[i][0]
        k=randPoints[i][1]
        arr[k][z]=9

    for i in range(y):
        for k in range(x):
            if arr[i][k]==9:
                pass
            elif i>0 and i<y-1 and k>0 and k<x-1:
                if arr[i][k-1]==9:
                    arr[i][k]+=1
                if arr[i][k+1]==9:
                    arr[i][k]+=1
                if arr[i-1][k]==9:
                    arr[i][k]+=1
                if arr[i+1][k]==9:
                    arr[i][k]+=1
                if arr[i-1][k-1]==9:
                    arr[i][k]+=1
                if arr[i+1][k+1]==9:
                    arr[i][k]+=1
                if arr[i+1][k-1]==9:
                    arr[i][k]+=1
                if arr[i-1][k+1]==9:
                    arr[i][k]+=1
            elif i==0 and k>0 and k<x-1:
                if arr[i+1][k]==9:
                    arr[i][k]+=1
                if arr[i][k+1]==9:
                    arr[i][k]+=1
                if arr[i][k-1]==9:
                    arr[i][k]+=1
                if arr[i+1][k+1]==9:
                    arr[i][k]+=1
                if arr[i+1][k-1]==9:
                    arr[i][k]+=1
            elif i==y-1 and k>0 and k<x-1:
                if arr[i][k+1]==9:
                    arr[i][k]+=1
                if arr[i][k-1]==9:
                    arr[i][k]+=1
                if arr[i-1][k+1]==9:
                    arr[i][k]+=1
                if arr[i-1][k-1]==9:
                    arr[i][k]+=1
                if arr[i-1][k]==9:
                    arr[i][k]+=1
            elif k==0 and i>0 and i<y-1:
                if arr[i][k+1]==9:
                    arr[i][k]+=1
                if arr[i-1][k]==9:
                    arr[i][k]+=1
                if arr[i+1][k]==9:
                    arr[i][k]+=1
                if arr[i-1][k+1]==9:
                    arr[i][k]+=1
                if arr[i+1][k+1]==9:
                    arr[i][k]+=1
            elif k==x-1 and i>0 and i<y-1:
                if arr[i-1][k]==9:
                    arr[i][k]+=1
                if arr[i-1][k-1]==9:
                    arr[i][k]+=1
                if arr[i+1][k]==9:
                    arr[i][k]+=1
                if arr[i][k-1]==9:
                    arr[i][k]+=1
                if arr[i+1][k-1]==9:
                    arr[i][k]+=1
            elif i==k and i==0:
                if arr[i+1][i+1]==9:
                    arr[i][i]+=1
                if arr[i+1][i]==9:
                    arr[i][i]+=1
                if arr[i][i+1]==9:
                    arr[i][i]+=1
            elif i==y-1 and k==x-1:
                if arr[i-1][k]==9:
                    arr[i][k]+=1
                if arr[i][k-1]==9:
                    arr[i][k]+=1
                if arr[i-1][k-1]==9:
                    arr[i][k]+=1
            elif i==y-1 and k==0:
                if arr[i-1][k]==9:
                    arr[i][k]+=1
                if arr[i-1][k+1]==9:
                    arr[i][k]+=1
                if arr[i][k+1]==9:
                    arr[i][k]+=1
            elif i==0 and k==x-1:
                if arr[i+1][k]==9:
                    arr[i][k]+=1
                if arr[i+1][k-1]==9:
                    arr[i][k]+=1
                if arr[i][k-1]==9:
                    arr[i][k]+=1
    for k in range(x):
        for i in range(y):
            if arr[i][k]==9:
                arr[i][k]+=10

def find_blanks(arr,i,k,x,y):
    if arr[i][k]>10 and arr[i][k]!=19:
        arr[i][k]-=10
    elif arr[i][k]==10:
        arr[i][k]-=10
        if i>0 and i<y-1 and k>0 and k<x-1:
            find_blanks(arr,i,k-1,x,y)
            find_blanks(arr,i,k+1,x,y)
            find_blanks(arr,i-1,k,x,y)
            find_blanks(arr,i+1,k,x,y)
            find_blanks(arr,i-1,k-1,x,y)
            find_blanks(arr,i+1,k+1,x,y)
            find_blanks(arr,i+1,k-1,x,y)
            find_blanks(arr,i+1,k+1,x,y)
        elif i==0 and k>0 and k<x-1:
            find_blanks(arr,i+1,k,x,y)
            find_blanks(arr,i,k+1,x,y)
            find_blanks(arr,i,k-1,x,y)
        elif i==y-1 and k>0 and k<x-1:
            find_blanks(arr,i,k+1,x,y)
            find_blanks(arr,i,k-1,x,y)
            find_blanks(arr,i-1,k,x,y)
        elif k==0 and i>0 and i<y-1:
            find_blanks(arr,i,k+1,x,y)
            find_blanks(arr,i-1,k,x,y)
            find_blanks(arr,i+1,k,x,y)
        elif k==x-1 and i>0 and i<y-1:
            find_blanks(arr,i-1,k,x,y)
            find_blanks(arr,i+1,k,x,y)
            find_blanks(arr,i,k-1,x,y)
        elif i==k and i==0:
            find_blanks(arr,i+1,k,x,y)
            find_blanks(arr,i,k+1,x,y)
        elif i==k and k==x-1:
            find_blanks(arr,i-1,k,x,y)
            find_blanks(arr,i,k-1,x,y)
        elif i==y-1 and k==0:
            find_blanks(arr,i-1,k,x,y)
            find_blanks(arr,i,k+1,x,y)
        elif i==0 and k==x-1:
            find_blanks(arr,i+1,k,x,y)
            find_blanks(arr,i,k-1,x,y)

opt='n'
print("Welcome to clisweeper")
print("Enter option:\n1) Easy Mode\n2) Medium Mode\n3) Hard Mode\n4) Custom\n5) Exit")
while type(opt)!=int:
    opt=input("option = ")
    try:
        opt=int(opt)
        if opt<=0 or opt>5:
            print("Wrong input. (0<option<=5)")
            opt="n"
    except:
        print("Wrong input. (0<option<=5)")

if opt==1:
    x=10
    y=8
    num_mines=10
elif opt==2:
    x=12
    y=12
    num_mines=40
elif opt==3:
    x=15
    y=15
    num_mines=80
elif opt==4:
    print("What size should your array be? (x*y)")
    x='j'
    while type(x)!=int:
        x=input("x = ")
        try:
            x=int(x)
            if x<=2:
                print("Wrong input. x must be an integer over 2")
                x='j'
        except:
            print("Wrong input. x must be a positive integer")
    y='j'
    while type(y)!=int:
        y=input("y = ")
        try:
            y=int(y)
            if y<=2:
                print("Wrong input. y must be an integer over 2")
                y='j'
        except:
            print("Wrong input. y must be a positive integer")
    num_mines='j'
    max_mines=int(x*y*0.6)
    while type(num_mines)!=int:
        num_mines=input("How many mines should there be? ")
        try:
            num_mines=int(num_mines)
            if num_mines<=0 or num_mines>max_mines:
                print("Wrong input. The amount of mines should be a positive integer (less than:" + str(max_mines) +")")
                num_mines='j'
        except:
            print("Wrong input. The amount of mines should be a positive integer (less than:" + str(max_mines) +")")
arr=[[0]*x for i in range(y)]
print(arr)
for i in range(y):
    for k in range(x):
            arr[i][k]+=10

def get_cord(i,arr,x,y):
    i='x'
    print("Coordinates: " + str(i) +':' + str(x))
    print("Enter y coordinate")
    while type(i)!=int:
        i=input("y = ")
        try:
            i=int(i)
            if i<=0 or i>y:
                print("Wrong input. (0<=y<=" + str(y) + ')')
                i='x'
        except:
            print("Wrong input. (0<=y<=" + str(y) + ')')
    return (i-1)

def get_cord2(k,arr,x,y):
    k='y'
    print("Enter x coordinate")
    while type(k)!=int:
        k=input("x = ")
        try:
            k=int(k)
            if k<=0 or k>x:
                print("Wrong input. (0<x<=" + str(x) + ')')
                k='x'
        except:
            print("Wrong input. (0<x<=" + str(x) + ')')
    return(k-1)

def get_input(opt):
    opt='n'
    print("Enter option:\n1) Dig Mine\n2) Flag Mine\n3) Unflag Mine\n4) Exit")
    while type(opt)!=int:
        opt=input("option = ")
        try:
            opt=int(opt)
            if opt<=0 or opt>4:
                print("Wrong input. (0<option<=4)")
                opt="n"
        except:
            print("Wrong input. (0<option<=4)")
    return(opt)

start_time=time.time()
first=True
flagged=[]
while True:
    print_arr(arr,x,y,flagged)
    opt='n'
    i='x'
    k='n'
    opt=get_input(opt)
    k=get_cord2(k,arr,x,y)
    i=get_cord(i,arr,x,y)
    if opt==4:
        break
    elif opt==2:
        if arr[i][k]>0 or ([i,k]) in flagged:
            print("This mine has already been dug or flagged. Try Again!")
            pass
        flagged.append([i,k])
    elif opt==3:
        if(not flagged or [i,k] not in flagged):
            print("This mine has not been flagged. Try Again!")
            pass
        if([i,k] in flagged):
            flagged.remove([i,k])
    else:
        if first:
            first=False
            build_map(arr,x,y,num_mines,i,k)
        if arr[i][k]<10:
            print("You have already dug this mine. Try Again!")
            pass
        elif arr[i][k]==19:
            for i in range(y):
                for k in range(x):
                    if arr[i][k]>9:
                        arr[i][k]-=10
            print_arr(arr,x,y,flagged)
            print('You touched a mine! Well that sucks.')
            print('You Game\'d Over in ' + str(round(time.time() - start_time,1)) + " seconds")
            break
        elif [i,k] not in flagged:
            find_blanks(arr,i,k,x,y)
    over=isover(arr,x,y)
    if over:
        for i in range(y):
            for k in range(x):
                if(arr[i][k]>=10):
                    arr[i][k]-=10
                if(arr[i][k]==19):
                    flagged.append([i,k])
        print_arr(arr,x,y,flagged)
        print("Congratulations. You succesfully sweeped all the mines in " + str(round(time.time() - start_time,1)) + " seconds")
        break
    print_arr(arr,x,y,flagged)
