#include <cstdint>
#include <iostream>
#include <string>
#include <algorithm>
#include <stdexcept>

using namespace std;

class BigInt
{
    string number;       // Stores the number as a string
    bool isNegative;     // True if number is negative

    // Remove unnecessary leading zeros from the number string
    void removeLeadingZeros()
    { //(1)
        int i = 0;

        while (i < (int)number.length() && number[i] == '0')
        {
            i++;
        }

        number = number.substr(i);

        if (number.empty())
        {
            number = "0";
        }

        if (number == "0")
        {
            isNegative = false;
        }
    }

    // Compare absolute values of two BigInts
    int compareMagnitude(const BigInt &other) const
    { //(2)
        if (number.length() > other.number.length())
            return 1;

        if (number.length() < other.number.length())
            return -1;

        if (number > other.number)
            return 1;

        if (number < other.number)
            return -1;

        return 0;
    }

public:

    // Default constructor
    BigInt()
    {
        number = "0";
        isNegative = false;
    }

    // Constructor from 64-bit integer
    BigInt(int64_t value)
    {
        string valueString = to_string(value);

        if (value < 0)
        {
            isNegative = true;
            number = valueString.substr(1);
        }
        else
        {
            isNegative = false;
            number = valueString;
        }

        removeLeadingZeros();
    }

    // Constructor from string representation
    BigInt(const string& str)
    {
        if (!str.empty() && str[0] == '-')
        {
            isNegative = true;
            number = str.substr(1);
        }
        else
        {
            isNegative = false;
            number = str;
        }

        removeLeadingZeros();
    }

    // Copy constructor
    BigInt(const BigInt& other)
    {
        number = other.number;
        isNegative = other.isNegative;
    }

    // Destructor
    ~BigInt()
    {
    }

    // Assignment operator
    BigInt& operator=(const BigInt& other)
    {
        if (this != &other)
        {
            number = other.number;
            isNegative = other.isNegative;
        }

        return *this;
    }

    // Unary negation operator (-x)
    BigInt operator-() const
    {
        BigInt result = *this;

        if (this->number != "0")
        {
            result.isNegative = !this->isNegative;
        }

        return result;
    }

    // Unary plus operator (+x)
    BigInt operator+() const
    {
        return *this;
    }

    // Addition assignment operator (x += y)
    BigInt& operator+=(const BigInt& other)
    {
        if (this->isNegative == other.isNegative)
        {
            string result = "";
            int carry = 0;

            int i = (int)this->number.length() - 1;
            int j = (int)other.number.length() - 1;

            while (i >= 0 || j >= 0 || carry > 0)
            {
                int sum = carry;

                if (i >= 0)
                    sum += this->number[i--] - '0';

                if (j >= 0)
                    sum += other.number[j--] - '0';

                carry = sum / 10;

                result += (sum % 10) + '0';
            }

            reverse(result.begin(), result.end());

            this->number = result;

            removeLeadingZeros();
        }
        else
        {
            BigInt temp = -other;
            *this -= temp;
        }

        return *this;
    }

    // Subtraction assignment operator (x -= y)
    BigInt& operator-=(const BigInt& other) //(14)
    {
        if (this->isNegative != other.isNegative)
        {
            BigInt temp = other;
            temp.isNegative = !temp.isNegative;

            *this += temp;

            return *this;
        }

        int comparison = compareMagnitude(other);

        if (comparison == 0)
        {
            number = "0";
            isNegative = false;

            return *this;
        }

        bool resultNegative;

        if (comparison > 0)
        {
            resultNegative = this->isNegative;
        }
        else
        {
            resultNegative = !this->isNegative;
        }

        string larger;
        string smaller;

        if (comparison > 0)
        {
            larger = this->number;
            smaller = other.number;
        }
        else
        {
            larger = other.number;
            smaller = this->number;
        }

        string result = "";

        int i = (int)larger.length() - 1;
        int j = (int)smaller.length() - 1;

        int borrow = 0;

        while (i >= 0)
        {
            int digit1 = larger[i] - '0' - borrow;
            int digit2 = 0;

            if (j >= 0)
            {
                digit2 = smaller[j] - '0';
            }

            if (digit1 < digit2)
            {
                digit1 += 10;
                borrow = 1;
            }
            else
            {
                borrow = 0;
            }

            result += (digit1 - digit2) + '0';

            i--;
            j--;
        }

        reverse(result.begin(), result.end());

        number = result;
        isNegative = resultNegative;

        removeLeadingZeros();

        return *this;
    }

    // Multiplication assignment operator (x *= y)
    BigInt& operator*=(const BigInt& other) //(15)
    {
        if (number == "0" || other.number == "0")
        {
            number = "0";
            isNegative = false;

            return *this;
        }

        string result(number.length() + other.number.length(), '0');

        for (int i = (int)number.length() - 1; i >= 0; i--)
        {
            for (int j = (int)other.number.length() - 1; j >= 0; j--)
            {
                int digit1 = number[i] - '0';
                int digit2 = other.number[j] - '0';

                int position = i + j + 1;

                int product =
                    digit1 * digit2 +
                    (result[position] - '0');

                result[position] = (product % 10) + '0';
                result[position - 1] += product / 10;
            }
        }

        int start = 0;

        while (start < (int)result.length() - 1 &&
               result[start] == '0')
        {
            start++;
        }

        number = result.substr(start);

        isNegative = (isNegative != other.isNegative);

        removeLeadingZeros();

        return *this;
    }

    // Division assignment operator (x /= y)
    BigInt& operator/=(const BigInt& other) //(16)
    {
        if (other.number == "0")
        {
            throw runtime_error("Division by zero");
        }

        if (compareMagnitude(other) < 0)
        {
            number = "0";
            isNegative = false;

            return *this;
        }

        bool resultNegative = (isNegative != other.isNegative);

        BigInt dividend(this->number);
        BigInt divisor(other.number);

        string quotient = "";
        BigInt remainder(0);

        for (int i = 0; i < (int)dividend.number.length(); i++)
        {
            if (remainder.number == "0")
            {
                remainder.number = string(1, dividend.number[i]);
            }
            else
            {
                remainder.number += dividend.number[i];
            }

            remainder.removeLeadingZeros();

            int quotientDigit = 0;

            while (remainder.compareMagnitude(divisor) >= 0)
            {
                remainder -= divisor;
                quotientDigit++;
            }

            quotient += char('0' + quotientDigit);
        }

        number = quotient;
        isNegative = resultNegative;

        removeLeadingZeros();

        return *this;
    }

    // Modulus assignment operator (x %= y)
    BigInt& operator%=(const BigInt& other) //(17)
    {
        if (other.number == "0")
        {
            throw runtime_error("Division by zero");
        }

        bool originalSign = isNegative;

        BigInt quotient = *this;
        quotient /= other;

        BigInt product = quotient;
        product *= other;

        BigInt result = *this;
        result -= product;

        number = result.number;

        isNegative = originalSign;

        removeLeadingZeros();

        return *this;
    }

    // Pre-increment operator (++x)
    BigInt& operator++() //(18)
    {
        *this += BigInt(1);

        return *this;
    }

    // Post-increment operator (x++)
    BigInt operator++(int) //(19)
    {
        BigInt temp = *this;

        ++(*this);

        return temp;
    }

    // Pre-decrement operator (--x)
    BigInt& operator--() //(20)
    {
        *this -= BigInt(1);

        return *this;
    }

    // Post-decrement operator (x--) //(21)
    BigInt operator--(int)
    {
        BigInt temp = *this;

        --(*this);

        return temp;
    }

    // Convert BigInt to string representation
    string toString() const //(22)
    {
        if (isNegative && number != "0")
            return "-" + number;

        return number;
    }

    // Output stream operator
    friend ostream& operator<<(ostream& os, const BigInt& num)
    {
        if (num.isNegative && num.number != "0")
        {
            os << "-";
        }

        os << num.number;

        return os;
    }

    // Input stream operator
    friend istream& operator>>(istream& is, BigInt& num)
    {
        string input;

        is >> input;

        if (is)
        {
            num = BigInt(input);
        }

        return is;
    }

    // Friend declarations for comparison operators
    friend bool operator==(const BigInt& lhs, const BigInt& rhs);
    friend bool operator<(const BigInt& lhs, const BigInt& rhs);
};


// Binary addition operator (x + y)
BigInt operator+(BigInt lhs, const BigInt &rhs)
{ //(23)
    lhs += rhs;

    return lhs;
}


// Binary subtraction operator (x - y)
BigInt operator-(BigInt lhs, const BigInt &rhs)
{ //(24)
    lhs -= rhs;

    return lhs;
}


// Binary multiplication operator (x * y)
BigInt operator*(BigInt lhs, const BigInt &rhs)
{ //(25)
    lhs *= rhs;

    return lhs;
}


// Binary division operator (x / y)
BigInt operator/(BigInt lhs, const BigInt &rhs)
{ //(26)
    lhs /= rhs;

    return lhs;
}


// Binary modulus operator (x % y)
BigInt operator%(BigInt lhs, const BigInt &rhs)
{ //(27)
    lhs %= rhs;

    return lhs;
}


// Equality comparison operator (x == y)
bool operator==(const BigInt &lhs, const BigInt &rhs)
{ //(28)
    if (lhs.isNegative != rhs.isNegative)
        return false;

    return lhs.number == rhs.number;
}


// Inequality comparison operator (x != y)
bool operator!=(const BigInt &lhs, const BigInt &rhs)
{ //(29)
    return !(lhs == rhs);
}


// Less-than comparison operator (x < y)
bool operator<(const BigInt &lhs, const BigInt &rhs)
{ //(30)
    if (lhs.isNegative && !rhs.isNegative)
        return true;

    if (!lhs.isNegative && rhs.isNegative)
        return false;

    if (!lhs.isNegative && !rhs.isNegative)
    {
        return lhs.compareMagnitude(rhs) < 0;
    }

    return lhs.compareMagnitude(rhs) > 0;
}


// Less-than-or-equal comparison operator (x <= y)
bool operator<=(const BigInt &lhs, const BigInt &rhs)
{ //(31)
    return (lhs < rhs) || (lhs == rhs);
}


// Greater-than comparison operator (x > y)
bool operator>(const BigInt &lhs, const BigInt &rhs)
{ //(32)
    return !(lhs <= rhs);
}


// Greater-than-or-equal comparison operator (x >= y)
bool operator>=(const BigInt &lhs, const BigInt &rhs)
{ //(33)
    return !(lhs < rhs);
}

int main() {
    cout << "=== BigInt Class Test Program ===" << endl << endl;
    cout << "NOTE: All functions are currently empty." << endl;
    cout << "Your task is to implement ALL the functions above." << endl;
    cout << "The tests below will work once you implement them correctly." << endl << endl;

    /*
    // Test 1: Constructors and basic output
    cout << "1. Constructors and output:" << endl;
    BigInt a(12345);              // Should create BigInt from integer
    BigInt b("-67890");           // Should create BigInt from string
    BigInt c("0");                // Should handle zero correctly
    BigInt d = a;                 // Should use copy constructor
    cout << "a (from int): " << a << endl;        // Should print "12345"
    cout << "b (from string): " << b << endl;     // Should print "-67890"
    cout << "c (zero): " << c << endl;            // Should print "0"
    cout << "d (copy of a): " << d << endl << endl; // Should print "12345"

    // Test 2: Arithmetic operations
    cout << "2. Arithmetic operations:" << endl;
    cout << "a + b = " << a + b << endl;          // Should calculate 12345 + (-67890)
    cout << "a - b = " << a - b << endl;          // Should calculate 12345 - (-67890)
    cout << "a * b = " << a * b << endl;          // Should calculate 12345 * (-67890)
    cout << "b / a = " << b / a << endl;          // Should calculate (-67890) / 12345
    cout << "a % 100 = " << a % BigInt(100) << endl << endl; // Should calculate 12345 % 100

    // Test 3: Relational operators
    cout << "3. Relational operators:" << endl;
    cout << "a == d: " << (a == d) << endl;       // Should be true (12345 == 12345)
    cout << "a != b: " << (a != b) << endl;       // Should be true (12345 != -67890)
    cout << "a < b: " << (a < b) << endl;         // Should be false (12345 < -67890)
    cout << "a > b: " << (a > b) << endl;         // Should be true (12345 > -67890)
    cout << "c == 0: " << (c == BigInt(0)) << endl << endl; // Should be true (0 == 0)

    // Test 4: Unary operators and increments
    cout << "4. Unary operators and increments:" << endl;
    cout << "-a: " << -a << endl;                 // Should print "-12345"
    cout << "++a: " << ++a << endl;               // Should increment and print "12346"
    cout << "a--: " << a-- << endl;               // Should print "12346" then decrement
    cout << "a after decrement: " << a << endl << endl; // Should print "12345"

    // Test 5: Large number operations
    cout << "5. Large number operations:" << endl;
    BigInt num1("12345678901234567890");
    BigInt num2("98765432109876543210");
    cout << "Very large addition: " << num1 + num2 << endl;
    cout << "Very large multiplication: " << num1 * num2 << endl << endl;

    // Test 6: Edge cases and error handling
    cout << "6. Edge cases:" << endl;
    BigInt zero(0);
    BigInt one(1);
    try {
        BigInt result = one / zero;               // Should throw division by zero error
        cout << "Division by zero succeeded (unexpected)" << endl;
    } catch (const runtime_error& e) {
        cout << "Division by zero correctly threw error: " << e.what() << endl;
    }
    cout << "Multiplication by zero: " << one * zero << endl;        // Should be "0"
    cout << "Negative multiplication: " << BigInt(-5) * BigInt(3) << endl;  // Should be "-15"
    cout << "Negative division: " << BigInt(-10) / BigInt(3) << endl;       // Should be "-3"
    cout << "Negative modulus: " << BigInt(-10) % BigInt(3) << endl;        // Should be "-1"
    */

    return 0;
}
