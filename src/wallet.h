#ifndef WALLET_WALLET_H
#define WALLET_WALLET_H

#include <chrono>
#include <vector>
#include <string>

class Wallet {
public:
    using Unit = uint64_t;
    using Bajtk = int;
    using Time = std::chrono::system_clock::time_point;
    
    class Operation {
    public:
        explicit Operation(Unit currentBalance);
        
        Unit getUnits();
        
        bool operator<(const Operation &rhs) const;
        
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

    Wallet& operator=(Wallet &&rhs) noexcept;

    Wallet& operator+=(Wallet &rhs);

    const Wallet operator+(Wallet &other);

    Wallet& operator*=(int n);

    const Wallet operator*(int n);

    Wallet& operator-=(Wallet &rhs);

    const Wallet operator-(Wallet &other);

    bool operator<(const Wallet &rhs);

    bool operator==(const Wallet &rhs);
};

const Wallet &Empty();

#endif //WALLET_WALLET_H
