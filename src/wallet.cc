#include "wallet.h"
#include <stdexcept>
#include <regex>
#include <iostream>
#include "wallet.h"


using namespace std;
using namespace std::rel_ops;

namespace {
    constexpr int MAX_STRING_LENGTH_OF_B = 8;
    constexpr int MAX_STRING_FRACTIONAL_LENGTH_OF_B = 8;
    constexpr int MAX_BINARY_STRING_LENGTH_OF_B = 24;
}

Wallet::Unit Wallet::numberOfExistingUnit = 0;

Wallet::Wallet() : balance(0), history() {
    createAndAddToBalance(0);
}

Wallet::Wallet(Wallet::Bajtk b) : balance(0), history() {
    Unit units = convertToUnit(b);
    createAndAddToBalance(units);
}

Wallet::Wallet(const std::string &str) : balance(), history() {
    initFromString(str);
}

Wallet::Wallet(const char *str) : balance(), history()  {
    initFromString(str);
}

void Wallet::initFromString(const std::string &str) {
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

Wallet::Wallet(Wallet &&other) noexcept : balance(), history(move(other.history)) {
    addToBalance(other.balance);
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
    out << "Wallet[" << Wallet::toString(w.balance) << " B]";
    return out;
}

const Wallet::Operation &Wallet::operator[](size_t k) const {
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

std::string Wallet::toString(Wallet::Unit balance) {
    auto b = Wallet::Bajtk(balance / Wallet::unitInBajtk);
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

Wallet::Unit Wallet::Operation::getUnits() const{
    return balanceAfterOperation;
}

bool Wallet::Operation::operator<(const Wallet::Operation &rhs) const {
    return time < rhs.time;
}

bool Wallet::Operation::operator==(const Wallet::Operation &rhs) const {
    return time == rhs.time;
}

std::ostream &operator<<(std::ostream &out, const Wallet::Operation &w) {
    string balanceString = Wallet::toString(w.balanceAfterOperation);
    string dateString = w.timeToDate();
    
    out << "Wallet balance is " + balanceString+ " B after operation made at day " + dateString << endl;
    
    return out;
}

std::string Wallet::Operation::timeToDate() const {
    time_t t = chrono::system_clock::to_time_t(time);
    auto timeStructure = *localtime(&t);
    return to_string(timeStructure.tm_year + 1900) + "-"
    + to_string(timeStructure.tm_mon + 1) + "-"
    + to_string(timeStructure.tm_mday);
}

const Wallet &Empty() {
    const static Wallet empty = Wallet();
    return empty;
}

Wallet& Wallet::operator=(Wallet &&rhs) noexcept {
    if (this == &rhs) {
        return *this;
    }
    balance = rhs.balance;
    rhs.balance = 0;
    history = std::move(rhs.history);
    addToBalance(0);
    
    return *this;
}

Wallet& Wallet::operator-=(Wallet &rhs) {
    balance -= rhs.balance;
    rhs.addToBalance(rhs.balance);
    numberOfExistingUnit += rhs.balance;
    addToBalance(0);
    return *this;
}

Wallet& Wallet::operator-=(Wallet &&rhs) {
    *this -= rhs;
    return *this;
}

Wallet&& operator-(Wallet &&lhs, Wallet &rhs) {
    lhs -= rhs;
    return std::move(rhs);
}

Wallet&& operator-(Wallet &&lhs, Wallet &&rhs) {
    lhs -= rhs;
    return std::move(rhs);
}

Wallet& Wallet::operator+=(Wallet &rhs) {
    balance += rhs.balance;
    addToBalance(0);
    rhs.balance = 0;
    rhs.addToBalance(0);
}

Wallet& Wallet::operator+=(Wallet &&rhs) {
    *this += rhs;
    return *this;
}

Wallet&& operator+(Wallet &&lhs, Wallet &rhs) {
    lhs += rhs;
    return(std::move(lhs));
}

Wallet&& operator+(Wallet &&lhs, Wallet &&rhs) {
    lhs += rhs;
    return(std::move(lhs));
}

Wallet& Wallet::operator*=(int n) {
    Unit balance = getUnits();
    Unit newCoins = balance * (n - 1);
    addToBalance(newCoins);
    numberOfExistingUnit += newCoins;
    // Sprawdzenie, czy nie przekraczamy limitu monet.
    return *this;
}

Wallet&& operator*(Wallet &lhs, int n) {
    lhs *= n;
    return(std::move(lhs));
}

Wallet&& operator*(Wallet &&lhs, int n) {
    lhs *= n;
    return(std::move(lhs));
}

Wallet&& operator*(int n, Wallet &rhs) {
    rhs *= n;
    return(std::move(rhs));
}

Wallet&& operator*(int n, Wallet &&rhs) {
    rhs *= n;
    return(std::move(rhs));
}

bool operator<(const Wallet &lhs, Wallet &&rhs) {
    return lhs.balance < rhs.balance;
}

bool operator<(const Wallet &&lhs, Wallet &&rhs) {
    return lhs.balance < rhs.balance;
}

bool operator<(const Wallet &lhs, Wallet &rhs) {
    return lhs.balance < rhs.balance;
}

bool operator<(const Wallet &&lhs, Wallet &rhs) {
    return lhs.balance < rhs.balance;
}

bool operator==(const Wallet &&lhs, const Wallet &rhs) {
    return lhs.getUnits() == rhs.getUnits();
}

bool operator==(const Wallet &&lhs, const Wallet &&rhs) {
    return lhs.getUnits() == rhs.getUnits();
}

bool operator==(const Wallet &lhs, const Wallet &rhs) {
    return lhs.getUnits() == rhs.getUnits();
}

bool operator==(const Wallet &lhs, const Wallet &&rhs) {
    return lhs.getUnits() == rhs.getUnits();
}

bool operator==(unsigned int balance, const Wallet &rhs) {
    return balance == rhs.getUnits() * Wallet::unitInBajtk;
}

bool operator==(unsigned int balance, const Wallet &&rhs) {
    return balance == rhs.getUnits() * Wallet::unitInBajtk;
}

bool operator==(const Wallet &lhs, unsigned int balance) {
    return balance == lhs.getUnits() * Wallet::unitInBajtk;
}

bool operator==(const Wallet &&lhs, unsigned int balance) {
    return balance == lhs.getUnits() * Wallet::unitInBajtk;
}



Wallet::~Wallet() {
    numberOfExistingUnit-=balance;
}

//bool Operation::operator<(const Operation op)