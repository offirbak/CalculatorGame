#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <optional>
#include <unordered_map>

constexpr int DigitsInCalculator = 10;
static uint64_t MaxCalculatorIntValue = static_cast<uint64_t>(pow(10, 10) - 1);
constexpr int MaxMoves = 1000;
constexpr bool ImplementTwoRepeatingDigitsRule = true;

enum class GameOutcome { unknown, finished, overflow, tooManyMoves };
static std::vector<GameOutcome> AllGameOutcomes { GameOutcome::finished, GameOutcome::overflow, GameOutcome::tooManyMoves };

double NextStep(const double n);
double NextStepForInt(const double n);
double NextStepForNonInt(const double n);
bool HasTwoRepeatingDigits(const uint64_t num, uint64_t* valueAfterTwoRepeatingDigits);
int main();
void BreakFractionToIntAndFracPart(const double n, uint64_t* inPart, uint64_t* fractionPart, int* fractPartLeadingZerosCount);

struct GameResults {
    GameResults() : startValue(0), outcome(GameOutcome::unknown), stepsCount(0), finalValue(std::nullopt) {}
    GameResults(uint64_t _startValue, GameOutcome _outcome, int _stepsCount, std::optional<double> _finalValue) 
        : startValue(_startValue), outcome(_outcome), stepsCount(_stepsCount), finalValue(_finalValue) {}
    uint64_t startValue = 0;
    GameOutcome outcome = GameOutcome::unknown;
    int stepsCount = 0;
    std::optional<double> finalValue = std::nullopt;

    bool operator==(const GameResults& other) const
    {
        return Equals(other);
    }
    bool operator!=(const GameResults& other) const
    {
        return !Equals(other);
    }
private:
    bool Equals(const GameResults& other) const {
        return startValue == other.startValue
            && outcome == other.outcome
            && stepsCount == other.stepsCount
            && finalValue == other.finalValue;
    }
};

GameResults Play(uint64_t num) {
    GameResults results;
    results.startValue = num;
    double n = static_cast<double>(num);
    for (int moves = 0; moves < MaxMoves; moves++) {
        if (n < 10) {
            results.finalValue = std::optional<double>{ n };
            results.outcome = GameOutcome::finished;
            break;
        }
        if (n > MaxCalculatorIntValue) {
            results.outcome = GameOutcome::overflow;
            break;
        }
        results.stepsCount++;
        n = NextStep(n);
    }
    return results;
}

double NextStep(const double n) {
    double intpart;
    bool isInteger = (modf(n, &intpart) == 0.0);
    return isInteger ? NextStepForInt(n) : NextStepForNonInt(n);
}

double NextStepForInt(const double n) {
    if (n < 10) {
        return n;
    }
    uint64_t num = (uint64_t)n;
    if (ImplementTwoRepeatingDigitsRule) {
        uint64_t valueAfterTwoRepeatingDigits = 0;
        bool hasTwoRepeatingDigits = HasTwoRepeatingDigits(num, &valueAfterTwoRepeatingDigits);
        if (hasTwoRepeatingDigits && valueAfterTwoRepeatingDigits != 0) {
            return n / valueAfterTwoRepeatingDigits;
        }
    }
    uint64_t divideBy = 0;
    uint64_t multiplier = 1;
    while ((divideBy == 0 || divideBy == 1) && (divideBy < n)) {
        int digit = num % 10;
        divideBy += digit * multiplier;
        num /= 10;
        multiplier *= 10;
    }
    return n / divideBy;
}

double NextStepForNonInt(const double n) {
    if (n < 10 || n > MaxCalculatorIntValue) {
        return n;
    }

    uint64_t intPart;
    uint64_t fractionPart;
    int fractPartLeadingZerosCount;
    BreakFractionToIntAndFracPart(n, &intPart, &fractionPart, &fractPartLeadingZerosCount);
    uint64_t multipler = fractionPart;
    uint64_t exponent = 1;
    while (fractPartLeadingZerosCount-- > 0) {
        exponent *= 10;
    }
    while (multipler < 2) {
        exponent *= 10;
        multipler += (exponent * (intPart % 10));
        intPart /= 10;
    }
    return n * multipler;
}

void BreakFractionToIntAndFracPart(const double n, uint64_t* intPart, uint64_t* fractionPart, int *fractPartLeadingZerosCount) {
    double _intPart;
    double _fractionPart = modf(n, &_intPart);
    int intPartDigits = static_cast<int>(_intPart < 10 ? 1 : log10(_intPart) + 1);
    int truncatedFractPartDigits = DigitsInCalculator - intPartDigits;
    int64_t truncatedFractPart = 0;

    int _fractPartLeadingZerosCount = 0;
    bool encounteredNonZeroDigit = false;
    while (truncatedFractPartDigits > 0) {
        _fractionPart *= 10;
        double _fractionPartRound = round(_fractionPart);
        int nextFracDigit;
        if (abs(_fractionPartRound - _fractionPart) < 0.001) {
            // Handle rare cases if examples like 12.12 represented as 12.119999999 due to precision limits of double.
            nextFracDigit = static_cast<int>(_fractionPartRound);
        } else {
            nextFracDigit = static_cast<int>(floor(_fractionPart));
        }

        if (nextFracDigit == 0) {
            if (!encounteredNonZeroDigit) {
                _fractPartLeadingZerosCount++;
            }
        } else {
            encounteredNonZeroDigit = true;
        }

        _fractionPart -= nextFracDigit;
        truncatedFractPart *= 10;
        truncatedFractPart += nextFracDigit;
        if (_fractionPart < 0.001) {
            break;
        }
        truncatedFractPartDigits--;
    }

    *intPart = static_cast<uint64_t>(_intPart);
    *fractionPart = static_cast<uint64_t>(truncatedFractPart);
    *fractPartLeadingZerosCount = _fractPartLeadingZerosCount;
}

bool HasTwoRepeatingDigits(const uint64_t num, uint64_t* valueAfterTwoRepeatingDigits) {
    *valueAfterTwoRepeatingDigits = 0;
    if (num < 1000) {
        return false;
    }
    if (num > MaxCalculatorIntValue) {
        return false;
    }

    uint64_t multipler = 1;
    while (multipler * 10 < num) {
        multipler *= 10;
    }

    uint64_t truncatedNum = num;
    uint64_t digit = 0;
    std::optional<uint64_t> prevDigit = std::nullopt;
    while (multipler > 1) {
        digit = truncatedNum / multipler;
        if (prevDigit && (*prevDigit == digit)) {
            *valueAfterTwoRepeatingDigits = num % multipler;
            return *valueAfterTwoRepeatingDigits > 1;
        }
        else {
            prevDigit = std::optional<uint64_t>{ digit };
        }
        truncatedNum -= (digit * multipler);
        multipler /= 10;
    }
    return false;
}

class OutcomeStats {
public:
    OutcomeStats() : totalRuns_(0), totalSteps_(0) {}

    void AddRun(uint64_t stepsCount) {
        totalRuns_++;
        totalSteps_ += stepsCount;
    }

    uint64_t GetTotalRuns() const { return totalRuns_; }
    double GetAvgStepsPerGame() const { return (1.0 * totalSteps_) / totalRuns_; }
private:
    uint64_t totalRuns_;
    uint64_t totalSteps_;
};

class OverallStats {
public:
    OverallStats();
    void AddRun(GameResults&& results);
    void Print() const;
private:
    static const std::string& ToString(GameOutcome outcome);
private:
    std::unordered_map<GameOutcome, OutcomeStats> stats_;
    uint64_t totalGames_;
};

OverallStats::OverallStats() : totalGames_(0) {
    for (const auto gameOutcome : AllGameOutcomes) {
        stats_[gameOutcome] = OutcomeStats();
    }
}

void OverallStats::AddRun(GameResults&& results) {
    stats_[results.outcome].AddRun(results.stepsCount);
    totalGames_++;
}

void OverallStats::Print() const {
    for (const auto gameOutcome : AllGameOutcomes) {
        const OutcomeStats& stats = stats_.at(gameOutcome);
        if (stats.GetTotalRuns() == 0) {
            continue;
        }
        std::cout << "Outcome " << ToString(gameOutcome)
            << " occured " << 100.0 * stats.GetTotalRuns() / totalGames_ << "% of times"
            << " and finished in " << stats.GetAvgStepsPerGame() << " of steps in average"
            << std::endl;        
    }
}

const std::string& OverallStats::ToString(GameOutcome outcome) {
    static const std::string sFinished = "finished";
    static const std::string sOverflow = "overflow";
    static const std::string sTooManyMoves = "tooManyMoves";
    switch (outcome) {
        case GameOutcome::finished: return sFinished;
        case GameOutcome::overflow: return sOverflow;
        case GameOutcome::tooManyMoves: return sTooManyMoves;
        default: throw "Unhandled GameOutcome";
    }
}

namespace tests {
    void RunTests();
}

int main()
{
    tests::RunTests();

    uint64_t n;
    for (n = MaxCalculatorIntValue; n > 10; n--) {
        if (Play(n).outcome == GameOutcome::finished) {
            std::cout << "Largest winning number: " << n << std::endl;
            break;
        }
    }
        
    OverallStats stats;
    for (uint64_t n = 10; n <= 9999; n++) {
        stats.AddRun(Play(n));
    }
    stats.Print();
    return 0;
}

// tests //////////////////////////////////////////////////////////////////////////////////////////

namespace tests {
    std::string testName;

    void RunTests();

    void asserts(const uint64_t valueTested, bool actual0, uint64_t actual1, uint64_t expected1) {
        if (!actual0) {
            std::cout << "Test failure " << testName << " valueTested=" << valueTested << " bool condition failed" << std::endl;
        }
        if (actual1 != expected1) {
            std::cout << "Test failure " << testName << " valueTested=" << valueTested << " expected1=" << expected1 << " actual1=" << actual1 << std::endl;
        }
    }

    void asserts(const double valueTested, const double expected0, const double actual0) {
        if (actual0 != expected0) {
            std::cout << "Test failure " << testName << " valueTested=" << valueTested << " expected0=" << expected0 << " actual0=" << actual0 << std::endl;
        }
    }

    void asserts(const double valueTested, const uint64_t expected0, const uint64_t actual0, const uint64_t expected1, const uint64_t actual1) {
        if (actual0 != expected0) {
            std::cout << "Test failure " << testName << " valueTested=" << valueTested << " expected0=" << expected0 << " actual0=" << actual0 << std::endl;
            return;
        }
        if (actual1 != expected1) {
            std::cout << "Test failure " << testName << " valueTested=" << valueTested << " expected1=" << expected1 << " actual1=" << actual1 << std::endl;
            return;
        }
    }

    void asserts(const uint64_t valueTested, const GameResults& expected, const GameResults& actual) {
        if (expected != actual) {
            std::cout << "Test failure " << testName << " valueTested=" << valueTested << std::endl;
        }
    }

    void TestHasTwoRepeatingDigits() {
        testName = "TestHasTwoRepeatingDigits";
        uint64_t valueAfterTwoRepeatingDigits;
        bool res;
        uint64_t valueTested;

        res = HasTwoRepeatingDigits(valueTested = 0, &valueAfterTwoRepeatingDigits);
        asserts(valueTested, !res, valueAfterTwoRepeatingDigits, 0);

        res = HasTwoRepeatingDigits(valueTested = 5, &valueAfterTwoRepeatingDigits);
        asserts(valueTested, !res, valueAfterTwoRepeatingDigits, 0);

        res = HasTwoRepeatingDigits(valueTested = 75, &valueAfterTwoRepeatingDigits);
        asserts(valueTested, !res, valueAfterTwoRepeatingDigits, 0);

        res = HasTwoRepeatingDigits(valueTested = 375, &valueAfterTwoRepeatingDigits);
        asserts(valueTested, !res, valueAfterTwoRepeatingDigits, 0);

        res = HasTwoRepeatingDigits(valueTested = 3759, &valueAfterTwoRepeatingDigits);
        asserts(valueTested, !res, valueAfterTwoRepeatingDigits, 0);

        res = HasTwoRepeatingDigits(valueTested = 37590, &valueAfterTwoRepeatingDigits);
        asserts(valueTested, !res, valueAfterTwoRepeatingDigits, 0);

        res = HasTwoRepeatingDigits(valueTested = 3759001, &valueAfterTwoRepeatingDigits);
        asserts(valueTested, !res, valueAfterTwoRepeatingDigits, 1);

        res = HasTwoRepeatingDigits(valueTested = 375900, &valueAfterTwoRepeatingDigits);
        asserts(valueTested, !res, valueAfterTwoRepeatingDigits, 0);

        res = HasTwoRepeatingDigits(valueTested = 37005977, &valueAfterTwoRepeatingDigits);
        asserts(valueTested, res, valueAfterTwoRepeatingDigits, 5977);

        res = HasTwoRepeatingDigits(valueTested = 37005900, &valueAfterTwoRepeatingDigits);
        asserts(valueTested, res, valueAfterTwoRepeatingDigits, 5900);

        res = HasTwoRepeatingDigits(valueTested = 375911, &valueAfterTwoRepeatingDigits);
        asserts(valueTested, !res, valueAfterTwoRepeatingDigits, 0);

        res = HasTwoRepeatingDigits(valueTested = 3759118, &valueAfterTwoRepeatingDigits);
        asserts(valueTested, res, valueAfterTwoRepeatingDigits, 8);

        res = HasTwoRepeatingDigits(valueTested = 37591187, &valueAfterTwoRepeatingDigits);
        asserts(valueTested, res, valueAfterTwoRepeatingDigits, 87);

        res = HasTwoRepeatingDigits(valueTested = 375911873, &valueAfterTwoRepeatingDigits);
        asserts(valueTested, res, valueAfterTwoRepeatingDigits, 873);
    }

    void TestNextStepForInt() {
        testName = "TestNextStepForInt";

        double valueTested = 0;
        double res = 0;

        res = NextStepForInt(valueTested = 0);
        asserts(valueTested, 0, res);

        res = NextStepForInt(valueTested = 7);
        asserts(valueTested, 7, res);

        res = NextStepForInt(valueTested = 79);
        asserts(valueTested, 79.0 / 9, res);

        res = NextStepForInt(valueTested = 70);
        asserts(valueTested, 1, res);

        res = NextStepForInt(valueTested = 71);
        asserts(valueTested, 1, res);

        res = NextStepForInt(valueTested = 11);
        asserts(valueTested, 1, res);

        res = NextStepForInt(valueTested = 77);
        asserts(valueTested, 11, res);

        res = NextStepForInt(valueTested = 771);
        asserts(valueTested, valueTested / 71, res);

        res = NextStepForInt(valueTested = 7710);
        asserts(valueTested, valueTested / 10, res);

        res = NextStepForInt(valueTested = 77100);
        asserts(valueTested, valueTested / 100, res);

        res = NextStepForInt(valueTested = 77105);
        asserts(valueTested, valueTested / 105, res);

        res = NextStepForInt(valueTested = 177105);
        asserts(valueTested, valueTested / 105, res);

        res = NextStepForInt(valueTested = 13775);
        asserts(valueTested, valueTested / 5, res);

        res = NextStepForInt(valueTested = 771052);
        asserts(valueTested, valueTested / 1052, res);

        res = NextStepForInt(valueTested = 761052);
        asserts(valueTested, valueTested / 2, res);

        res = NextStepForInt(valueTested = 32771052);
        asserts(valueTested, valueTested / 1052, res);

        res = NextStepForInt(valueTested = 42001052);
        asserts(valueTested, valueTested / 1052, res);

        res = NextStepForInt(valueTested = 5277105200);
        asserts(valueTested, valueTested / 105200, res);

        res = NextStepForInt(valueTested = 6276105200);
        asserts(valueTested, valueTested / 200, res);

        res = NextStepForInt(valueTested = 7276105201);
        asserts(valueTested, valueTested / 201, res);

        res = NextStepForInt(valueTested = 8276105203);
        asserts(valueTested, valueTested / 3, res);

        res = NextStepForInt(valueTested = 9276105211);
        asserts(valueTested, valueTested / 11, res);
    }

    void TestBreakFractionToIntAndFracPart() {
        testName = "TestBreakFractionToIntAndFracPart";

        uint64_t inPart;
        uint64_t fractPart;
        int fractPartLeadingZerosCount;
        double valueTested;

        BreakFractionToIntAndFracPart(valueTested = 0, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 0, inPart, 0, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 7, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 7, inPart, 0, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 1.1, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 1, inPart, 1, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 1.12, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 1, inPart, 12, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 12.12, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 12, inPart, 12, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 123.123, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 123, inPart, 123, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 123.03, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 123, inPart, 3, fractPart);
        asserts(valueTested, 1, fractPartLeadingZerosCount);

        BreakFractionToIntAndFracPart(valueTested = 123.103, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 123, inPart, 103, fractPart);
        asserts(valueTested, 0, fractPartLeadingZerosCount);

        BreakFractionToIntAndFracPart(valueTested = 123.003, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 123, inPart, 3, fractPart);
        asserts(valueTested, 2, fractPartLeadingZerosCount);

        BreakFractionToIntAndFracPart(valueTested = 123.1003, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 123, inPart, 1003, fractPart);
        asserts(valueTested, 0, fractPartLeadingZerosCount);

        BreakFractionToIntAndFracPart(valueTested = 123.0003, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 123, inPart, 3, fractPart);
        asserts(valueTested, 3, fractPartLeadingZerosCount);

        BreakFractionToIntAndFracPart(valueTested = 1234.1234, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 1234, inPart, 1234, fractPart);
        asserts(valueTested, 0, fractPartLeadingZerosCount);

        BreakFractionToIntAndFracPart(valueTested = 12345.12345, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 12345, inPart, 12345, fractPart);
        asserts(valueTested, 0, fractPartLeadingZerosCount);

        BreakFractionToIntAndFracPart(valueTested = 223456.123444, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 223456, inPart, 1234, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 323456.123456, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 323456, inPart, 1234, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 4234567.1234567, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 4234567, inPart, 123, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 5234567.1239567, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 5234567, inPart, 123, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 1.123456789, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 1, inPart, 123456789, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 12.123456789, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 12, inPart, 12345678, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 123.123456789, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 123, inPart, 1234567, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 1234.123456789, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 1234, inPart, 123456, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 12345.123456789, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 12345, inPart, 12345, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 123456.123456789, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 123456, inPart, 1234, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 1234567.123456789, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 1234567, inPart, 123, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 1234567.1, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 1234567, inPart, 1, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 1234567.12, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 1234567, inPart, 12, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 1234567.123, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 1234567, inPart, 123, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 1234567.1234, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 1234567, inPart, 123, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 1234567.12345, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 1234567, inPart, 123, fractPart);

        BreakFractionToIntAndFracPart(valueTested = 1234567.123456, &inPart, &fractPart, &fractPartLeadingZerosCount);
        asserts(valueTested, 1234567, inPart, 123, fractPart);
    }

    void TestNextStepForNonInt() {
        testName = "TestNextStepForNonInt";

        double testedValue;
        double nextValue;

        nextValue = NextStepForNonInt(testedValue = 2.5);
        asserts(testedValue, 2.5, nextValue);

        nextValue = NextStepForNonInt(testedValue = 12.5);
        asserts(testedValue, 12.5 * 5, nextValue);

        nextValue = NextStepForNonInt(testedValue = 123.987);
        asserts(testedValue, 123.987 * 987, nextValue);

        nextValue = NextStepForNonInt(testedValue = 123.1);
        asserts(testedValue, 123.1 * 31, nextValue);

        nextValue = NextStepForNonInt(testedValue = 123.001);
        asserts(testedValue, 123.001 * 3001, nextValue);

        nextValue = NextStepForNonInt(testedValue = 120.001);
        asserts(testedValue, 120.001 * 20001, nextValue);
    }

    void TestPlay() {
        testName = "TestPlay";
        uint64_t valueTested;
        GameResults results;
        double expectedFinalValue;

        results = Play(valueTested = 9922);
        expectedFinalValue = (9922.0 / 22) / 51;
        asserts(valueTested, GameResults{ valueTested, GameOutcome::finished, 2, std::optional<double> { expectedFinalValue } }, results);

        results = Play(valueTested = 9122);
        asserts(valueTested, GameResults{ valueTested, GameOutcome::overflow, 4, std::nullopt }, results);
    }

    void RunTests() {
        TestHasTwoRepeatingDigits();
        TestNextStepForInt();
        TestBreakFractionToIntAndFracPart();
        TestNextStepForNonInt();
        TestPlay();
    }
}


