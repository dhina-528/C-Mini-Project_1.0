#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define CLR_RESET   "\033[0m"
#define CLR_BOLD    "\033[1m"
#define CLR_RED     "\033[1;31m"
#define CLR_GREEN   "\033[1;32m"
#define CLR_YELLOW  "\033[1;33m"
#define CLR_CYAN    "\033[1;36m"

#define MAX_ACCOUNTS   100
#define LAST_NAME_LEN  15
#define FIRST_NAME_LEN 10

struct clientData {
    unsigned int acctNum;
    char         lastName[LAST_NAME_LEN];
    char         firstName[FIRST_NAME_LEN];
    double       balance;
    unsigned int pin;
    char         acctType;
    double       interestRate;
};

unsigned int enterChoice(void);
int          isValidAccount(unsigned int acctNum);
int          readRecord(FILE *fPtr, unsigned int acctNum, struct clientData *client);
void         writeRecord(FILE *fPtr, struct clientData *client);
void         printHeader(void);
void         printRecord(struct clientData *client);
int          verifyPIN(struct clientData *client);
void         logTransaction(unsigned int acctNum, const char *type, double amount, double newBalance);
void         printBanner(void);

void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void listAllAccounts(FILE *fPtr);
void searchByName(FILE *fPtr);
void transferFunds(FILE *fPtr);
void showStatistics(FILE *fPtr);
void applyInterest(FILE *fPtr);
void sortAndDisplay(FILE *fPtr);
void viewHistory(void);
void exportJSON(FILE *fPtr);

int main(void)
{
    FILE        *cfPtr;
    unsigned int choice;

    printBanner();

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL) {
        fprintf(stderr, CLR_RED "ERROR: 'credit.dat' could not be opened.\n" CLR_RESET);
        fprintf(stderr, CLR_YELLOW "Tip: Make sure credit.dat exists in the same folder.\n" CLR_RESET);
        exit(EXIT_FAILURE);
    }

    while ((choice = enterChoice()) != 0) {
        switch (choice) {
        case 1:  textFile(cfPtr);        break;
        case 2:  updateRecord(cfPtr);    break;
        case 3:  newRecord(cfPtr);       break;
        case 4:  deleteRecord(cfPtr);    break;
        case 5:  listAllAccounts(cfPtr); break;
        case 6:  searchByName(cfPtr);    break;
        case 7:  transferFunds(cfPtr);   break;
        case 8:  showStatistics(cfPtr);  break;
        case 9:  applyInterest(cfPtr);   break;
        case 10: sortAndDisplay(cfPtr);  break;
        case 11: viewHistory();          break;
        case 12: exportJSON(cfPtr);      break;
        default:
            printf(CLR_RED "Invalid choice! Enter 0-12.\n" CLR_RESET);
            break;
        }
    }

    fclose(cfPtr);
    printf(CLR_CYAN "\nThank you for using TPS v2.0. Goodbye!\n" CLR_RESET);
    return 0;
}

void printBanner(void)
{
    printf(CLR_CYAN CLR_BOLD);
    printf("\n  +--------------------------------------------------+\n");
    printf("  |                                                  |\n");
    printf("  |    TRANSACTION PROCESSING SYSTEM  v2.0          |\n");
    printf("  |         Bank Account Management                 |\n");
    printf("  |         24UCS271 Lab - Mini Project             |\n");
    printf("  |                                                  |\n");
    printf("  +--------------------------------------------------+\n");
    printf(CLR_RESET "\n");
}

unsigned int enterChoice(void)
{
    unsigned int choice;
    printf(CLR_CYAN CLR_BOLD "\n============== MENU =============\n" CLR_RESET);
    printf("  1  - Export accounts to .txt\n");
    printf("  2  - Update account balance     " CLR_YELLOW "[PIN]\n" CLR_RESET);
    printf("  3  - Add new account\n");
    printf("  4  - Delete account             " CLR_YELLOW "[PIN]\n" CLR_RESET);
    printf("  5  - List all accounts\n");
    printf("  6  - Search by name\n");
    printf("  7  - Transfer funds             " CLR_YELLOW "[PIN]\n" CLR_RESET);
    printf("  8  - Account statistics\n");
    printf(CLR_GREEN "  9  - Apply interest (Savings)   [NEW]\n" CLR_RESET);
    printf(CLR_GREEN "  10 - Sort & display accounts    [NEW]\n" CLR_RESET);
    printf(CLR_GREEN "  11 - View transaction history   [NEW]\n" CLR_RESET);
    printf(CLR_GREEN "  12 - Export accounts to JSON    [NEW]\n" CLR_RESET);
    printf("  0  - Exit\n");
    printf(CLR_CYAN "=================================\n" CLR_RESET);
    printf("Enter your choice: ");

    if (scanf("%u", &choice) != 1) {
        while (getchar() != '\n');
        return 99;
    }
    while (getchar() != '\n');
    return choice;
}

int isValidAccount(unsigned int acctNum)
{
    if (acctNum < 1 || acctNum > MAX_ACCOUNTS) {
        printf(CLR_RED "ERROR: Account number must be 1-%d.\n" CLR_RESET, MAX_ACCOUNTS);
        return 0;
    }
    return 1;
}

int readRecord(FILE *fPtr, unsigned int acctNum, struct clientData *client)
{
    fseek(fPtr, (long)(acctNum - 1) * (long)sizeof(struct clientData), SEEK_SET);
    if (fread(client, sizeof(struct clientData), 1, fPtr) != 1) return 0;
    return 1;
}

void writeRecord(FILE *fPtr, struct clientData *client)
{
    fseek(fPtr, (long)(client->acctNum - 1) * (long)sizeof(struct clientData), SEEK_SET);
    fwrite(client, sizeof(struct clientData), 1, fPtr);
}

void printHeader(void)
{
    printf(CLR_CYAN CLR_BOLD);
    printf("\n%-6s %-15s %-10s %-5s %-10s %12s\n",
           "Acct", "Last Name", "First Name", "Type", "Rate", "Balance");
    printf("%-6s %-15s %-10s %-5s %-10s %12s\n",
           "------", "---------------", "----------", "-----", "----------", "------------");
    printf(CLR_RESET);
}

void printRecord(struct clientData *client)
{
    const char *typeStr = (client->acctType == 'S') ? "SAV" : "CHK";
    if (client->balance < 0)        printf(CLR_RED);
    else if (client->balance > 10000) printf(CLR_GREEN);
    printf("%-6u %-15s %-10s %-5s %9.2f%% %12.2f\n",
           client->acctNum, client->lastName, client->firstName,
           typeStr, client->interestRate * 100.0, client->balance);
    printf(CLR_RESET);
}

int verifyPIN(struct clientData *client)
{
    unsigned int pin;
    printf("Enter PIN for account #%u: ", client->acctNum);
    if (scanf("%u", &pin) != 1) {
        while (getchar() != '\n');
        printf(CLR_RED "ERROR: Invalid PIN input.\n" CLR_RESET);
        return 0;
    }
    while (getchar() != '\n');
    if (pin != client->pin) {
        printf(CLR_RED "ERROR: Incorrect PIN. Access denied.\n" CLR_RESET);
        return 0;
    }
    printf(CLR_GREEN "PIN verified.\n" CLR_RESET);
    return 1;
}

void logTransaction(unsigned int acctNum, const char *type, double amount, double newBalance)
{
    FILE      *logPtr;
    time_t     now;
    struct tm *tinfo;
    char       timeBuf[20];

    time(&now);
    tinfo = localtime(&now);
    strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", tinfo);

    if ((logPtr = fopen("history.log", "a")) == NULL) return;
    fprintf(logPtr, "[%s] ACCT#%-3u | %-12s | %+12.2f | BAL: %12.2f\n",
            timeBuf, acctNum, type, amount, newBalance);
    fclose(logPtr);
}

void textFile(FILE *readPtr)
{
    FILE             *writePtr;
    struct clientData client;
    int               count = 0;

    if ((writePtr = fopen("accounts.txt", "w")) == NULL) {
        printf(CLR_RED "ERROR: accounts.txt could not be created.\n" CLR_RESET);
        return;
    }
    rewind(readPtr);
    fprintf(writePtr, "%-6s %-15s %-10s %-5s %-10s %12s\n",
            "Acct", "Last Name", "First Name", "Type", "Rate", "Balance");
    fprintf(writePtr, "%-6s %-15s %-10s %-5s %-10s %12s\n",
            "------", "---------------", "----------", "-----", "----------", "------------");
    while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1) {
        if (client.acctNum != 0) {
            fprintf(writePtr, "%-6u %-15s %-10s %-5c %9.2f%% %12.2f\n",
                    client.acctNum, client.lastName, client.firstName,
                    client.acctType, client.interestRate * 100.0, client.balance);
            count++;
        }
    }
    fclose(writePtr);
    printf(CLR_GREEN "SUCCESS: %d account(s) written to 'accounts.txt'.\n" CLR_RESET, count);
}

void updateRecord(FILE *fPtr)
{
    unsigned int      account;
    double            transaction;
    struct clientData client = {0, "", "", 0.0, 0, 'C', 0.0};

    printf("Enter account to update (1-%d): ", MAX_ACCOUNTS);
    if (scanf("%u", &account) != 1) {
        printf(CLR_RED "ERROR: Invalid input.\n" CLR_RESET);
        while (getchar() != '\n') {}
        return;
    }
    while (getchar() != '\n');
    if (!isValidAccount(account)) return;
    if (!readRecord(fPtr, account, &client) || client.acctNum == 0) {
        printf(CLR_RED "Account #%u does not exist.\n" CLR_RESET, account); return;
    }
    if (!verifyPIN(&client)) return;

    printf("\nCurrent details:\n");
    printHeader(); printRecord(&client);

    printf("\nEnter charge (+) or payment (-): ");
    if (scanf("%lf", &transaction) != 1) {
        printf(CLR_RED "ERROR: Invalid amount.\n" CLR_RESET);
        while (getchar() != '\n') {}
        return;
    }
    while (getchar() != '\n');

    if (client.balance + transaction < 0.0) {
        printf(CLR_YELLOW "WARNING: This would cause a negative balance.\n" CLR_RESET);
        printf("Balance: %.2f | Transaction: %.2f | Proceed? (y/n): ",
               client.balance, transaction);
        char ans; scanf(" %c", &ans); while (getchar() != '\n');
        if (tolower(ans) != 'y') { printf(CLR_YELLOW "Cancelled.\n" CLR_RESET); return; }
    }

    client.balance += transaction;
    writeRecord(fPtr, &client);
    logTransaction(client.acctNum, (transaction >= 0) ? "DEPOSIT" : "WITHDRAWAL",
                   transaction, client.balance);
    printf("\nUpdated details:\n");
    printHeader(); printRecord(&client);
    printf(CLR_GREEN "Transaction of %.2f applied.\n" CLR_RESET, transaction);
}

void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blank = {0, "", "", 0.0, 0, 'C', 0.0};
    unsigned int      accountNum;
    char              confirm;

    printf("Enter account to delete (1-%d): ", MAX_ACCOUNTS);
    if (scanf("%u", &accountNum) != 1) {
        printf(CLR_RED "ERROR: Invalid input.\n" CLR_RESET);
        while (getchar() != '\n') {}
        return;
    }
    while (getchar() != '\n');
    if (!isValidAccount(accountNum)) return;
    if (!readRecord(fPtr, accountNum, &client) || client.acctNum == 0) {
        printf(CLR_RED "Account #%u does not exist.\n" CLR_RESET, accountNum); return;
    }

    printf("\nAccount to be deleted:\n");
    printHeader(); printRecord(&client);
    if (!verifyPIN(&client)) return;

    printf(CLR_YELLOW "Confirm delete? (y/n): " CLR_RESET);
    scanf(" %c", &confirm); while (getchar() != '\n');

    if (tolower(confirm) == 'y') {
        logTransaction(client.acctNum, "DELETED", -client.balance, 0.0);
        fseek(fPtr, (long)(accountNum - 1) * (long)sizeof(struct clientData), SEEK_SET);
        fwrite(&blank, sizeof(struct clientData), 1, fPtr);
        printf(CLR_GREEN "Account #%u deleted.\n" CLR_RESET, accountNum);
    } else {
        printf(CLR_YELLOW "Deletion cancelled.\n" CLR_RESET);
    }
}

void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0, 0, 'C', 0.0};
    unsigned int      accountNum, pin1, pin2;
    char              typeChar;

    printf("Enter new account number (1-%d): ", MAX_ACCOUNTS);
    if (scanf("%u", &accountNum) != 1) {
        printf(CLR_RED "ERROR: Invalid input.\n" CLR_RESET);
        while (getchar() != '\n') {}
        return;
    }
    while (getchar() != '\n');
    if (!isValidAccount(accountNum)) return;
    if (!readRecord(fPtr, accountNum, &client)) {
        printf(CLR_RED "ERROR: Could not read file.\n" CLR_RESET); return;
    }
    if (client.acctNum != 0) {
        printf(CLR_YELLOW "Account #%u already exists.\n" CLR_RESET, accountNum); return;
    }

    printf("Last Name   : "); scanf("%14s", client.lastName);
    printf("First Name  : "); scanf("%9s", client.firstName);
    while (getchar() != '\n');

    printf("Type (S=Savings / C=Checking): ");
    scanf(" %c", &typeChar); while (getchar() != '\n');
    client.acctType = (toupper(typeChar) == 'S') ? 'S' : 'C';

    if (client.acctType == 'S') {
        printf("Annual Interest Rate (e.g. 0.035): ");
        if (scanf("%lf", &client.interestRate) != 1 || client.interestRate < 0) {
            printf(CLR_RED "ERROR: Invalid rate.\n" CLR_RESET);
            while (getchar() != '\n') {}
        return;
        }
        while (getchar() != '\n');
    }

    printf("Opening Balance : ");
    if (scanf("%lf", &client.balance) != 1 || client.balance < 0) {
        printf(CLR_RED "ERROR: Invalid balance.\n" CLR_RESET);
        while (getchar() != '\n') {}
        return;
    }
    while (getchar() != '\n');

    printf("Set 4-digit PIN    : ");
    if (scanf("%u", &pin1) != 1 || pin1 > 9999) {
        printf(CLR_RED "ERROR: PIN must be 0000-9999.\n" CLR_RESET);
        while (getchar() != '\n') {}
        return;
    }
    printf("Confirm PIN        : ");
    if (scanf("%u", &pin2) != 1 || pin2 != pin1) {
        printf(CLR_RED "ERROR: PINs do not match.\n" CLR_RESET);
        while (getchar() != '\n') {}
        return;
    }
    while (getchar() != '\n');

    client.acctNum = accountNum;
    client.pin     = pin1;
    writeRecord(fPtr, &client);
    logTransaction(client.acctNum, "CREATED", client.balance, client.balance);

    printf(CLR_GREEN "\nAccount created:\n" CLR_RESET);
    printHeader(); printRecord(&client);
}

void listAllAccounts(FILE *fPtr)
{
    struct clientData client;
    int               count = 0;

    rewind(fPtr);
    printf(CLR_BOLD "\n========== All Accounts ==========\n" CLR_RESET);
    printHeader();
    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1) {
        if (client.acctNum != 0) { printRecord(&client); count++; }
    }
    if (count == 0) printf(CLR_YELLOW "  (No accounts found)\n" CLR_RESET);
    printf("\nTotal: %d account(s)\n", count);
}

void searchByName(FILE *fPtr)
{
    struct clientData client;
    char              searchName[LAST_NAME_LEN];
    int               found = 0;

    printf("Enter last name to search: ");
    scanf("%14s", searchName); while (getchar() != '\n');
    for (int i = 0; searchName[i]; i++) searchName[i] = (char)tolower(searchName[i]);

    rewind(fPtr);
    printf(CLR_BOLD "\n====== Results for '%s' ======\n" CLR_RESET, searchName);
    printHeader();
    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1) {
        if (client.acctNum != 0) {
            char copy[LAST_NAME_LEN];
            strncpy(copy, client.lastName, LAST_NAME_LEN - 1);
            copy[LAST_NAME_LEN - 1] = '\0';
            for (int i = 0; copy[i]; i++) copy[i] = (char)tolower(copy[i]);
            if (strstr(copy, searchName)) { printRecord(&client); found++; }
        }
    }
    if (found == 0)
        printf(CLR_YELLOW "  No accounts found.\n" CLR_RESET);
    else
        printf(CLR_GREEN "\n%d account(s) found.\n" CLR_RESET, found);
}

void transferFunds(FILE *fPtr)
{
    struct clientData src, dst;
    unsigned int      srcAcct, dstAcct;
    double            amount;

    printf("Source account (1-%d): ", MAX_ACCOUNTS);
    if (scanf("%u", &srcAcct) != 1 || !isValidAccount(srcAcct)) {
        while (getchar() != '\n') {}
        return;
    }
    while (getchar() != '\n');

    printf("Destination account (1-%d): ", MAX_ACCOUNTS);
    if (scanf("%u", &dstAcct) != 1 || !isValidAccount(dstAcct)) {
        while (getchar() != '\n') {}
        return;
    }
    while (getchar() != '\n');

    if (srcAcct == dstAcct) {
        printf(CLR_RED "ERROR: Source and destination cannot be the same.\n" CLR_RESET); return;
    }
    if (!readRecord(fPtr, srcAcct, &src) || src.acctNum == 0) {
        printf(CLR_RED "ERROR: Source account #%u not found.\n" CLR_RESET, srcAcct); return;
    }
    if (!readRecord(fPtr, dstAcct, &dst) || dst.acctNum == 0) {
        printf(CLR_RED "ERROR: Destination account #%u not found.\n" CLR_RESET, dstAcct); return;
    }

    printf("\nSource:\n");      printHeader(); printRecord(&src);
    printf("\nDestination:\n"); printHeader(); printRecord(&dst);

    if (!verifyPIN(&src)) return;

    printf("\nAmount to transfer: ");
    if (scanf("%lf", &amount) != 1 || amount <= 0) {
        printf(CLR_RED "ERROR: Amount must be positive.\n" CLR_RESET);
        while (getchar() != '\n') {}
        return;
    }
    while (getchar() != '\n');

    if (src.balance < amount) {
        printf(CLR_RED "ERROR: Insufficient funds. Available: %.2f\n" CLR_RESET, src.balance); return;
    }

    src.balance -= amount;
    dst.balance += amount;
    writeRecord(fPtr, &src);
    writeRecord(fPtr, &dst);
    logTransaction(src.acctNum, "TRANSFER-OUT", -amount, src.balance);
    logTransaction(dst.acctNum, "TRANSFER-IN",   amount, dst.balance);

    printf(CLR_GREEN "\nTransfer of %.2f successful!\n" CLR_RESET, amount);
    printf("\nUpdated Source:\n");      printHeader(); printRecord(&src);
    printf("\nUpdated Destination:\n"); printHeader(); printRecord(&dst);
}

void showStatistics(FILE *fPtr)
{
    struct clientData client;
    int          count = 0;
    double       total = 0.0, maxB = -1e18, minB = 1e18;
    unsigned int maxA = 0, minA = 0;

    rewind(fPtr);
    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1) {
        if (client.acctNum != 0) {
            count++;
            total += client.balance;
            if (client.balance > maxB) { maxB = client.balance; maxA = client.acctNum; }
            if (client.balance < minB) { minB = client.balance; minA = client.acctNum; }
        }
    }
    printf(CLR_CYAN CLR_BOLD "\n========== Account Statistics ==========\n" CLR_RESET);
    if (count == 0) { printf(CLR_YELLOW "No accounts.\n" CLR_RESET); return; }
    printf("  Total Accounts  : %d\n",     count);
    printf("  Total Balance   : %.2f\n",   total);
    printf("  Average Balance : %.2f\n",   total / count);
    printf("  Highest Balance : %.2f  (Account #%u)\n", maxB, maxA);
    printf("  Lowest Balance  : %.2f  (Account #%u)\n", minB, minA);
    printf(CLR_CYAN "=========================================\n" CLR_RESET);
}

void applyInterest(FILE *fPtr)
{
    struct clientData client;
    int    count = 0;
    double totalInterest = 0.0;

    rewind(fPtr);
    printf(CLR_BOLD "\n====== Applying Interest to Savings Accounts ======\n" CLR_RESET);
    printHeader();
    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1) {
        if (client.acctNum != 0 && client.acctType == 'S' && client.interestRate > 0) {
            double interest = client.balance * client.interestRate;
            client.balance += interest;
            totalInterest  += interest;
            writeRecord(fPtr, &client);
            logTransaction(client.acctNum, "INTEREST", interest, client.balance);
            printRecord(&client);
            count++;
        }
    }
    if (count == 0) printf(CLR_YELLOW "  No savings accounts found.\n" CLR_RESET);
    printf(CLR_GREEN "\nInterest applied to %d account(s). Total credited: %.2f\n" CLR_RESET,
           count, totalInterest);
}

static int cmpBalanceAsc(const void *a, const void *b) {
    double ba = ((struct clientData *)a)->balance;
    double bb = ((struct clientData *)b)->balance;
    return (ba > bb) - (ba < bb);
}
static int cmpBalanceDesc(const void *a, const void *b) {
    return cmpBalanceAsc(b, a);
}
static int cmpName(const void *a, const void *b) {
    return strcmp(((struct clientData *)a)->lastName,
                  ((struct clientData *)b)->lastName);
}

void sortAndDisplay(FILE *fPtr)
{
    struct clientData accounts[MAX_ACCOUNTS];
    int count = 0;
    unsigned int sortChoice;

    rewind(fPtr);
    while (fread(&accounts[count], sizeof(struct clientData), 1, fPtr) == 1) {
        if (accounts[count].acctNum != 0 && count < MAX_ACCOUNTS) count++;
    }
    if (count == 0) { printf(CLR_YELLOW "No accounts.\n" CLR_RESET); return; }

    printf("\nSort by:\n");
    printf("  1 - Balance (Low to High)\n");
    printf("  2 - Balance (High to Low)\n");
    printf("  3 - Last Name (A to Z)\n");
    printf("Choice: ");
    if (scanf("%u", &sortChoice) != 1) { while (getchar() != '\n') {}
        return; }
    while (getchar() != '\n');

    if      (sortChoice == 1) qsort(accounts, count, sizeof(struct clientData), cmpBalanceAsc);
    else if (sortChoice == 2) qsort(accounts, count, sizeof(struct clientData), cmpBalanceDesc);
    else if (sortChoice == 3) qsort(accounts, count, sizeof(struct clientData), cmpName);
    else { printf(CLR_RED "Invalid sort option.\n" CLR_RESET); return; }

    printf(CLR_BOLD "\n========== Sorted Accounts ==========\n" CLR_RESET);
    printHeader();
    for (int i = 0; i < count; i++) printRecord(&accounts[i]);
    printf("\nTotal: %d account(s)\n", count);
}

void viewHistory(void)
{
    FILE *logPtr;
    char  lines[20][256];
    int   lineCount = 0;
    char  buf[256];

    if ((logPtr = fopen("history.log", "r")) == NULL) {
        printf(CLR_YELLOW "No transaction history found (history.log missing).\n" CLR_RESET);
        return;
    }
    while (fgets(buf, sizeof(buf), logPtr) != NULL) {
        strncpy(lines[lineCount % 20], buf, 255);
        lines[lineCount % 20][255] = '\0';
        lineCount++;
    }
    fclose(logPtr);

    int total  = (lineCount > 20) ? 20 : lineCount;
    int start  = lineCount % 20;

    printf(CLR_CYAN CLR_BOLD "\n====== Last %d Transaction(s) ======\n" CLR_RESET, total);
    for (int i = 0; i < total; i++)
        printf("%s", lines[(start + i) % 20]);
}

void exportJSON(FILE *fPtr)
{
    FILE             *jsonPtr;
    struct clientData client;
    int               first = 1;

    if ((jsonPtr = fopen("accounts.json", "w")) == NULL) {
        printf(CLR_RED "ERROR: accounts.json could not be created.\n" CLR_RESET); return;
    }
    rewind(fPtr);
    fprintf(jsonPtr, "[\n");
    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1) {
        if (client.acctNum != 0) {
            if (!first) fprintf(jsonPtr, ",\n");
            fprintf(jsonPtr, "  {\n");
            fprintf(jsonPtr, "    \"acctNum\": %u,\n",       client.acctNum);
            fprintf(jsonPtr, "    \"lastName\": \"%s\",\n",  client.lastName);
            fprintf(jsonPtr, "    \"firstName\": \"%s\",\n", client.firstName);
            fprintf(jsonPtr, "    \"balance\": %.2f,\n",     client.balance);
            fprintf(jsonPtr, "    \"acctType\": \"%c\",\n",  client.acctType);
            fprintf(jsonPtr, "    \"interestRate\": %.4f\n", client.interestRate);
            fprintf(jsonPtr, "  }");
            first = 0;
        }
    }
    fprintf(jsonPtr, "\n]\n");
    fclose(jsonPtr);
    printf(CLR_GREEN "SUCCESS: accounts.json exported. Open dashboard.html to visualize.\n" CLR_RESET);
}
