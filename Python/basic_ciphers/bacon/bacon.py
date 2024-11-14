# returns matching string of combination of 'a' and 'b' for each letter

s = input("Enter the string to be encrypted: ")
chars = {
    "a": "aaaaa ",
    "b": "aaaab ",
    "c": "aaaba ",
    "d": "aaabb ",
    "e": "aabaa ",
    "f": "aabab ",
    "g": "aabba ",
    "h": "aabbb ",
    "i": "abaaa ",
    "j": "abaab ",
    "k": "abaab ",
    "l": "ababa ",
    "m": "ababb ",
    "n": "abbaa ",
    "o": "abbab ",
    "p": "abbba ",
    "q": "abbbb ",
    "r": "baaaa ",
    "s": "baaab ",
    "t": "baaba ",
    "u": "babaa ",
    "v": "babab ",
    "w": "babaa ",
    "x": "babab ",
    "y": "babba ",
    "z": "babbb ",
}
transtable = s.maketrans(chars)
print("Your encrypted string is: " + s.lower().translate(transtable))
