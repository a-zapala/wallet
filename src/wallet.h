#ifndef WALLET_WALLET_H
#define WALLET_WALLET_H

#include <chrono>
#include <vector>
#include <string>

class Wallet {
public:
    using Unit = uint64_t;
    using Bajtk = unsigned int;
    using Time = std::chrono::system_clock::time_point;
    
    class Operation {
    public:
        explicit Operation(Unit currentBalance);
        
        Unit getUnits();
        
        bool operator<(const Operation &rhs) const;
        bool operator==(const Operation &rhs) const;
        
        friend std::ostream &operator<<(std::ostream &out, const Operation &w);
    
    
    private:
        Unit balanceAfterOperation;
        Time time;
    };
private:
    
    
    
    Unit balance;
    std::vector<Operation> history;
    
    static constexpr Unit unitInBajtk = 100000000;
    static constexpr Unit maxNumberOfBajtk = 21000000;
    static constexpr Unit maxNumberOfUnit = maxNumberOfBajtk * unitInBajtk;
    
    static Unit numberOfExistingUnit;
    
    void addToBalance(Unit unit);
    
    void createAndAddToBalance(Unit unit);
    
    std::string balanceToBString() const;
    
    static Unit convertToUnit(Bajtk b);
    
    static Bajtk convertToBajtk(const std::string &str);
    
    static void checkBajtkOverflow(Unit unit);

public:
    
    
    Wallet();
    
    Wallet(Bajtk n);
    
    Wallet(std::string str);
    
    
    
    static Wallet fromBinary(std::string str);
    
    Wallet(const Wallet &other) = delete;
    
    Wallet(Wallet &&other) noexcept;
    
    Wallet(Wallet &&w1, Wallet &&w2);
    
    Unit getUnits() const;
    
    size_t opSize() const;
    
    friend std::ostream &operator<<(std::ostream &out, const Wallet &w);
    
    const Operation &operator[](size_t idx);

    friend bool operator<(const Wallet &lhs, Wallet &&rhs);
    friend bool operator<(const Wallet &lhs, Wallet &rhs);
    friend bool operator<(const Wallet &&lhs, Wallet &&rhs);
    friend bool operator<(const Wallet &&lhs, Wallet &rhs);


    Wallet& operator=(Wallet &&rhs) noexcept;

    Wallet& operator+=(Wallet &rhs);
    Wallet& operator+=(Wallet &&rhs);

    friend Wallet&& operator+(Wallet &&lhs, Wallet&rhs);
    friend Wallet&& operator+(Wallet &&lhs, Wallet&&rhs);


    Wallet& operator*=(int n);
    friend Wallet&& operator*(Wallet &lhs, int n);
    friend Wallet&& operator*(Wallet &&lhs, int n);
    friend Wallet&& operator*(int n, Wallet &lhs);
    friend Wallet&& operator*(int n, Wallet &&lhs);

    //Wallet operator*(int n) const; // TODO dodane const

    Wallet& operator-=(Wallet &rhs);
    Wallet& operator-=(Wallet &&rhs);

    friend Wallet&& operator-(Wallet &&lhs, Wallet&rhs);
    friend Wallet&& operator-(Wallet &&lhs, Wallet&&rhs);

    bool operator<(const Wallet &rhs);

    bool operator==(const Wallet &rhs) const; // TODO dodane const

    friend bool operator==(const Wallet &lhs, const Wallet &rhs);
    friend bool operator==(const Wallet &lhs, const Wallet &&rhs);
    friend bool operator==(const Wallet &&lhs, const Wallet &rhs);
    friend bool operator==(const Wallet &&lhs, const Wallet &&rhs);
    friend bool operator==(const Wallet &lhs, unsigned int balance);
    friend bool operator==(const Wallet &&lhs, unsigned int balance);
    friend bool operator==(const Wallet &lhs, unsigned int balance);
    friend bool operator==(unsigned int balance, const Wallet &rhs);
    friend bool operator==(unsigned int balance, const Wallet &&rhs);
};

const Wallet &Empty();

#endif //WALLET_WALLET_H
