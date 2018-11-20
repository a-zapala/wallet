#ifndef WALLET_WALLET_H
#define WALLET_WALLET_H

#include <chrono>
#include <vector>
#include <string>

class Wallet {
    using Unit = uint64_t;
    using Bajtk = int;
    using Time = std::chrono::system_clock::time_point;
public:
    
    class Operation {
    public:
        explicit Operation(Unit currentBalance);
        
        Unit getUnits() const;
        
        bool operator<(const Operation &rhs) const;
        bool operator==(const Operation &rhs) const;
        bool operator<=(const Operation &rhs) const;
        bool operator>(const Operation &rhs) const;
        bool operator>=(const Operation &rhs) const;
        bool operator!=(const Operation &rhs) const;



        friend std::ostream &operator<<(std::ostream &out, const Operation &w);
    
    
    private:
        Unit balanceAfterOperation;
        Time time;
        
        std::string timeToDate() const;
    };
private:
    Unit balance;
    std::vector<Operation> history;
    
    static constexpr Unit unitInBajtk = 100000000;
    static constexpr Bajtk maxNumberOfBajtk = 21000000;
    static constexpr Unit maxNumberOfUnit = maxNumberOfBajtk * unitInBajtk;
    
    static Unit numberOfExistingUnit;
    
    void addToBalance(Unit unit);
    
    void createAndAddToBalance(Unit unit);
    
    static Unit convertToUnit(Bajtk b);
    
    static Bajtk convertToBajtk(const std::string &str);
    
    static void checkBajtkOverflow(Unit unit);
    
    void initFromString(const std::string &str);

public:
    
    
    Wallet();
    
    Wallet(Bajtk n);
    
    Wallet(const std::string &str);
    
    Wallet(const char *str);
    
    template <typename T>
    Wallet(T t) = delete;
    
    virtual ~Wallet();
    
    static Wallet fromBinary(std::string str);
    
    static std::string toString(Unit balance);
    
    Wallet(const Wallet &other) = delete;
    
    Wallet(Wallet &&other) noexcept;
    
    Wallet(Wallet &&w1, Wallet &&w2);
    
    Unit getUnits() const;
    
    size_t opSize() const;
    
    friend std::ostream &operator<<(std::ostream &out, const Wallet &w);
    
    const Operation &operator[](size_t idx) const;
    

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
    

    Wallet& operator-=(Wallet &rhs);
    Wallet& operator-=(Wallet &&rhs);

    friend Wallet&& operator-(Wallet &&lhs, Wallet&rhs);
    friend Wallet&& operator-(Wallet &&lhs, Wallet&&rhs);

    friend bool operator<(const Wallet &lhs, const Wallet &rhs);
    friend bool operator==(const Wallet &lhs, const Wallet &rhs);
    friend bool operator<=(const Wallet &lhs, const Wallet &rhs);
    friend bool operator!=(const Wallet &lhs, const Wallet &rhs);
    friend bool operator>(const Wallet &lhs, const Wallet &rhs);
    friend bool operator>=(const Wallet &lhs, const Wallet &rhs);
   

};

const Wallet &Empty();

#endif //WALLET_WALLET_H
