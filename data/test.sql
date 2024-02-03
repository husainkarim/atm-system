CREATE TABLE users (
    user_id INTEGER NOT NULL,
    user_name CHAR(50) NOT NULL UNIQUE,
    user_pass PASSWORD NOT NULL,
    PRIMARY KEY("user_id" AUTOINCREMENT)
);

CREATE TABLE records (
    record_id INTEGER NOT NULL,
    user_id INTEGER NOT NULL,
    user_name CHAR(50) NOT NULL,
    account_id INTEGER NOT NULL UNIQUE,
    month INTEGER NOT NULL,
    day INTEGER NOT NULL,
    year INTEGER NOT NULL,
    country CHAR(50) NOT NULL,
    phone_no INTEGER NOT NULL,
    balance REAL NOT NULL,
    account_type CHAR(10) NOT NULL,
    PRIMARY KEY("record_id" AUTOINCREMENT)
    FOREIGN KEY("user_id") REFERENCES users("user_id")
);

SELECT user_pass FROM users WHERE user_name = ?
INSERT INTO users (user_name, user_pass) VALUES ("Alice", "q1w2e3r4t5y6")
SELECT record_id, user_id, user_name, account_id, creation_date, country, phone_no, balance, account_type FROM records WHERE user_name = ?

INSERT INTO records (user_id, user_name, account_id, creation_date, country, phone_no, balance, account_type) VALUES (?, ?, ?, ?, ?, ?, ?, ?)

UPDATE records SET country = ? WHERE account_id = ?