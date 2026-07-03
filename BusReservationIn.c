#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

struct User {
    char username[50];
    char password[50];
};

struct Bus {
    int id;
    char name[50];
    char source[50];
    char dest[50];
    int capacity;
    int available;
    char schedule[50];
    int price;
};

struct Ticket {
    int ticketId;
    char username[50];
    int busId;
    int seats;
    int amount;
    int status;
};

struct User users[100];
int userCount = 0;

struct Bus buses[10];
int busCount = 0;

struct Ticket tickets[100];
int ticketCount = 0;

char currentUser[50];

void mySleep(int seconds) {
    clock_t start = clock();
    while (clock() < start + seconds * CLOCKS_PER_SEC);
}

void setupBuses() {
    buses[0] = (struct Bus){1, "RSRTC Express (Rajasthan)", "Jaipur", "Dholpur", 40, 40, "10:00 AM", 250};
    buses[1] = (struct Bus){2, "DTC Volvo (Delhi)", "Delhi", "Agra", 30, 30, "12:30 PM", 500};
    buses[2] = (struct Bus){3, "KSRTC Airavat (Karnataka)", "Bangalore", "Mysore", 45, 45, "09:00 PM", 300};
    buses[3] = (struct Bus){4, "RSRTC Sleeper", "Jodhpur", "Dholpur", 35, 35, "11:00 PM", 600};
    busCount = 4;
}

int checkUPI(int bank, char upi[]) {
    if (bank == 1 && strstr(upi, "@sbi")) return 1;
    if (bank == 2 && strstr(upi, "@hdfc")) return 1;
    if (bank == 3 && strstr(upi, "@icici")) return 1;
    return 0;
}

void clearInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void registerUser() {
    printf("\n--- REGISTER ---\n");
    printf("Enter Username: ");
    scanf("%s", users[userCount].username);
    printf("Enter Password: ");
    scanf("%s", users[userCount].password);
    userCount++;
    printf("Registration Successful! Please login.\n");
}

int loginUser() {
    char u[50], p[50];
    printf("\n--- LOGIN ---\n");
    printf("Username: ");
    scanf("%s", u);
    printf("Password: ");
    scanf("%s", p);
    
    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, u) == 0 && strcmp(users[i].password, p) == 0) {
            strcpy(currentUser, u);
            printf("Login Successful! Welcome %s\n", currentUser);
            return 1;
        }
    }
    printf("Invalid Credentials!\n");
    return 0;
}

void showBuses() {
    printf("\n--- AVAILABLE BUSES ---\n");
    for (int i = 0; i < busCount; i++) {
        printf("%d. %s | %s to %s | Time: %s | Seats: %d/%d | Price: %d\n",
               buses[i].id, buses[i].name, buses[i].source, buses[i].dest,
               buses[i].schedule, buses[i].available, buses[i].capacity, buses[i].price);
    }
}

void bookTicket() {
    showBuses();
    int bId, seats;
    printf("\nEnter Bus ID to book (or 0 to go back): ");
    scanf("%d", &bId);
    if (bId == 0) return;
    
    int bIndex = bId - 1;
    if (bIndex < 0 || bIndex >= busCount) {
        printf("Invalid Bus ID!\n");
        return;
    }
    
    printf("Enter number of seats (Bulk Booking allowed): ");
    scanf("%d", &seats);
    
    if (seats > buses[bIndex].available) {
        printf("Not enough seats available!\n");
        return;
    }
    
    int total = seats * buses[bIndex].price;
    printf("Total Amount: Rs. %d\n", total);
    
    int bank;
    char upi[50];
    printf("\nSelect Bank for Payment:\n1. SBI (@sbi)\n2. HDFC (@hdfc)\n3. ICICI (@icici)\nEnter choice: ");
    scanf("%d", &bank);
    if(bank < 1 || bank > 3) {
        printf("Invalid Bank!\n");
        return;
    }
    
    printf("Enter UPI ID: ");
    scanf("%s", upi);
    
    if (!checkUPI(bank, upi)) {
        printf("Payment Failed! UPI ID does not match the selected bank.\n");
        return;
    }
    
    printf("\nProcessing Payment");
    for (int i = 5; i > 0; i--) {
        printf("...%d", i);
        fflush(stdout);
        mySleep(1);
    }
    
    buses[bIndex].available -= seats;
    tickets[ticketCount].ticketId = ticketCount + 100;
    strcpy(tickets[ticketCount].username, currentUser);
    tickets[ticketCount].busId = bId;
    tickets[ticketCount].seats = seats;
    tickets[ticketCount].amount = total;
    tickets[ticketCount].status = 1;
    ticketCount++;
    
    printf("\nPayment Successful! Thanks for booking.\n");
    printf("Ticket ID: %d generated.\n", tickets[ticketCount-1].ticketId);
}

void viewHistory() {
    printf("\n--- YOUR TICKETS ---\n");
    int found = 0;
    for (int i = 0; i < ticketCount; i++) {
        if (strcmp(tickets[i].username, currentUser) == 0) {
            found = 1;
            char stat[20];
            strcpy(stat, tickets[i].status == 1 ? "CONFIRMED" : "CANCELLED");
            int bIndex = tickets[i].busId - 1;
            printf("ID: %d | Bus: %s | Route: %s-%s | Seats: %d | Total: %d | Status: %s\n",
                   tickets[i].ticketId, buses[bIndex].name, buses[bIndex].source,
                   buses[bIndex].dest, tickets[i].seats, tickets[i].amount, stat);
        }
    }
    if (!found) printf("No tickets booked yet.\n");
}

void cancelTicket() {
    viewHistory();
    int tId;
    printf("\nEnter Ticket ID to cancel (or 0 to go back): ");
    scanf("%d", &tId);
    if(tId == 0) return;
    
    for (int i = 0; i < ticketCount; i++) {
        if (tickets[i].ticketId == tId && strcmp(tickets[i].username, currentUser) == 0) {
            if (tickets[i].status == 0) {
                printf("Ticket is already cancelled.\n");
                return;
            }
            tickets[i].status = 0;
            buses[tickets[i].busId - 1].available += tickets[i].seats;
            printf("Ticket %d Cancelled Successfully! Amount Rs.%d will be refunded.\n", tId, tickets[i].amount);
            return;
        }
    }
    printf("Invalid Ticket ID.\n");
}

void userDashboard() {
    int choice;
    while(1) {
        printf("\n=== DASHBOARD ===\n");
        printf("1. View Buses & Routes\n2. Book Ticket\n3. View History\n4. Cancel Ticket\n5. Logout\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        
        if (choice == 1) showBuses();
        else if (choice == 2) bookTicket();
        else if (choice == 3) viewHistory();
        else if (choice == 4) cancelTicket();
        else if (choice == 5) {
            strcpy(currentUser, "");
            printf("Logged out.\n");
            break;
        }
    }
}

int main() {
    setupBuses();
    int choice;
    
    while(1) {
        printf("\n=== BUS RESERVATION SYSTEM ===\n");
        printf("1. Register\n2. Login\n3. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        
        if (choice == 1) {
            registerUser();
        } else if (choice == 2) {
            if (loginUser()) {
                userDashboard();
            }
        } else if (choice == 3) {
            printf("Exiting system. Thank you!\n");
            break;
        } else {
            printf("Invalid choice!\n");
        }
    }
    return 0;
}
