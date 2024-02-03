#include <termios.h>
#include "header.h"
#include "sqlite3.h"

const char *FILENAME = "./data/atm.db";


void loginMenu(char a[50], char pass[50])
{
    struct termios oflags, nflags;

    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Login:");
    scanf("%s", a);

    // disabling echo
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
    {
        perror("tcsetattr");
        return exit(1);
    }
    printf("\n\n\n\n\n\t\t\t\tEnter the password to login:");
    scanf("%s", pass);

    // restore terminal
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
    {
        perror("tcsetattr");
        return exit(1);
    }
};

const char *getPassword(struct User u)
{
    sqlite3 *db;
    int rc;
    rc = sqlite3_open(FILENAME, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    struct User userChecker;
    sqlite3_stmt *stmt;
    char *sql = "SELECT * FROM users WHERE user_name = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_bind_text(stmt, 1, u.name, -1, SQLITE_TRANSIENT);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        u.id = sqlite3_column_int(stmt, 0);
        strcpy(userChecker.name, sqlite3_column_text(stmt, 1));
        strcpy(userChecker.password, sqlite3_column_text(stmt, 2));
        if (strcmp(userChecker.name, u.name) == 0)
        {
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            char *buff = userChecker.password;
            return buff;
        }
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return "no user found";
}

void registerMenu(char a[50], char pass[50]) {
    struct termios oflags, nflags;

    system("clear");
    printf("\n\n\n\t\t\t\t   Bank Management System\n\t\t\t\t\t User Name:");
    scanf("%s", a);

    // disabling echo
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0)
    {
        perror("tcsetattr");
        return exit(1);
    }
    printf("\n\n\n\n\n\t\t\t\tEnter the User password:");
    scanf("%s", pass);
    if (strlen(a) > 50 || strlen(pass) > 50) {
        printf("The length of user/password not allowed you should not exceed 50 char!!\n\n");
        return exit(1);
    }
    // restore terminal
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0)
    {
        perror("tcsetattr");
        return exit(1);
    }
}

int insertUser(struct User u) {
    sqlite3 *db;
    int rc;
    rc = sqlite3_open(FILENAME, &db);
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
    sqlite3_bind_text(stmt, 1, u.name, -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        if (count != 0) {
            return -1;
        } else {
            char *sql = "INSERT INTO users (user_name, user_pass) VALUES (?, ?)";
            rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
            if (rc != SQLITE_OK) {
                fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                exit(1);
            }
            sqlite3_bind_text(stmt, 1, u.name, -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, u.password, -1, SQLITE_TRANSIENT);
            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                exit(1);
            }
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return 1;
        }
    } else {
        fprintf(stderr, "Error retrieving count\n");
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return -1;
}

int getAccountId(struct User u) {
    sqlite3 *db;
    int rc;
    int n = 0;
    rc = sqlite3_open(FILENAME, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_stmt *stmt;
    char *sql = "SELECT user_id FROM users WHERE user_name = ?";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(1);
    }
    sqlite3_bind_text(stmt, 1, u.name, -1, SQLITE_TRANSIENT);
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        n = sqlite3_column_int(stmt, 0);
    }
    sqlite3_close(db);
    return n;
}