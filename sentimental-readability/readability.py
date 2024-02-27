from cs50 import get_string


text = get_string("Text: ")
L = 0
S = 0
W = 1
stops = ['.', '!', '?']
for c in text:
    if c.isalpha():
        L += 1
    elif c == ' ':
        W += 1
    elif c in stops:
        S += 1
index = round(5.88 * (L / W) - 29.6 * (S / W) - 15.8)
if (index < 1):
    print("Before Grade 1")
elif (index >= 16):
    print("Grade 16+")
else:
    print(f"Grade {index}")