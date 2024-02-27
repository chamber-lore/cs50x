import os
import datetime

from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")


@app.after_request
def after_request(response):
    """Ensure responses aren't cached"""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""
    portfolio = db.execute(
        "SELECT symbol, SUM(shares), price FROM transactions WHERE user_id = ? GROUP BY symbol", session["user_id"])
    balance = float(db.execute("SELECT cash FROM users WHERE id = ?", session["user_id"])[0]["cash"])
    grand_total = balance
    quotes = []

    for stock in portfolio:
        quote = lookup(stock["symbol"])
        stock["total_value"] = float(stock["SUM(shares)"]) * float(quote["price"])
        grand_total += stock["total_value"]
        quotes.append(quote)

    return render_template("index.html", portfolio=portfolio, quotes=quotes, balance=balance, grand_total=grand_total)


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""
    if request.method == "GET":
        return render_template("buy.html")

    symbol = request.form.get("symbol")
    quote = lookup(symbol)

    if not symbol or quote is None:
        return apology("symbol not recognized")

    shares = request.form.get("shares")

    # I missed the part about all transactions being positive INTEGERS, and didn't want to refactor
    # my database, so this effectively prevents proper float input (while preserving my math and
    # queries). see this logic also in sell() and cash()
    if not shares or not shares.isdigit() or float(shares) <= 0.0:
        return apology("not a positive number of shares")

    shares = float(shares)
    price = quote["price"]
    cash = float(db.execute("SELECT cash FROM users WHERE id = ?", session["user_id"])[0]["cash"])

    if cash < price * shares:
        return apology("not enough cash")

    cash -= price * shares
    db.execute("UPDATE users SET cash = ? WHERE id = ?", cash, session["user_id"])

    db.execute("INSERT INTO transactions(user_id, type, symbol, price, shares, datetime) VALUES(?, ?, ?, ?, ?, ?)",
               session["user_id"], "buy", symbol, price, shares, datetime.datetime.now())
    return redirect("/")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""
    transactions = db.execute(
        "SELECT type, symbol, price, shares, datetime FROM transactions WHERE user_id = ?", session["user_id"])

    for transaction in transactions:
        transaction["overall_value"] = float(transaction["price"]) * float(transaction["shares"])

    return render_template("history.html", transactions=transactions)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = ?", request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    if request.method == "GET":
        return render_template("quote.html")

    symbol = request.form.get("symbol")
    quote = lookup(symbol)

    if not symbol or quote is None:
        return apology("symbol not recognized")

    return render_template("quoted.html", quote=quote)


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""
    if request.method == "GET":
        return render_template("register.html")

    username = request.form.get("username")
    password = request.form.get("password")
    confirmation = request.form.get("confirmation")

    if not username or db.execute("SELECT username FROM users WHERE username = ?", username):
        return apology("username blank or taken")
    elif not password or password != confirmation:
        return apology("passwords blank or not matching")

    db.execute("INSERT INTO users(username, hash) VALUES(?, ?)", username, generate_password_hash(password))
    flash("Successfully registered for account " + username)

    session["user_id"] = db.execute("SELECT id FROM users WHERE username = ?", username)[0]["id"]
    return redirect("/")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""
    symbols = db.execute("SELECT DISTINCT(symbol) FROM transactions WHERE user_id = ?", session["user_id"])

    if request.method == "GET":
        return render_template("sell.html", symbols=symbols)

    symbol = request.form.get("symbol")
    shares = request.form.get("shares")

    if not symbol:
        return apology("stock not owned")

    symbol_allowed = False
    for s in symbols:
        if symbol == s["symbol"]:
            symbol_allowed = True

    if not symbol_allowed:
        return apology("stock not owned")

    owned = float(db.execute("SELECT SUM(shares) FROM transactions WHERE user_id = ? AND symbol = ?",
                             session["user_id"], symbol)[0]["SUM(shares)"])

    if not shares or not shares.isdigit() or float(shares) > owned:
        return apology("not enough stock owned")

    shares = float(shares)
    price = lookup(symbol)["price"]
    cash = float(db.execute("SELECT cash FROM users WHERE id = ?", session["user_id"])[0]["cash"])

    db.execute("INSERT INTO transactions(user_id, type, symbol, price, shares, datetime) VALUES(?, ?, ?, ?, ?, ?)",
               session["user_id"], "sell", symbol, price, -1.00 * shares, datetime.datetime.now())
    db.execute("UPDATE users SET cash = ? WHERE id = ?", cash + shares * price, session["user_id"])
    return redirect("/")


@app.route("/cash", methods=["GET", "POST"])
@login_required
def cash():
    """Add cash to your account"""
    if request.method == "GET":
        return render_template("cash.html")

    amount = request.form.get("amount")

    if not amount or not amount.isdigit() or float(amount) <= 0.0:
        return apology("not a positive number of dollars")

    amount = float(amount)
    print(amount)
    cash = float(db.execute("SELECT cash FROM users WHERE id = ?", session["user_id"])[0]["cash"])

    db.execute("UPDATE users SET cash = ? WHERE id = ?", cash + amount, session["user_id"])
    flash("$" + str(amount) + " successfully added to your account")
    return redirect("/")