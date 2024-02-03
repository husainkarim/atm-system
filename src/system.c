#include "header.h"
#include "sqlite3.h"
#include "string.h"
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

const char *FILENAME2 = "./data/atm.db";

void stayOrReturn(int notGood, void f(struct User u), struct User u)
{
    int option;
    if (notGood == 0) {
        system("clear");
        printf("\n✖ Record not found!!\n");
    invalid:
        printf("\nEnter 0 to try again, 1 to return to main menu and 2 to exit:");
        scanf("%d", &option);
        if (option == 0)
            f(u);
        else if (option == 1)
            mainMenu(u);
        else if (option == 2)
            exit(0);
        else
        {
            printf("Insert a valid operation!\n");
            goto invalid;
        }
    } else {
        printf("\nEnter 1 to go to the main menu and 0 to exit:");
        scanf("%d", &option);
    }
    if (option == 1) {
        system("clear");
        mainMenu(u);
    } else {
        system("clear");
        exit(1);
    }
}

void success(struct User u)
{
    int option;
    printf("\n✔ Success!\n\n");
invalid:
    printf("Enter 1 to go to the main menu and 0 to exit!\n");
    scanf("%d", &option);
    system("clear");
    if (option == 1) {
        mainMenu(u);
    } else if (option == 0) {
        exit(1);
    } else {
        printf("Insert a valid operation!\n");
        goto invalid;
    }
}

void fail(struct User u) {
    int option;
    printf("\nX Fail!\n\n");
invalid:
    printf("Enter 1 to go to the main menu and 0 to exit!\n");
    scanf("%d", &option);
    system("clear");
    if (option == 1) {
        mainMenu(u);
    } else if (option == 0) {
        exit(1);
    } else {
        printf("Insert a valid operation!\n");
        goto invalid;
    }
}

void saveAccountToDB(struct User u, struct Record r) {
    sqlite3 *db;
    int rc;
    rc = sqlite3_open(FILENAME2, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    char *sql = "INSERT INTO records (user_id, user_name, account_id, month, day, year, country, phone_no, balance, account_type) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    // sqlite3_busy_timeout(db, 5000);
    sqlite3_bind_int(stmt, 1, u.id);
    sqlite3_bind_text(stmt, 2, u.name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, r.accountNbr);
    sqlite3_bind_int(stmt, 4, r.deposit.month);
    sqlite3_bind_int(stmt, 5, r.deposit.day);
    sqlite3_bind_int(stmt, 6, r.deposit.year);
    sqlite3_bind_text(stmt, 7, r.country, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 8, r.phone);
    sqlite3_bind_double(stmt, 9, r.amount);
    sqlite3_bind_text(stmt, 10, r.accountType, -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error3: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_close(db);
}

int validateAccountType(char *accountType) {
    if (strcmp(accountType, "saving") == 0 || strcmp(accountType, "current") == 0 ||
        strcmp(accountType, "fixed01") == 0 || strcmp(accountType, "fixed02") == 0 ||
        strcmp(accountType, "fixed03") == 0) {
        return 1;
    }
    return 0;
}

int validate_date(int month, int day, int year) {
    if (month < 1 || month > 12) {
        return 0;  // 0 indicates invalid input
    }
    int max_days = 31;  // Assume 31 days by default
    if (month == 4 || month == 6 || month == 9 || month == 11) {
        max_days = 30;  // Adjust for months with 30 days
    } else if (month == 2) {
        int is_leap_year = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
        max_days = is_leap_year ? 29 : 28;
    }
    if (day < 1 || day > max_days) {
        return 0;
    }
    if (year < 2000 || year > 2023) {
        return 0;
    }
    return 1;
}

int checkAccountNo(int accountNo) {
    sqlite3 *db;
    int rc;
    sqlite3_stmt *stmt;
    rc = sqlite3_open(FILENAME2, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    char *sql = "SELECT COUNT(*) FROM records WHERE account_id = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_bind_int(stmt, 1, accountNo);
    
    sqlite3_step(stmt);
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return count;
}

int checkAccountNoForUser(int accountNo, char name[50]) {
    sqlite3 *db;
    int rc;
    sqlite3_stmt *stmt;
    rc = sqlite3_open(FILENAME2, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    char *sql = "SELECT COUNT(*) FROM records WHERE user_name = ? AND account_id = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, accountNo);
    int c = 0;
    int count;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    } else {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return count;
}

int is_valid_country_name(char *name) {
    // Check for empty string or leading/trailing spaces
    if (*name == '\0' || isspace(*name) || isspace(*(name + strlen(name) - 1))) {
        return 0;
    }

    // Check for only letters, spaces, hyphens, and apostrophes
    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha(name[i]) && !isspace(name[i]) && name[i] != '-' && name[i] != '\'') {
            return 0;
        }
    }

    return 1;
}

void createNewAcc(struct User u) {
    struct Record r;

noAccount:
    system("clear");
    printf("\t\t\t===== New record =====\n");
    printf("\nEnter today's date(mm/dd/yyyy):");
    printf("\nMonth:");
    if (scanf("%d", &r.deposit.month) != 1) {
        printf("Invalid date!\n");
        printf("You will be redirect to register new account.\n\n");
        fail(u);
    }
    printf("\nDay:");
    if (scanf("%d", &r.deposit.day) != 1) {
        printf("Invalid date!\n");
        printf("You will be redirect to register new account.\n\n");
        fail(u);
    }
    printf("\nYear:");
    if (scanf("%d", &r.deposit.year) != 1) {
        printf("Invalid date!\n");
        printf("You will be redirect to register new account.\n\n");
        fail(u);
    }
    if (validate_date(r.deposit.month, r.deposit.day, r.deposit.year) != 1) {
        printf("Invalid date!\n");
        printf("You will be redirect to register new account.\n\n");
        fail(u);
    }
    
    printf("\nEnter the account number:");
    scanf("%d", &r.accountNbr);
    if (r.accountNbr > 999999999999 || r.accountNbr < 0) {
        printf("✖ Your account number invalid!!\n\n");
        fail(u);
    }
    int count = checkAccountNo(r.accountNbr);
    if (count != 0) {
        printf("✖ This Account already exists for this user\n\n");
        fail(u);
    }
    printf("\nEnter the country:");
    scanf("%s", r.country);
    if (is_valid_country_name(r.country) != 1) {
        printf("✖ Your country name invalid!!\n\n");
        fail(u);
    }
    printf("\nEnter the phone number:");
    scanf("%d", &r.phone);
    if (r.phone > 999999999999 || r.phone < 0) {
        printf("✖ Your phone number invalid!!\n\n");
        fail(u);
    }
    printf("\nEnter amount to deposit: $");
    scanf("%lf", &r.amount);
    if (r.amount < 0) {
        printf("✖ The amount can not be negative!!\n\n");
        fail(u);
    }
    printf("\nChoose the type of account:\n\t-> saving\n\t-> current\n\t-> fixed01(for 1 year)\n\t-> fixed02(for 2 years)\n\t-> fixed03(for 3 years)\n\n\tEnter your choice:");
    scanf("%s", r.accountType);
    if (validateAccountType(r.accountType)==0) {
        fail(u);
    } else {
        saveAccountToDB(u, r);
        success(u);
    }
}

void getAccountsDetailsFromDB(char name[50]) {
    sqlite3 *db;
    int rc;
    struct Record r;
    rc = sqlite3_open(FILENAME2, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    
    sqlite3_stmt *stmt;
    char *sql = "SELECT * FROM records WHERE user_name = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_TRANSIENT);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        r.id = sqlite3_column_int(stmt, 0);
        r.userId = sqlite3_column_int(stmt, 1);
        strcpy(r.name, sqlite3_column_text(stmt, 2));
        r.accountNbr = sqlite3_column_int(stmt, 3);
        r.deposit.month = sqlite3_column_int(stmt, 4);
        r.deposit.day = sqlite3_column_int(stmt, 5);
        r.deposit.year = sqlite3_column_int(stmt, 6);
        strcpy(r.country, sqlite3_column_text(stmt, 7));
        r.phone = sqlite3_column_int(stmt, 8);
        r.amount = sqlite3_column_double(stmt, 9);
        strcpy(r.accountType, sqlite3_column_text(stmt, 10));
        printf("_____________________\n");
        printf("\nAccount number:%d\nDeposit Date:%d/%d/%d \ncountry:%s \nPhone number:%d \nAmount deposited: $%.2f \nType Of Account:%s\n",
            r.accountNbr,
            r.deposit.day,
            r.deposit.month,
            r.deposit.year,
            r.country,
            r.phone,
            r.amount,
            r.accountType);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void checkAllAccounts(struct User u)
{
    system("clear");
    printf("\t\t====== All accounts from user, %s =====\n\n", u.name);
    getAccountsDetailsFromDB(u.name);
    success(u);
}

void updateInfo(struct Record r, int option) {
    char change[10];
    if (option == 1) {
        strcpy(change, "phone_no");
    } else if (option == 2) {
        strcpy(change, "country");
    }
    sqlite3 *db;
    int rc;
    rc = sqlite3_open(FILENAME2, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    
    sqlite3_stmt *stmt;
    char buffer[70];
    sprintf(buffer, "UPDATE records SET %s = ? WHERE account_id = ?", change);
    char *sql = buffer;
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    if (option == 1) {
        sqlite3_bind_int64(stmt, 1, r.phone);
    } else if (option == 2) {
        sqlite3_bind_text(stmt, 1, r.country, -1, SQLITE_TRANSIENT);
    }
    sqlite3_bind_int64(stmt, 2, r.accountNbr);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void updateAccountInfo(struct User u) {
    struct Record r;
    int option;
    printf("\nWhat is the account number you want to change:");
    scanf("%d", &r.accountNbr);
    int count = checkAccountNoForUser(r.accountNbr, u.name);
    if (count != 1) {
        printf("✖ This Account not exists for this user\n\n");
        fail(u);
    }
    printf("\nWhich information do you want to update?\n1-> phone number\n2-> country\n\n");
    printf("Enter your choice:");
    scanf("%d", &option);
    switch (option) 
    {
    case 1:
        system("clear");
        printf("Enter the new phone number:");
        scanf("%d", &r.phone);
        break;
    case 2:
        system("clear");
        printf("Enter the new country:");
        scanf("%s", r.country);
        if (is_valid_country_name(r.country) != 1) {
            printf("✖ Your country name invalid!!\n\n");
            fail(u);
        }
        break;
    default:
        printf("Insert a valid operation!\n");
        exit(1);
    }
    
    updateInfo(r, option);
    success(u);
}

void checkAccountDetails(struct User u) {
    struct Record r;
    printf("\nWhat is the account number you want to check:");
    scanf("%d", &r.accountNbr);
    int count = checkAccountNoForUser(r.accountNbr, u.name);
    if (count != 1) {
        printf("✖ This Account not exists for this user\n\n");
        fail(u);
    }
    sqlite3 *db;
    int rc;
    rc = sqlite3_open(FILENAME2, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    
    sqlite3_stmt *stmt;
    char *sql = "SELECT * FROM records WHERE user_name = ? AND account_id = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_bind_text(stmt, 1, u.name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, r.accountNbr);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        r.id = sqlite3_column_int(stmt, 0);
        r.userId = sqlite3_column_int(stmt, 1);
        strcpy(r.name, sqlite3_column_text(stmt, 2));
        r.accountNbr = sqlite3_column_int(stmt, 3);
        r.deposit.month = sqlite3_column_int(stmt, 4);
        r.deposit.day = sqlite3_column_int(stmt, 5);
        r.deposit.year = sqlite3_column_int(stmt, 6);
        strcpy(r.country, sqlite3_column_text(stmt, 7));
        r.phone = sqlite3_column_int(stmt, 8);
        r.amount = sqlite3_column_double(stmt, 9);
        strcpy(r.accountType, sqlite3_column_text(stmt, 10));
        system("clear");
        printf("_____________________\n");
        printf("\nAccount number:%d\nDeposit Date:%d/%d/%d \ncountry:%s \nPhone number:%d \nAmount deposited: $%.2f \nType Of Account:%s\n\n",
            r.accountNbr,
            r.deposit.day,
            r.deposit.month,
            r.deposit.year,
            r.country,
            r.phone,
            r.amount,
            r.accountType);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    double interest;
    if (strcmp(r.accountType, "current") == 0) {
        printf("You will not get interests because the account is of type current\n");
    } else if (strcmp(r.accountType, "saving") == 0) {
        interest = r.amount * 0.07 / 12;
        printf("You will get $%.2f as interest on day %d of every month\n", interest, r.deposit.day);
    } else if (strcmp(r.accountType, "fixed01") == 0) {
        interest = r.amount * 0.04 * 1;
        printf("You will get $%.2f as interest on %d/%d/%d\n", interest, r.deposit.month, r.deposit.day, r.deposit.year+1);
    } else if (strcmp(r.accountType, "fixed02") == 0) {
        interest = r.amount * 0.05 * 2;
        printf("You will get $%.2f as interest on %d/%d/%d\n", interest, r.deposit.month, r.deposit.day, r.deposit.year+2);
    } else if (strcmp(r.accountType, "fixed03") == 0) {
        interest = r.amount * 0.08 * 3;
        printf("You will get $%.2f as interest on %d/%d/%d\n", interest, r.deposit.month, r.deposit.day, r.deposit.year+3);
    }
    success(u);
}

void updateAmount(struct Record r) {
    sqlite3 *db;
    int rc;
    rc = sqlite3_open(FILENAME2, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    
    sqlite3_stmt *stmt;
    char *sql = "UPDATE records SET balance = ? WHERE account_id = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_bind_double(stmt, 1, r.amount);
    sqlite3_bind_int64(stmt, 2, r.accountNbr);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void makeTransaction(struct User u) {
    struct Record r;
    int option;
    printf("\nEnter the account number of the customer:");
    scanf("%d", &r.accountNbr);
    int count = checkAccountNoForUser(r.accountNbr, u.name);
    if (count != 1) {
        printf("✖ This Account not exists for this user\n\n");
        fail(u);
    }
    sqlite3 *db;
    int rc;
    rc = sqlite3_open(FILENAME2, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_stmt *stmt;
    char *sql = "SELECT * FROM records WHERE user_name = ? AND account_id = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_bind_text(stmt, 1, u.name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, r.accountNbr);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        r.id = sqlite3_column_int(stmt, 0);
        r.userId = sqlite3_column_int(stmt, 1);
        strcpy(r.name, sqlite3_column_text(stmt, 2));
        r.accountNbr = sqlite3_column_int(stmt, 3);
        r.deposit.month = sqlite3_column_int(stmt, 4);
        r.deposit.day = sqlite3_column_int(stmt, 5);
        r.deposit.year = sqlite3_column_int(stmt, 6);
        strcpy(r.country, sqlite3_column_text(stmt, 7));
        r.phone = sqlite3_column_int(stmt, 8);
        r.amount = sqlite3_column_double(stmt, 9);
        strcpy(r.accountType, sqlite3_column_text(stmt, 10));
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    if (strcmp(r.accountType, "fixed01") == 0 || strcmp(r.accountType, "fixed02") == 0 || strcmp(r.accountType, "fixed03") == 0) {
        printf("\nAccounts of type fixed01, fixed02 and fixed03 are not allowed to make transactions\n\n");
        fail(u);
    }
    printf("\nDo you want to:\n\t1-> Withdraw\n\t2-> Deposit\n\n");
    printf("Enter your choice:");
    scanf("%d", &option);
    double transaction;
    switch (option) 
    {
    case 1:
        printf("Enter the amount you want to Withdraw:");
        scanf("%lf", &transaction);
        if (r.amount < transaction) {
            printf("\nThe amount you choice to withdraw is superior to your available balance\n\n");
            fail(u);
        }
        transaction *= -1; 
        break;
    case 2:
        printf("Enter the amount you want to Deposit:");
        scanf("%lf", &transaction);
        break;
    default:
        printf("Insert a valid operation!\n");
        exit(1);
    }
    r.amount += transaction;
    updateAmount(r);
    success(u);
}

void removeAccount(struct User u) {
    struct Record r;
    printf("\nEnter the account number you want to delete:");
    scanf("%d", &r.accountNbr);
    int count = checkAccountNoForUser(r.accountNbr, u.name);
    if (count != 1) {
        printf("✖ This Account not exists for this user\n\n");
        fail(u);
    }
    sqlite3 *db;
    int rc;
    rc = sqlite3_open(FILENAME2, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    
    sqlite3_stmt *stmt;
    char *sql = "SELECT * FROM records WHERE user_name = ? AND account_id = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_bind_text(stmt, 1, u.name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, r.accountNbr);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        r.id = sqlite3_column_int(stmt, 0);
        r.userId = sqlite3_column_int(stmt, 1);
        strcpy(r.name, sqlite3_column_text(stmt, 2));
        r.accountNbr = sqlite3_column_int(stmt, 3);
        r.deposit.month = sqlite3_column_int(stmt, 4);
        r.deposit.day = sqlite3_column_int(stmt, 5);
        r.deposit.year = sqlite3_column_int(stmt, 6);
        strcpy(r.country, sqlite3_column_text(stmt, 7));
        r.phone = sqlite3_column_int(stmt, 8);
        r.amount = sqlite3_column_double(stmt, 9);
        strcpy(r.accountType, sqlite3_column_text(stmt, 10));
        system("clear");
        printf("_____________________\n");
        printf("\nAccount number:%d\nDeposit Date:%d/%d/%d \ncountry:%s \nPhone number:%d \nAmount deposited: $%.2f \nType Of Account:%s\n\n",
            r.accountNbr,
            r.deposit.day,
            r.deposit.month,
            r.deposit.year,
            r.country,
            r.phone,
            r.amount,
            r.accountType);
    }
    char *sql2 = "DELETE FROM records WHERE user_name = ? AND account_id = ?";
    rc = sqlite3_prepare_v2(db, sql2, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_bind_text(stmt, 1, u.name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, r.accountNbr);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    success(u);
}

int checkUser(char name[50]) {
    sqlite3 *db;
    int rc;
    rc = sqlite3_open(FILENAME2, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_stmt *stmt;
    char *sql = "SELECT COUNT(*) FROM users WHERE user_name = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    int count = sqlite3_column_int(stmt, 0);
    if (count != 1) {
        return 0;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1;
}

int getAccountId2(char name[50]) {
    sqlite3 *db;
    int rc;
    int n = 0;
    rc = sqlite3_open(FILENAME2, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_stmt *stmt;
    char *sql = "SELECT user_id FROM users WHERE user_name = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_TRANSIENT);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        n = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return n;
}

void transferAccount(struct User u) {
    struct Record r;
    printf("\nEnter the account number you want to transfer ownership:");
    scanf("%d", &r.accountNbr);
    int count = checkAccountNoForUser(r.accountNbr, u.name);
    if (count != 1) {
        printf("✖ This Account not exists for this user\n\n");
        fail(u);
    }
    sqlite3 *db;
    int rc;
    rc = sqlite3_open(FILENAME2, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    
    sqlite3_stmt *stmt;
    char *sql = "SELECT * FROM records WHERE user_name = ? AND account_id = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_bind_text(stmt, 1, u.name, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, r.accountNbr);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        r.id = sqlite3_column_int(stmt, 0);
        r.userId = sqlite3_column_int(stmt, 1);
        strcpy(r.name, sqlite3_column_text(stmt, 2));
        r.accountNbr = sqlite3_column_int(stmt, 3);
        r.deposit.month = sqlite3_column_int(stmt, 4);
        r.deposit.day = sqlite3_column_int(stmt, 5);
        r.deposit.year = sqlite3_column_int(stmt, 6);
        strcpy(r.country, sqlite3_column_text(stmt, 7));
        r.phone = sqlite3_column_int(stmt, 8);
        r.amount = sqlite3_column_double(stmt, 9);
        strcpy(r.accountType, sqlite3_column_text(stmt, 10));
        system("clear");
        printf("_____________________\n");
        printf("\nAccount number:%d\nDeposit Date:%d/%d/%d \ncountry:%s \nPhone number:%d \nAmount deposited: $%.2f \nType Of Account:%s\n\n",
            r.accountNbr,
            r.deposit.day,
            r.deposit.month,
            r.deposit.year,
            r.country,
            r.phone,
            r.amount,
            r.accountType);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    char newUser[50];
    printf("\n\nWhich user you want to transfer ownership to (user name):");
    scanf("%s", newUser);
    if (checkUser(newUser) != 1) {
        printf("\n\nX User selected not available!!\n\n");
        fail(u);
    }
    int userId = getAccountId2(newUser);
    rc = sqlite3_open(FILENAME2, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    char *sql2 = "UPDATE records SET user_id = ?, user_name = ? WHERE account_id = ?";
    rc = sqlite3_prepare_v2(db, sql2, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_bind_int64(stmt, 1, userId);
    sqlite3_bind_text(stmt, 2, newUser, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 3, r.accountNbr);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    success(u);
}