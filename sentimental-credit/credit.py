from cs50 import get_string


def main():
    while (True):
        card_num = get_string("Number: ")
        if card_num.isdigit():
            break
    print(luhn(card_num))


def luhn(num):
    lengths = [13, 15, 16]
    length = len(num)
    if length not in lengths:
        return "INVALID"
    sum = 0
    i = length - 1
    while i >= 0:
        if (length - i) % 2 == 0:
            dub = 2 * int(num[i])
            if dub > 9:
                dub = int(((dub - (dub % 10)) / 10) + dub % 10)
            sum += dub
        else:
            sum += int(num[i])
        i -= 1
    if sum % 10 == 0:
        if length == 13:
            if num[0] == "4":
                return "VISA"
        elif length == 15:
            second = ["4", "7"]
            if num[0] == "3" and num[1] in second:
                return "AMEX"
        elif length == 16:
            second = ["1", "2", "3", "4", "5"]
            if num[0] == "4":
                return "VISA"
            elif num[0] == "5" and num[1] in second:
                return "MASTERCARD"
        return "INVALID"
    return "INVALID"


if __name__ == "__main__":
    main()