import random#,pyttsx3

# This program picks randomly one item from each array and constructs a very random sentence from them, trying to insult the user. It could be modified to convert the result string into speech but I was not able to make this happen

one = ['a retarded','a very big','a dumb','a stupid', 'such a','one']
two = ['balding','table-headed','mid-ass','stupid-looking','ping pong table forehead','tiny hands','small legs','big-headed','wardrobe-shaped','cookie-cutter','retarded little','dopamine-deficient','super mega','ultra','butter-hands','eggy breath','dirty asshole','dumbass-looking','hairless legs','shit-smelling','gloopy','fragile','vitamn B-12 defficient']
three = ['gayman','stupid idiot','pig','poopyhead','mf','snowflake','airhead','dork','freak','idiot','psycho','kind of guy']
print("You are " + random.choice(one) + ' ' + random.choice(two) + ' ' + random.choice(three))
#engine = pyttsx3.init()
#engine.say("You are " + random.choice(one) + ' ' + random.choice(two) + random.choice(three))
#engine.runAndWait()
