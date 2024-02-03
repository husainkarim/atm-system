# ATM MANAGEMENT SYSTEM

ATM Management is a compiled program state of the art technology that allows users to create account, withdraw/ deposit, remove existing accounts and even transfer account to another user.

## USAGE

It is imperative to run the below command to build the program

To run the program, run the following command:

```
$ make
$ cc -o atm src/main.o src/system.o src/auth.o -lsqlite3
$ ./atm

```

## FEATURES

- Registration: users able to register new unique username, and password.
- Update Information: Users can update certain information per account.
- Check account details: Users can check account details.
- Make Transactions: Users can make deposit and withdraw transactions per each account (except for fixed ones).
- Remove Existing Accounts: Users can remove existing accounts
- Transfer Ownership: users can transfer ownership of accounts to another user.

## AUTHOR

Husain AL Abbood

## LICENSE

This project is licensed by 01 Reboot