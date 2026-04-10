/*
Project Title: Bus Reservation and Cancellation System - "Seat Release and Revenue"
Student Name: Sarbajeet Pattadar
Register No: 25BCE1920
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
using namespace std;

// Route class: stores route details and seat info
class Route {
private:
    int routeID;
    string from, to;
    float fare;
    int capacity;
    vector<int> bookedSeats; // stores booked seat numbers

public:
    Route(int id, string f, string t, float fa, int cap) {
        routeID = id;
        from = f;
        to = t;
        fare = fa;
        capacity = cap;
    }

    int getID() { return routeID; }
    string getFrom() { return from; }
    string getTo() { return to; }
    float getFare() { return fare; }
    int getCapacity() { return capacity; }

    // check if seat already booked
    bool isSeatBooked(int seat) {
        for (int s : bookedSeats) {
            if (s == seat) return true;
        }
        return false;
    }

    // try booking seat
    bool bookSeat(int seat) {
        if (seat < 1 || seat > capacity) return false;
        if (isSeatBooked(seat)) return false;
        bookedSeats.push_back(seat);
        return true;
    }

    // release seat during cancellation
    void releaseSeat(int seat) {
        for (int i = 0; i < bookedSeats.size(); i++) {
            if (bookedSeats[i] == seat) {
                bookedSeats.erase(bookedSeats.begin() + i);
                return;
            }
        }
    }

    // display route info
    void display() {
        cout << "Route " << routeID << ": "
             << from << " -> " << to
             << " | Fare: Rs." << fare << "\n";
    }
};

// Ticket class: stores one booking
class Ticket {
private:
    string ticketID;
    string name;
    int routeID;
    int seatNo;
    float amount;
    bool cancelled;

public:
    Ticket(string id, string n, int r, int s, float a) {
        ticketID = id;
        name = n;
        routeID = r;
        seatNo = s;
        amount = a;
        cancelled = false;
    }

    string getID() { return ticketID; }
    int getRouteID() { return routeID; }
    int getSeat() { return seatNo; }
    float getAmount() { return amount; }
    bool isCancelled() { return cancelled; }

    // mark ticket as cancelled
    void cancel() { cancelled = true; }

    // display ticket details
    void display() {
        cout << "TicketID: " << ticketID
             << " | Name: " << name
             << " | Route: " << routeID
             << " | Seat: " << seatNo
             << " | Status: " << (cancelled ? "Cancelled" : "Active") << "\n";
    }

    // save ticket to file
    void save(ofstream &file) {
        file << ticketID << " " << name << " "
             << routeID << " " << seatNo << " "
             << amount << " " << cancelled << "\n";
    }
};

// main system class
class ReservationSystem {
private:
    vector<Route> routes;
    vector<Ticket> tickets;
    int counter;

public:
    ReservationSystem() {
        counter = 0;

        // predefined routes
        routes.push_back(Route(1, "Chennai", "Madurai", 450, 30));
        routes.push_back(Route(2, "Chennai", "Coimbatore", 500, 30));
        routes.push_back(Route(3, "Madurai", "Trichy", 250, 20));

        load(); // load old data if exists
    }

    // generate simple ticket id
    string generateID() {
        counter++;
        return "T" + to_string(counter);
    }

    // find route by id
    Route* findRoute(int id) {
        for (int i = 0; i < routes.size(); i++) {
            if (routes[i].getID() == id)
                return &routes[i];
        }
        return NULL;
    }

    // show all routes
    void showRoutes() {
        for (int i = 0; i < routes.size(); i++) {
            routes[i].display();
        }
    }

    // booking function
    void bookTicket() {
        int rid, seat;
        string name;

        showRoutes();

        cout << "Enter Route ID: ";
        cin >> rid;

        Route* r = findRoute(rid);
        if (r == NULL) {
            cout << "Invalid Route\n";
            return;
        }

        cout << "Enter Seat Number: ";
        cin >> seat;

        if (seat <= 0) {
            cout << "Invalid seat\n";
            return;
        }

        if (!r->bookSeat(seat)) {
            cout << "Seat not available\n";
            return;
        }

        cout << "Enter Name: ";
        cin.ignore();
        getline(cin, name);

        if (name == "") {
            cout << "Name cannot be empty\n";
            return;
        }

        string id = generateID();
        tickets.push_back(Ticket(id, name, rid, seat, r->getFare()));

        save();

        cout << "Ticket Booked! ID: " << id << "\n";
    }

    // cancel ticket
    void cancelTicket() {
        string id;
        cout << "Enter Ticket ID: ";
        cin >> id;

        for (int i = 0; i < tickets.size(); i++) {
            if (tickets[i].getID() == id && !tickets[i].isCancelled()) {
                Route* r = findRoute(tickets[i].getRouteID());
                r->releaseSeat(tickets[i].getSeat());
                tickets[i].cancel();
                save();
                cout << "Cancelled\n";
                return;
            }
        }

        cout << "Ticket not found\n";
    }

    // search ticket
    void searchTicket() {
        string id;
        cout << "Enter Ticket ID: ";
        cin >> id;

        for (int i = 0; i < tickets.size(); i++) {
            if (tickets[i].getID() == id) {
                tickets[i].display();
                return;
            }
        }

        cout << "Not found\n";
    }

    // generate report
    void report() {
        for (int i = 0; i < routes.size(); i++) {
            int rid = routes[i].getID();
            int count = 0;
            float revenue = 0;

            for (int j = 0; j < tickets.size(); j++) {
                if (tickets[j].getRouteID() == rid && !tickets[j].isCancelled()) {
                    count++;
                    revenue += tickets[j].getAmount();
                }
            }

            cout << "Route " << rid
                 << " | Tickets: " << count
                 << " | Revenue: " << revenue << "\n";
        }
    }

    // save all tickets to file
    void save() {
        ofstream file("data.txt");
        file << counter << "\n";

        for (int i = 0; i < tickets.size(); i++) {
            tickets[i].save(file);
        }

        file.close();
    }

    // load data from file
    void load() {
        ifstream file("data.txt");
        if (!file) return;

        file >> counter;

        string id, name;
        int rid, seat;
        float amt;
        bool cancel;

        while (file >> id >> name >> rid >> seat >> amt >> cancel) {
            Ticket t(id, name, rid, seat, amt);
            if (cancel) t.cancel();
            else {
                Route* r = findRoute(rid);
                if (r) r->bookSeat(seat);
            }
            tickets.push_back(t);
        }

        file.close();
    }
};

// main function
int main() {
    ReservationSystem system;
    int choice;
    cout << "BUS RESERVATION AND CANCELLATION - \"SEAT RELEASE AND REVENUE\"\n";
    do {
        cout << "\n1. View Routes\n2. Book Ticket\n3. Cancel Ticket\n4. Search Ticket\n5. Report\n0. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: system.showRoutes(); break;
            case 2: system.bookTicket(); break;
            case 3: system.cancelTicket(); break;
            case 4: system.searchTicket(); break;
            case 5: system.report(); break;
            case 0: cout << "Exit\n"; break;
            default: cout << "Invalid\n";
        }

    } while (choice != 0);

    return 0;
}
