#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>

using namespace std;

const int MAX_BOOKS = 100;
const int MAX_MEMBERS = 100;

class Book {
public:
    int id;
    string title;
    string author;
    string genre;
    int publicationYear;
    bool isAvailable;

    Book() : id(0), isAvailable(true) {}
};

class Member {
public:
    int memberId;
    string name;
    string address;
    string contactInfo;

    Member() : memberId(0) {}
};

struct Loan {
    int bookId;
    int memberId;
    time_t dueDate;
};

class LibrarySystem {
private:
    Book books[MAX_BOOKS];
    Member members[MAX_MEMBERS];
    Loan loans[MAX_BOOKS]; // Assuming a member can borrow only one book at a time
    int bookCount;
    int memberCount;
    int loanCount;

public:
    LibrarySystem() : bookCount(0), memberCount(0), loanCount(0) {
        loadBooks();
        loadMembers();
    }

    void addBook() {
        if (bookCount >= MAX_BOOKS) {
            cout << "Cannot add more books. Maximum limit is reached.\n";
            return;
        }

        Book newBook;
        cout << "Enter Book ID: ";
        cin >> newBook.id;
        cin.ignore();
        cout << "Enter Title: ";
        getline(cin, newBook.title);
        cout << "Enter Author: ";
        getline(cin, newBook.author);
        cout << "Enter Genre: ";
        getline(cin, newBook.genre);
        cout << "Enter Publication Year: ";
        cin >> newBook.publicationYear;

        books[bookCount++] = newBook;
        cout << "Book added successfully!\n";
        saveBooks();
    }

    void removeBook() {
        int id;
        cout << "Enter Book ID to remove: ";
        cin >> id;

        for (int i = 0; i < bookCount; ++i) {
            if (books[i].id == id) {
                books[i] = books[--bookCount]; // Replace with the last book and decrease count
                cout << "Book removed successfully!\n";
                saveBooks();
                return;
            }
        }
        cout << "Book not found!\n";
    }

    void registerMember() {
        if (memberCount >= MAX_MEMBERS) {
            cout << "Cannot add more members. Maximum limit reached.\n";
            return;
        }

        Member newMember;
        cout << "Enter Member ID: ";
        cin >> newMember.memberId;
        cin.ignore();
        cout << "Enter Name: ";
        getline(cin, newMember.name);
        cout << "Enter Address: ";
        getline(cin, newMember.address);
        cout << "Enter Contact Info: ";
        getline(cin, newMember.contactInfo);

        members[memberCount++] = newMember;
        cout << "Member registered successfully!\n";
        saveMembers();
    }

    void removeMember() {
        int memberId;
        cout << "Enter Member ID to remove: ";
        cin >> memberId;

        for (int i = 0; i < memberCount; ++i) {
            if (members[i].memberId == memberId) {
                members[i] = members[--memberCount]; // Replace with the last member and decrease count
                cout << "Member removed successfully!\n";
                saveMembers();
                return;
            }
        }
        cout << "Member not found!\n";
    }

    void borrowBook() {
        int memberId, bookId;
        cout << "Enter Member ID: ";
        cin >> memberId;
        cout << "Enter Book ID: ";
        cin >> bookId;

        for (int i = 0; i < memberCount; ++i) {
            if (members[i].memberId == memberId) {
                for (int j = 0; j < bookCount; ++j) {
                    if (books[j].id == bookId && books[j].isAvailable) {
                        books[j].isAvailable = false;
                        loans[loanCount].bookId = bookId;
                        loans[loanCount].memberId = memberId;
                        loans[loanCount].dueDate = time(nullptr) + 14 * 24 * 60 * 60; loanCount++;
                        cout << "Book borrowed successfully! Due date is set.\n";
                        saveBooks();
                        return;
                    }
                }
                cout << "Book is unavailable or not found.\n";
                return;
            }
        }
        cout << "Member not found!\n";
    }

    void returnBook() {
        int memberId, bookId;
        cout << "Enter Member ID: ";
        cin >> memberId;
        cout << "Enter Book ID: ";
        cin >> bookId;

        for (int i = 0; i < loanCount; ++i) {
            if (loans[i].memberId == memberId && loans[i].bookId == bookId) {
                time_t currentTime = time(nullptr);
                double secondsLate = difftime(currentTime, loans[i].dueDate);
                int fine = 0;

                if (secondsLate > 0) {
                    fine = static_cast<int>(secondsLate / (60 * 60 * 24)) * 5; // Rs 5 per day late
                    cout << "Book is overdue! Fine: Rs " << fine << "\n";
                } else {
                    cout << "Book returned on time.\n";
                }

                // Mark the book as available
                for (int j = 0; j < bookCount; ++j) {
                    if (books[j].id == bookId) {
                        books[j].isAvailable = true;
                        break;
                    }
                }

                // Remove the loan record
                loans[i] = loans[--loanCount]; // Replace with the last loan and decrease count
                saveBooks();
                return;
            }
        }
        cout << "Loan record not found!\n";
    }

    void generateReport() {
        cout << "\nCurrently Borrowed Books:\n";
        for (int i = 0; i < loanCount; ++i) {
            for (int j = 0; j < bookCount; ++j) {
                if (books[j].id == loans[i].bookId) {
                    cout << "Book ID: " << books[j].id << ", Title: " << books[j].title
                         << ", Borrowed by Member ID: " << loans[i].memberId
                         << ", Due Date: " << ctime(&loans[i].dueDate);
                }
            }
        }
    }

    void showOverdueList() {
        cout << "\nOverdue Books:\n";
        time_t currentTime = time(nullptr);
        for (int i = 0; i < loanCount; ++i) {
            if (difftime(currentTime, loans[i].dueDate) > 0) {
                for (int j = 0; j < bookCount; ++j) {
                    if (books[j].id == loans[i].bookId) {
                        cout << "Book ID: " << books[j].id << ", Title: " << books[j].title
                             << ", Borrowed by Member ID: " << loans[i].memberId
                             << ", Days Overdue: " << static_cast<int>(difftime(currentTime, loans[i].dueDate) / (60 * 60 * 24)) << "\n";
                    }
                }
            }
        }
    }

    void saveBooks() {
        ofstream outFile("books.txt");
        for (int i = 0; i < bookCount; ++i) {
            outFile << books[i].id << "\n" << books[i].title << "\n" << books[i].author << "\n"
                    << books[i].genre << "\n" << books[i].publicationYear << "\n"
                    << books[i].isAvailable << "\n";
        }
        outFile.close();
    }

    void loadBooks() {
        ifstream inFile("books.txt");
        while (inFile >> books[bookCount].id) {
            inFile.ignore();
            getline(inFile, books[bookCount].title);
            getline(inFile, books[bookCount].author);
            getline(inFile, books[bookCount].genre);
            inFile >> books[bookCount].publicationYear;
            inFile >> books[bookCount].isAvailable;
            inFile.ignore();
            bookCount++;
        }
        inFile.close();
    }

    void saveMembers() {
        ofstream outFile("members.txt");
        for (int i = 0; i < memberCount; ++i) {
            outFile << members[i].memberId << "\n" << members[i].name << "\n"
                    << members[i].address << "\n" << members[i].contactInfo << "\n";
        }
        outFile.close();
    }

    void loadMembers() {
        ifstream inFile("members.txt");
        while (inFile >> members[memberCount].memberId) {
            inFile.ignore();
            getline(inFile, members[memberCount].name);
            getline(inFile, members[memberCount].address);
            getline(inFile, members[memberCount].contactInfo);
            memberCount++;
        }
        inFile.close();
    }
};

int main() {
    LibrarySystem library;
    int choice;

    do {
        cout << "\nLibrary Management System\n";
        cout << "1. Add Book\n";
        cout << "2. Remove Book\n";
        cout << "3. Register Member\n";
        cout << "4. Remove Member\n";
        cout << "5. Borrow Book\n";
        cout << "6. Return Book\n";
        cout << "7. Generate Report\n";
        cout << "8. Show Overdue List\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: library.addBook(); break;
            case 2: library.removeBook(); break;
            case 3: library.registerMember(); break;
            case 4: library.removeMember(); break;
            case 5: library.borrowBook(); break;
            case 6: library.returnBook(); break;
            case 7: library.generateReport(); break;
            case 8: library.showOverdueList(); break;
            case 9: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice. Please try again.\n"; break;
        }
    } while (choice != 9);

    return 0;
}