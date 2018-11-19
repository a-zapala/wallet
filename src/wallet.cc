#include "wallet.h"
#include <stdexcept>
#include <regex>
#include <iostream>

using namespace std;

constexpr int MAX_STRING_LENGTH_OF_B = 8;
constexpr int MAX_STRING_FRACTIONAL_LENGTH_OF_B = 8;
constexpr int MAX_BINARY_STRING_LENGTH_OF_B = 24;

Wallet::Unit Wallet::numberOfExistingUnit = 0;

Wallet::Wallet() : balance(0), history() {
    createAndAddToBalance(0);
}

Wallet::Wallet(Wallet::Bajtk b) : balance(0), history() {
    Unit units = convertToUnit(b);
    createAndAddToBalance(units);
}

Wallet::Wallet(std::string str) : balance(), history() {
    smatch matchResult;
    regex expr("^\\s*"
               "(0|[1-9]\\d*)"
               "([.,]"
               "\\d{1,8}){0,1}"
               "\\s*$"
    );
    if (regex_search(str, matchResult, expr)) {
        auto itr = matchResult.begin();
        string bString = itr[1].str();
        string unitString = itr[2].str();
        
        if (bString.size() > MAX_STRING_LENGTH_OF_B) {
            throw overflow_error("Passed B exceed maximum number of B");
        }
        
        auto b = Bajtk(stoi(bString));
        Unit units = convertToUnit(b);
        
        if (!unitString.empty()) { //matched find
            unitString.erase(0, 1); //delete comma
            
            while (unitString.size() != MAX_STRING_FRACTIONAL_LENGTH_OF_B) //add ended zeros
                unitString.push_back('0');
            
            units += Unit(stoi(unitString));
        }
        
        createAndAddToBalance(units);
    }
    else {
        throw invalid_argument("String is not accepted");
    }
}

Wallet Wallet::fromBinary(std::string str) {
    return Wallet(convertToBajtk(str));
}

Wallet::Wallet(Wallet &&other) noexcept : balance(other.balance), history(move(other.history)) {
    other.balance = 0;
}

Wallet::Wallet(Wallet &&w1, Wallet &&w2) : balance(0), history(move(w1.history)) {
    move(w2.history.begin(), w2.history.end(), back_inserter(history));
    w2.history.erase(w2.history.begin(), w2.history.end());
    sort(history.begin(), history.end());
    addToBalance(w1.balance + w2.balance);
    w1.balance = 0;
    w2.balance = 0;
}

Wallet::Unit Wallet::getUnits() const {
    return balance;
}

size_t Wallet::opSize() const {
    return history.size();
}

ostream &operator<<(ostream &out, const Wallet &w) {
    out << "Wallet[" << w.balanceToBString() << " B]" << endl;
    return out;
}

const Wallet::Operation &Wallet::operator[](size_t k) {
    return history[k];
}

void Wallet::addToBalance(Wallet::Unit unit) {
    balance += unit;
    Operation op(balance);
    history.push_back(op);
}

void Wallet::createAndAddToBalance(Wallet::Unit unit) {
    checkBajtkOverflow(unit);
    addToBalance(unit);
    numberOfExistingUnit += unit;
}

std::string Wallet::balanceToBString() const {
    auto b = Bajtk(balance / Wallet::unitInBajtk);
    auto fractionalPart = balance % Wallet::unitInBajtk;
    string ret = to_string(b);
    if (fractionalPart != 0) {
        string fractionalString = to_string(fractionalPart);
        size_t found = fractionalString.find_last_of("123456789");
        fractionalString = fractionalString.substr(0, found + 1);
        ret += "," + fractionalString;
    }
    return ret;
}

Wallet::Unit Wallet::convertToUnit(Wallet::Bajtk b) {
    if (b > Wallet::maxNumberOfBajtk) {
        throw overflow_error("Passed B exceed maximum number of B");
    }
    return b * Wallet::unitInBajtk;
}

Wallet::Bajtk Wallet::convertToBajtk(const std::string &str) {
    smatch matchResult;
    regex expr("^0*"
               "(0|1[01]*)$");
    if (regex_search(str, matchResult, expr)) {
        auto itr = matchResult.begin();
        string BinaryBString = itr[1].str();
        
        if (BinaryBString.size() > MAX_BINARY_STRING_LENGTH_OF_B) {
            throw overflow_error("Passed B exceed maximum number of B");
        }
        
        return Bajtk(stoi(BinaryBString, nullptr, 2));
    }
    else {
        throw invalid_argument("String is not accepted");
    }
}

void Wallet::checkBajtkOverflow(Wallet::Unit unit) {
    if (unit + Wallet::numberOfExistingUnit > Wallet::maxNumberOfUnit) {
        throw overflow_error("Exceed number of units");
    }
}

Wallet::Operation::Operation(Unit currentBalance) : balanceAfterOperation(currentBalance) {
    auto now = chrono::system_clock::now();
    time = chrono::time_point_cast<chrono::milliseconds>(now); //cat precision to milliseconds
}

Wallet::Unit Wallet::Operation::getUnits() {
    return balanceAfterOperation;
}

bool Wallet::Operation::operator<(const Wallet::Operation &rhs) const {
    return time < rhs.time;
}

std::ostream &operator<<(std::ostream &out, const Wallet::Operation &w) {
    return out;
}

const Wallet &Empty() {
    const static Wallet ref = Wallet();
    return ref;
}

Wallet& Wallet::operator=(Wallet &&rhs) noexcept {
    if (this == &rhs) {
        return *this;
    }
    
    *this = Wallet(std::move(rhs));
    Operation op(balance);
    history.push_back(op);
    
    return *this;
}

Wallet& Wallet::operator-=(Wallet &rhs) {
    this->balance -= rhs.balance;
    rhs.addToBalance(rhs.balance);
    numberOfExistingUnit += rhs.balance;
    Operation op(this->balance);
    history.push_back(op);
}

const Wallet Wallet::operator-(Wallet &other) {
    Wallet result = std::move(*this);
    result -= other;
    return result;
}

Wallet& Wallet::operator+=(Wallet &rhs) {
    this->balance += rhs.balance;
    rhs.balance = 0;
    Operation op (0);
    rhs.history.push_back(op);

}

const Wallet Wallet::operator+(Wallet &other) {
    Wallet result = std::move(*this);
    result += other;
    return result;
}

Wallet& Wallet::operator*=(int n) {
    Unit balance = this->getUnits();
    Unit newCoins = balance * (n - 1);
    this->addToBalance(newCoins);
    numberOfExistingUnit += newCoins;
    // Sprawdzenie, czy nie przekraczamy limitu monet.
    return *this;
}

const Wallet Wallet::operator*(int n) {
    Wallet result = std::move(*this);
    result *= n;
    return result;
}

bool Wallet::operator<(const Wallet &rhs) {
    return this->getUnits() < rhs.getUnits();
}

bool Wallet::operator==(const Wallet &rhs) {
    return this->getUnits() == rhs.getUnits();
}







