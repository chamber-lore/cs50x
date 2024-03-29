import csv
import sys


def main():
    if len(sys.argv) != 3:
        sys.exit("Usage: python dna.py [STR count csv] [DNA sequence txt]")

    # Read database file into a variable
    people = []
    strs = []
    with open(sys.argv[1], "r") as database:
        reader = csv.reader(database)
        for row in reader:
            if row[0] == "name":
                for i in range(1, len(row)):
                    strs.append(row[i])
            else:
                person = {
                    "name": row[0]
                }
                people.append(person)
                for i in range(1, len(row)):
                    person[strs[i - 1]] = int(row[i])

    # Read DNA sequence file into a variable
    with open(sys.argv[2], "r") as sequence:
        dna = sequence.readline().rstrip("\n")

    # Find longest match of each STR in DNA sequence and check database for matching profiles
    match_name = ""
    for person in people:
        match = True
        for str in strs:
            if person[str] != longest_match(dna, str):
                match = False
        if match:
            match_name = person["name"]

    if match_name == "":
        print("No match")
    else:
        print(f"{match_name}")

    return


def longest_match(sequence, subsequence):
    """Returns length of longest run of subsequence in sequence."""

    # Initialize variables
    longest_run = 0
    subsequence_length = len(subsequence)
    sequence_length = len(sequence)

    # Check each character in sequence for most consecutive runs of subsequence
    for i in range(sequence_length):

        # Initialize count of consecutive runs
        count = 0

        # Check for a subsequence match in a "substring" (a subset of characters) within sequence
        # If a match, move substring to next potential match in sequence
        # Continue moving substring and checking for matches until out of consecutive matches
        while True:

            # Adjust substring start and end
            start = i + count * subsequence_length
            end = start + subsequence_length

            # If there is a match in the substring
            if sequence[start:end] == subsequence:
                count += 1

            # If there is no match in the substring
            else:
                break

        # Update most consecutive matches found
        longest_run = max(longest_run, count)

    # After checking for runs at each character in seqeuence, return longest run found
    return longest_run


main()
