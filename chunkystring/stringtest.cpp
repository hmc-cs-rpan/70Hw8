/**
 * \file stringtest.cpp
 * \author CS 70 SAMPLE SOLUTION - DO NOT SHARE OR COPY.
 *
 * \brief Tests a ChunkyString for correctness.
 *
 * \details
 *    Configured to use the ChunkyString found in chunkystring.hpp, as
 *    well as avoid testing insert/erase requiring functions. Change
 *    the #defines to alter this behavior.
 */

#include <gtest/gtest.h>
#ifndef LOAD_GENERIC_STRING
/* This test code can be used two ways.  It can either test a local
 * ChunkyString implementation, or it can dynamically load an implementation
 * at runtime and test that.  In the first homework, you'll load an
 * implementation that someone else used, but in later assignments, you'll
 * set the value to zero to use your own.
 */
#define LOAD_GENERIC_STRING 0       // 0 = Normal, 1 = Load Code Dynamically
#endif

#define INSERT_ERASE 0 // 0 = Do not test. 1 = Do test.


#if LOAD_GENERIC_STRING
// #include "string-wrapper.hpp"       // Use dynamic loading magic!
typedef GenericString TestingString;
#else
#include "chunkystring.hpp"         // Just include and link as normal.
typedef ChunkyString TestingString;
#endif

#include <string>
#include <sstream>
#include <stdexcept>
#include <cstddef>
#include <cstdlib>
#include <cassert>
#include <cmath>

#include "signal.h"
#include "unistd.h"

using namespace std;

static size_t CHUNKSIZE;     // This is a "constant", but the test code doesn't
                             // know what it is until after startup.

//--------------------------------------------------
//           HELPER FUNCTIONS
//--------------------------------------------------

enum randomness_t  { MIN_VALUE, MAX_VALUE, MID_VALUE, RANDOM_VALUE };

/**
 * Returns a value n, where 0 < n < maximum.  Depending on the choice
 * parameter, n will either be at the midpoint, one of the extreme ends
 * or at a random position.
 */
static int maybeRandomInt(int maximum, randomness_t choice)
{
    switch (choice) {
    case MIN_VALUE:
        return 0;

    case MAX_VALUE:
        return maximum;

    case MID_VALUE:
        return (maximum + 1) / 2;

    case RANDOM_VALUE:
    default:
        // We take rather unnecessary care here to avoid skewing the
        // distribution.  Only really necessary if maximum is very
        // large.
        ++maximum;
        long divisor = RAND_MAX / maximum;
        int candidate;

        do {
            candidate = random() / divisor;
        } while (candidate >= maximum);

        return candidate;
    }
}

/// Returns a random character, using the maybeRandomInt function.
static char randomChar()
{
    return maybeRandomInt(127, RANDOM_VALUE);
}

/**
 * \brief Converts any type that operator << can write out into a string.
 *
 * \param thing         thing to convert
 * \returns             string representation of thing
 * \throws std::invalid_argument  if conversion fails
 */
template <typename T>
std::string stringFrom (const T& thing)
{
    std::stringstream stream;
    stream << thing;

    if (!stream.fail())
        return stream.str();
    else
        throw std::invalid_argument("Conversion to string failed");
}


/**
 * \brief Make sure that equivalent TestingStrings are not actually using
 *        the same data (i.e. not a shallow copy)
 *
 * \param test          TestingString that will be modified in this test
 * \param copy          TestingString that will remain constant in this test
 * \param origin        Description of the calling function, to assist the
 *                      user in tracing back an error.
 *
 * \remark  The first input will be modified during the test, but is returned
 *          to its original value before the test returns.
 */
void checkDeepCopy(TestingString& test, const TestingString& copy,
                   string origin)
{
    // Don't deal with empty strings
    if (test.size() == 0) return;

    string backtrace = "Backtrace: " + origin;
    // Select random character to change
    size_t pos = maybeRandomInt(test.size() - 1, RANDOM_VALUE);
    TestingString::iterator tIter = test.begin();
    std::advance(tIter, pos);
    TestingString::const_iterator cIter = copy.begin();
    std::advance(cIter, pos);

    // Select a random character that is not the same as the existing one
    char c;

    do {
        c = randomChar();
    } while (c == *tIter);

    // Modify test string and see that copy is unchanged
    EXPECT_TRUE(test == copy) << backtrace;
    EXPECT_EQ(*cIter, *tIter) << backtrace;
    *tIter = c;
    EXPECT_NE(c, *cIter) << backtrace;
    EXPECT_NE(*cIter, *tIter) << backtrace;
    EXPECT_TRUE(test != copy) << backtrace;

    // Return them to equality
    *tIter = *cIter;
    EXPECT_TRUE(test == copy) << backtrace;
}

/**
 * \brief Compare the TestingString to an expected value.
 *
 * \details Tests size, operator<<, begin, end, forward iteration, and
 *          edge case comparisons.
 *
 * \param test          TestingString to check
 * \param control       Expected value of the TestingString
 * \param origin        Description of the calling function, to assist the
 *                      user in tracing back an error.
 */
void checkWithControl(const TestingString& test, const string& control,
                      string origin)
{
    string backtrace = "Backtrace: " + origin;
    // Make sure controls and tests match
    EXPECT_EQ(control.size(), test.size()) << backtrace;
    ASSERT_EQ(control.size(), stringFrom(control).size()) << backtrace;
    ASSERT_EQ(stringFrom(control), stringFrom(test)) << backtrace;
    ASSERT_EQ(control, stringFrom(test)) << backtrace;

    TestingString::const_iterator tIter = test.begin();
    string::const_iterator cIter = control.begin();

    // Walk forward
    for ( ; cIter != control.end(); ++cIter, ++tIter) {
        ASSERT_EQ(*cIter, *tIter) << backtrace;
    }

    EXPECT_TRUE(tIter == test.end()) << backtrace;

    // Walk backward
    for ( ; cIter != control.begin(); ) {
        --cIter;
        --tIter;
        ASSERT_EQ(*cIter, *tIter) << backtrace;
    }

    EXPECT_TRUE(tIter == test.begin()) << backtrace;

    // Test edge case characters
    TestingString chunkyChr0;
    string      checkChr0;
    chunkyChr0.push_back(0); // Null character
    checkChr0.push_back(0);

    EXPECT_EQ((control     == checkChr0), (test     == chunkyChr0)  )
            << backtrace;
    EXPECT_EQ((checkChr0 == control), (chunkyChr0 == test)      )
            << backtrace;
    EXPECT_EQ((control     <  checkChr0), (test     <  chunkyChr0)  )
            << backtrace;
    EXPECT_EQ((checkChr0 <  control), (chunkyChr0 <  test)      )
            << backtrace;

    TestingString chunkyChr127;
    string      checkChr127;
    chunkyChr127.push_back(127); // Delete character
    checkChr127.push_back(127);

    EXPECT_EQ((control       == checkChr127), (test       == chunkyChr127)  )
            << backtrace;
    EXPECT_EQ((checkChr127 == control), (chunkyChr127 == test)        )
            << backtrace;
    EXPECT_EQ((control       <  checkChr127), (test       <  chunkyChr127)  )
            << backtrace;
    EXPECT_EQ((checkChr127 <  control), (chunkyChr127 <  test)        )
            << backtrace;
}


/**
 * \brief Compare two TestingStrings to an expected values and each other.
 *
 * \details Tests operator==, operator!=, operator<. and all functions
 *          tested by checkWithControl(test, control).
 *
 * \param first         TestingString to check
 * \param second        TestingString to check
 * \param fControl      Expected value of 'first'
 * \param sControl      Expected value of 'second'
 * \param origin        Description of the calling function, to assist the
 *                      user in tracing back an error.
 */
void checkTwoWithControl(
            const TestingString& first, const TestingString& second,
            const string& fControl,     const string& sControl,
            string origin)
{
    string backtrace = "Backtrace: " + origin;

    // Check validity of each TestingString
    checkWithControl(first, fControl, "first : " + origin);
    checkWithControl(second, sControl, "second : " + origin);

    // Test equality and inequality
    EXPECT_EQ(fControl == sControl, first == second) << backtrace;
    EXPECT_EQ(sControl == fControl, second == first) << backtrace;
    EXPECT_EQ(fControl != sControl, first != second) << backtrace;
    EXPECT_EQ(sControl != fControl, second != first) << backtrace;
    EXPECT_EQ(fControl < sControl, first < second) << backtrace;
    EXPECT_EQ(sControl < fControl, second < first) << backtrace;
}

/// Compares two TestingStrings by redirecting to
/// checkWithControl(const TestingString&, const TestingString&,
///             const string&, const string&, string).
void checkBothIdentical(const TestingString& first, const TestingString& second,
                        string origin)
{
    checkTwoWithControl(first, second,
                        stringFrom(first), stringFrom(second), origin);
}

/**
 * \brief Compare a TestingString and a TestingString iterator with expected
 *        values.
 *
 * \details Tests iterator correctness and all functions tested by
 *          checkWithControl(test, control).
 *
 * \param test          TestingString to check
 * \param control       Expected value of test
 * \param tIter         TestingString iterator to check
 * \param cIter         Expected value of tIter
 * \param origin        Description of the calling function, to assist the
 *                      user in tracing back an error.
 */
void checkIterWithControl(const TestingString& test, const string& control,
                          const TestingString::const_iterator& tIter,
                          const string::iterator& cIter, string origin)
{
    string backtrace = "Backtrace: " + origin;
    EXPECT_EQ(cIter == control.begin(), tIter == test.begin()) << backtrace;
    EXPECT_EQ(cIter == control.end(), tIter == test.end()) << backtrace;

    if (tIter != test.end())
        EXPECT_EQ(*cIter, *tIter) << backtrace;

    // Check this AFTER iterator checks, so iterators are not invalidated
    checkWithControl(test, control, origin);
}

/**
 * \brief Assuming chunks are supposed to be at least 1/divisor full, checks
 *        for the lowest allowable utilization for the input string
 *
 * \remarks For insertion only, we assume chunks are at least 1/2 full.
 *          For erase, chunks can go down to 1/4 full.
 *
 * \param test          TestingString to check
 * \param divisor       Fullness of chunk = 1/divisor
 * \param origin        String to describe the caller of this function to
 *                      aid in human debugging.
 */
void checkUtilization(const TestingString& test, size_t divisor, string origin)
{
    string backtrace = "Backtrace: " + origin;

    if (test.size() == 0)
        return;

    size_t chunks = 1;
    size_t size = test.size() - 1;
    divisor = CHUNKSIZE / divisor;
    chunks += (size + divisor - 1) / divisor;
    EXPECT_GT(test.utilization(), double(size) / double(chunks * CHUNKSIZE))
            << origin;
}


//--------------------------------------------------
//           TEST FUNCTIONS
//--------------------------------------------------

/// Check all known conditions of a single default constructed object
TEST(constructors, defaultConstructor)
{
    TestingString test;
    string control;

    checkWithControl(test, control, "empty strings");
    EXPECT_TRUE(test.begin() == test.end());
}

/// Test the copy constructor
TEST(constructors, copyConstructor)
{
    // Copy an empty TestingString
    TestingString original;
    TestingString empty(original);
    checkBothIdentical(original, empty, "empty");

    // Copy a small TestingString
    original.push_back('e');
    TestingString tiny(original);
    checkBothIdentical(original, tiny, "tiny");
    checkDeepCopy(original, tiny, "tiny");

    // Make sure previous copy is not changed
    TestingString secondEmpty;
    checkBothIdentical(empty, secondEmpty, "secondEmpty");
    EXPECT_TRUE(original != empty);


    // Copy a larger string
    for (char alpha = 'A'; alpha < 'z'; ++alpha) {
        original.push_back(alpha);
    }

    TestingString large(original);
    checkBothIdentical(original, large, "large");
    checkDeepCopy(original, large, "large");

    // Still different from previous copies
    EXPECT_TRUE(original != empty);
    EXPECT_TRUE(original != tiny);
}


/**
 * \brief Assign one TestingString to another, then verify the assignment
 *
 * \remarks This relies on a correct copy constructor, since the inputs
 *          are copied for repeated testing.
 * \param lhs          The left side of assignment
 * \param rhs          The right side of assignment
 */
void assignTest(const TestingString& lhs, const TestingString& rhs)
{
    // Basic equality
    TestingString lhsBasic(lhs);
    lhsBasic = rhs;
    checkBothIdentical(lhsBasic, rhs, "lhsBasic");
    // Check deep copy by iterators
    checkDeepCopy(lhsBasic, rhs, "assign");

    // Deep copy by push_back
    TestingString lhsPush(lhs);
    lhsPush = rhs;
    lhsPush.push_back('r');
    EXPECT_TRUE(lhsPush != rhs);
    EXPECT_TRUE(rhs < lhsPush);

#if INSERT_ERASE
    // Deep copy by insert
    TestingString lhsInsert(lhs);
    lhsInsert = rhs;
    lhsInsert.insert(lhsInsert.begin(), 'c');
    EXPECT_TRUE(lhsPush != rhs);
#endif
}

/// Fixture object for functions requiring two arguments.
/// TEST_F functions in this suite have access to all protected
/// members of the class.
class DualArguments : public ::testing::Test {
protected:
    /// SetUp() is run immediately before each TEST_F in this suite
    virtual void SetUp() {
        // 0 = empty
        // 1 = one char
        // 2 = different one char
        // 3 = one full chunk
        // 4 = different full chunk
        // 5 = several chunks
        // 6 = different several chunks
        // 7 = very long string
        for (size_t i = 0; i < CHUNKSIZE * 10 + 5; ++i) {
            // Decide which strings still need filling
            size_t j = 1;

            if (i >= 3 * CHUNKSIZE + 1)
                j = 7;
            else if (i >= CHUNKSIZE)
                j = 5;
            else if (i >= 1)
                j = 3;

            for (; j < NUM_HELPERS; ++j) {
                helpers_[j].push_back(randomChar());
            }
        }

        // Make sure single char strings are different
        if (*helpers_[1].begin() == *helpers_[2].begin())
            *helpers_[1].begin() = *helpers_[1].begin() + 1;
    }

    static const size_t NUM_HELPERS = 8;  ///< Size of helpers_ array
    TestingString helpers_[NUM_HELPERS];  ///< Array of useful TestingStrings
};

/// Test Assignment in as many combinations as possible
TEST_F(DualArguments, assignment)
{
    for (size_t i = 0; i < NUM_HELPERS; ++i) {
        // Assign makes copies, so lets try self-assignment directly
        size_t preSize = helpers_[i].size();
        string value = stringFrom(helpers_[i]);
        helpers_[i] = helpers_[i];
        EXPECT_EQ(preSize, helpers_[i].size());
        ASSERT_EQ(value, stringFrom(helpers_[i]));

        // Assign each helper_ to each other helper_, to cover all cases
        // of small and large TestingStrings being assigned to each other.
        for (size_t j = 0; j < NUM_HELPERS; ++j)
            assignTest(helpers_[i], helpers_[j]);
    }

}

/// Test += in as many combinations as possible
TEST_F(DualArguments, append)
{
    for (size_t i = 0; i < NUM_HELPERS; ++i) {
        TestingString temp = helpers_[i];
        string control = stringFrom(temp);

        // Check for self-append
        temp += temp;
        control += control;
        checkWithControl(temp, control, "self append");

        // Check all pairs
        for (size_t j = 0; j < NUM_HELPERS; ++j) {
            temp = helpers_[i];
            control = stringFrom(temp);

            temp += helpers_[j];
            control += stringFrom(helpers_[j]);

            checkWithControl(temp, control, "i = " + stringFrom(i)
                   + ", j = " +  stringFrom(j));
        }
    }

    // Make sure they didn't directly link to the other object
    TestingString* heapString = new TestingString();
    heapString->push_back('a');
    heapString->push_back('b');
    // Use a full chunk for our lhs to encourage bad chunk combining behavior
    TestingString stackString;

    for (size_t i = 0; i < CHUNKSIZE; ++i) {
        stackString.push_back('z');
    }

    string control = stringFrom(stackString);
    stackString += *heapString;
    control += stringFrom(*heapString);
    // Ensure deep copy
    TestingString::iterator it = heapString->begin();
    *it = 'e';
    checkWithControl(stackString, control, "before heapString deleted");
    // Check for memory isses
    delete heapString;
    checkWithControl(stackString, control, "after heapString deleted");
}

/// Test equality and inequality
TEST_F(DualArguments, equality)
{
    // The variety of arguments in helpers_ assure we have all special
    // cases covered:
    // - empty, single chunk, multi chunk compared in all combos
    // - self equality
    // - same size inequality
    // - different size inequality
    for (size_t i = 0; i < NUM_HELPERS; ++i) {
        for (size_t j = 0; j < NUM_HELPERS; ++j) {
            string sI = stringFrom(i);
            string sJ = stringFrom(j);
            checkTwoWithControl(helpers_[i], helpers_[j],
                   stringFrom(helpers_[i]), stringFrom(helpers_[j]),
                   "i = " + sI + ", j = " + sJ);
        }
    }

#if INSERT_ERASE
    TestingString pushBack;
    TestingString insert;

    // Same string constructed different ways
    for (size_t i = 0; i < 128; ++i) {
        pushBack.push_back(i);
    }

    for (int i = 127; i >= 0; i -= 2) {
        insert.insert(insert.begin(), i);
    }

    TestingString::iterator it = insert.end();

    for (int i = 126; i >= 0; i -= 2) {
        --it;
        it = insert.insert(it, i);
    }

    checkBothIdentical(pushBack, insert, "same string, two constructions");

    // Almost the same, but one is longer
    pushBack.push_back('a');
    checkTwoWithControl(pushBack, insert, stringFrom(pushBack),
                        stringFrom(insert),
                        "identical but first is longer by one character");
#endif
}


/// Test push_back on a variety of characters
TEST(modifyChars, push_back)
{
    TestingString test;
    string control;

    // Add random characters
    for (size_t i = 0; i < CHUNKSIZE * 5; ++i) {
        checkWithControl(test, control, "basic chars. i = " + stringFrom(i) );
        char next = randomChar();
        test.push_back(next);
        control.push_back(next);
    }

    // Use weird characters
    TestingString test2;
    string control2;

    for (int i = 20; i >= 0; --i) {
        test2.push_back(i);
        control2.push_back(i);
        checkWithControl(test2, control2, "weird chars. i = " + stringFrom(i));
    }

    for (int i = 245; i < 256; ++i) {
        test2.push_back(i);
        control2.push_back(i);
        checkWithControl(test2, control2, "weird chars. i = " + stringFrom(i));
    }
}

/// Basic iteration tests
TEST(modifyChars, iterate)
{
    TestingString test;
    TestingString::iterator iter;       // Test default constructor

    // Empty string iterators
    iter = test.begin();
    EXPECT_TRUE(iter == test.end());

    // Add a few items
    test.push_back('a');
    iter = test.begin();
    EXPECT_TRUE(iter != test.end());
    EXPECT_EQ('a', *iter);

    // Forward iteration
    test.push_back('b');
    iter = test.begin();
    ++iter;
    EXPECT_EQ('b', *iter);
    ++iter;
    EXPECT_TRUE(iter == test.end());

    // Reverse iteration
    --iter;
    EXPECT_EQ('b', *iter);
    --iter;
    EXPECT_EQ('a', *iter);
    EXPECT_TRUE(iter == test.begin());

    // Already used assignment, but lets check it just to be sure
    iter = test.end();
    EXPECT_TRUE(iter == test.end());
    TestingString::iterator iter2 = iter;
    EXPECT_TRUE(iter2 == iter);
    EXPECT_TRUE(iter == iter2);
    // Point both iterators at different characters
    iter2 = test.begin();
    --iter;
    EXPECT_TRUE(iter2 != iter);
    EXPECT_TRUE(iter != iter2);
    // Assignment!
    iter2 = iter;
    EXPECT_EQ(*iter, *iter2);
}

#if INSERT_ERASE
/**
 * \brief Create a long string using insert.
 *
 * \param action        Method for inserting. MIN_VALUE = begin(),
 *                      MAX_VALUE = end(), RANDOM_VALUE = MID_VALUE = random
 *                      spot in the string.
 * \param origin        Description of calling function to assist user in
 *                      debugging errors.
 */
void basicInsertTest(randomness_t action, string origin)
{
    TestingString test;
    string        control;
    TestingString::iterator tIter;
    string::iterator        cIter;

    for (size_t i = 0; i < 500; ++i) {
        int r;

        switch (action) {
        case MAX_VALUE:
            tIter = test.end();
            cIter = control.end();
            break;

        case MID_VALUE:
        case RANDOM_VALUE:
            r = maybeRandomInt(test.size(), action);
            tIter = test.begin();
            std::advance(tIter, r);
            cIter = control.begin();
            std::advance(cIter, r);
            break;

        case MIN_VALUE:
        default:
            tIter = test.begin();
            cIter = control.begin();
            break;
        }

        char c = randomChar();
        tIter = test.insert(tIter, c);
        cIter = control.insert(cIter, c);
        checkIterWithControl(test, control, tIter, cIter,
                             origin + ", size:" + stringFrom(i));
        // We expect half full chunks when inserting only
        checkUtilization(test, 2, origin + ", size:" + stringFrom(i));
    }
}

/// Basic insert tests (with iterator checks)
TEST(modifyChars, insert)
{
    TestingString test;
    string        control;

    TestingString::iterator tIter;
    string::iterator        cIter;

    // Insert at begin()
    basicInsertTest(MIN_VALUE, "begin()");

    // Insert at end()
    basicInsertTest(MAX_VALUE, "end()");

    // Insert at midpoint
    basicInsertTest(MID_VALUE, "middle");

    // Insert randomly
    basicInsertTest(RANDOM_VALUE, "random");

    // Insert at chunk boundary
    for (size_t i = 0; i < 2 * CHUNKSIZE; ++i) {
        char c = randomChar();
        test.push_back(c);
        control.push_back(c);
    }

    ASSERT_GT(test.size(), CHUNKSIZE);
    tIter = test.begin();
    std::advance(tIter, CHUNKSIZE);
    cIter = control.begin();
    std::advance(cIter, CHUNKSIZE);
    tIter = test.insert(tIter, 'z');
    cIter = control.insert(cIter, 'z');
    checkIterWithControl(test, control, tIter, cIter, "left boundary");
    tIter = test.begin();
    std::advance(tIter, CHUNKSIZE + 1);
    cIter = control.begin();
    std::advance(cIter, CHUNKSIZE + 1);
    tIter = test.insert(tIter, 'D');
    cIter = control.insert(cIter, 'D');
    checkIterWithControl(test, control, tIter, cIter, "right boundary");
}
#endif

/**
 * \class LongString
 *
 * Test Fixture for tests that need a long string at the start.
 */
class LongString : public ::testing::Test {
// We use protected here because function using this fixture
// will inherit from this class
protected:
    virtual void SetUp() {
        for (size_t i = 0; i < SIZE; ++i) {
            char c = randomChar();
            testString_.push_back(c);
            controlString_.push_back(c);
        }
    }

    static const size_t SIZE = 500;     ///< Length of string
    TestingString testString_;          ///< TestingString created
    string controlString_;              ///< Expected value of testString_
};

#if INSERT_ERASE
/**
 * \brief Erase a long string using erase.
 *
 * \param test          String to erase
 * \param control       Expected value
 * \param action        Method for erasing. MIN_VALUE = begin(),
 *                      MAX_VALUE = end(), RANDOM_VALUE = MID_VALUE = random
 *                      spot in the string.
 * \param origin        Description of calling function to assist user in
 *                      debugging errors.
 */
void eraseAllTest(TestingString& test, string& control, randomness_t action,
              string origin)
{
    TestingString::iterator tIter;
    string::iterator        cIter;

    while (test.begin() != test.end()) {
        int r;

        switch (action) {
        case MAX_VALUE:
            tIter = --test.end();
            cIter = --control.end();

        case RANDOM_VALUE:
        case MID_VALUE:
            r = maybeRandomInt(test.size() - 1, action);
            tIter = test.begin();
            cIter = control.begin();
            std::advance(tIter, r);
            std::advance(cIter, r);
            break;

        case MIN_VALUE:
        default:
            tIter = test.begin();
            cIter = control.begin();
            break;
        }

        tIter = test.erase(tIter);
        cIter = control.erase(cIter);
        checkIterWithControl(test, control, tIter, cIter,
               "size: " + stringFrom(test.size()) + origin);
        // We expect 1/4 full chunks when erasing
        checkUtilization(test, 4, "size: " + stringFrom(test.size()) + origin);
    }

}


/// Basic erase tests (with iterator checks)
TEST_F(LongString, erase)
{
    // Make copies of our large strings
    TestingString test = testString_;
    string        control = controlString_;

    TestingString::iterator tIter;
    string::iterator        cIter;

    // Erase first character
    eraseAllTest(test, control, MIN_VALUE, "Erase from front");
    // Erase last character
    test = testString_;
    control = controlString_;
    eraseAllTest(test, control, MAX_VALUE, "Erase from back");
    // Erase randomly
    test = testString_;
    control = controlString_;
    eraseAllTest(test, control, RANDOM_VALUE, "Erase randomly");


    // Make sure push_back still works after erasing
    test.push_back(' ');
    control.push_back(' ');
    checkWithControl(test, control, "push_back");

    // Erase from chunk boundary
    test    = testString_;
    control = controlString_;
    ASSERT_GT(test.size(), CHUNKSIZE);
    tIter = test.begin();
    std::advance(tIter, CHUNKSIZE - 1);
    cIter = control.begin();
    std::advance(cIter, CHUNKSIZE - 1);
    tIter = test.erase(tIter);
    cIter = control.erase(cIter);
    checkIterWithControl(test, control, tIter, cIter, "left chunk boundary");

    ASSERT_GT(test.size(), CHUNKSIZE + 1);
    tIter = test.begin();
    std::advance(tIter, CHUNKSIZE);
    cIter = control.begin();
    std::advance(cIter, CHUNKSIZE);
    tIter = test.erase(tIter);
    cIter = control.erase(cIter);
    checkIterWithControl(test, control, tIter, cIter, "right chunk boundary");
}
#endif

/// Basic iteration tests
TEST_F(LongString, iterate)
{
    // Basic forward iteration
    TestingString::iterator tIter = testString_.begin();
    string::iterator        cIter = controlString_.begin();

    for (size_t i = 0; i < SIZE; ++i) {
        EXPECT_EQ(*cIter, *tIter);
        ++tIter;
        ++cIter;
    }

    EXPECT_TRUE(tIter == testString_.end());

    // Basic backward iteration
    tIter = testString_.end();
    cIter = controlString_.end();

    for (size_t i = 0; i < SIZE; ++i) {
        --tIter;
        --cIter;
        EXPECT_EQ(*cIter, *tIter);
    }

    EXPECT_TRUE(tIter == testString_.begin());
}

/// Create a low-utilization string by repeated appending
TEST(utilization, append)
{
    TestingString test;
    string        control;

    for (size_t i = 0; i < 550; ++i) {
        TestingString append;
        char c  = randomChar();
        append.push_back(c);
        append.push_back(c); // Two char chunk, in case 1 char is a special case
        test += append;
        control.push_back(c);
        control.push_back(c);

        checkUtilization(test, 2, "i = " + stringFrom(i));
        checkWithControl(test, control, "i = " + stringFrom(i));
    }

}

/// Advances iter by dist or until it hits end, whichever comes first
void myAdvance(TestingString::iterator& iter,
               const TestingString::iterator& end, size_t dist)
{
    while (iter != end && dist > 0) {
        ++iter;
        --dist;
    }
}

#if INSERT_ERASE
/// Try to break the utilization of the string using insert and erase.
/// Strategy: force overflow of each chunk, then erase full chunks.
TEST_F(LongString, utilizationOverflow)
{
    // Overflow each chunk
    TestingString::iterator iter = testString_.begin();
    advance(iter, CHUNKSIZE - 1); // Jump to end of chunk

    for ( ; iter != testString_.end();
            // advance by CHUNKSIZE + 1 since we are inserting
            myAdvance(iter, testString_.end(), CHUNKSIZE + 1)) {
        iter = testString_.insert(iter, 'x');
        checkUtilization(testString_, 2,
                         "size: " + stringFrom(testString_.size()));
    }

    // Erase the full chunks
    for (iter = testString_.begin(); iter != testString_.end(); ++iter) {
        for (size_t j = 0; j < CHUNKSIZE; ++j) {
            if (iter == testString_.end())
                break;

            iter = testString_.erase(iter);
        }

        checkUtilization(testString_, 4,
                         "size: " + stringFrom(testString_.size()));

        // Don't ++iter if we are at the end
        if (iter == testString_.end())
            break;
    }
}


enum insertpoint_t { FRONT, BACK };
enum usepush_t { INSERT_ONLY, PUSH_AND_INSERT };

/**
 * Test insert.  Inserts characters into a string within a locus of 'n' from
 * the front or back (depending on whether where is FRONT or BACK), exactly
 * how the locus is defined depends on randomize and the number of characters
 * currently in the string.
 */
void randomInsertTest(size_t n, insertpoint_t where,
                   randomness_t randomize, usepush_t method,
                   string origin)
{
    string backtrace = "Backtrace: " + origin + ", n = " + stringFrom(n);

    // Test data
    static const string TEST_DATA("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                  "abcdefghijklmnopqrstuvwxyz" "123456789");
    static const size_t TEST_DATA_SIZE = TEST_DATA.size();

    string control;
    TestingString test;

    for (size_t i = 0; i < TEST_DATA_SIZE;  ++i) {
        char c = TEST_DATA[i];

        int m; // numeric index of point where we wish to insert

        if (where == FRONT)
            m = n > i ? i : maybeRandomInt(n, randomize);
        else
            m = n > i ? 0 : i - maybeRandomInt(n, randomize);

        // Find desired insertion point
        TestingString::iterator p = test.begin();
        advance(p, m);

        TestingString::iterator tIter =
            (method == PUSH_AND_INSERT && (p == test.end()))
            ? (test.push_back(c), --test.end())
            : test.insert(p, c);

        string::iterator mIter = control.insert(control.begin() + m, c);
        // Biggest deal, does the string look the way it should
        checkIterWithControl(test, control, tIter, mIter,
                             backtrace + ", i = " + stringFrom(i));
        // Utilization okay?
        checkUtilization(test, 4, backtrace);
    }
}


/**
 * Test erase.  Erases characters into a string within a locus of 'n' from
 * the front or back (depending on whether where is FRONT or BACK), exactly
 * how the locus is defined depends on randomize and the number of characters
 * currently in the string.
 */

void randomEraseTest(size_t n, insertpoint_t where, randomness_t randomize,
                     string origin)
{
    string backtrace = "Backtrace: " + origin + ", n = " + stringFrom(n);
    // Test data
    static const string TEST_DATA("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                  "abcdefghijklmnopqrstuvwxyz" "123456789");
    static const size_t TEST_DATA_SIZE = TEST_DATA.size();

    string control(TEST_DATA);

    TestingString test;

    for (string::const_iterator i = TEST_DATA.begin();
            i != TEST_DATA.end();
            ++i)
        test.push_back(*i);


    for (size_t i = TEST_DATA_SIZE; i > 0; --i) {
        // The string is not yet empty
        EXPECT_FALSE(test == TestingString()) << origin;

        int m;

        if (where == FRONT)
            m = n >= i ? i - 1 : maybeRandomInt(n, randomize);
        else
            m = n >= i ? 0 : i - 1 - maybeRandomInt(n, randomize);


        TestingString::iterator p = test.begin();
        advance(p, m);

        TestingString::iterator tIter = test.erase(p);
        string::iterator cIter = control.erase(control.begin() + m);

        checkIterWithControl(test, control, tIter, cIter,
               backtrace + ", i = " + stringFrom(i));

        if (i > 0)
            checkUtilization(test, 4, backtrace);
    }

    // The string is now empty
    EXPECT_TRUE(test == TestingString()) << origin;
}

/// Try to break utilization by pure insertion
TEST(utilization, insertion)
{
    for (randomness_t r : { MIN_VALUE, MAX_VALUE, MID_VALUE, RANDOM_VALUE }) {
        for (int i = 0; i < 25; ++i)
            randomInsertTest(i, FRONT, r, INSERT_ONLY, "front");
        
        for (int i = 0; i < 25; ++i)
            randomInsertTest(i, BACK, r, INSERT_ONLY, "back");
    }
}

/// Try to break utilization by a combo of insertion and push_back
TEST(utilization, pushAndInsertion)
{
    for (randomness_t r : { MIN_VALUE, MAX_VALUE, MID_VALUE, RANDOM_VALUE }) {
        for (int i = 0; i < 25; ++i)
            randomInsertTest(i, FRONT, r, PUSH_AND_INSERT, "front");
        
        for (int i = 0; i < 25; ++i)
            randomInsertTest(i, BACK, r, PUSH_AND_INSERT, "back");
    }
}

/// Try to break utilization by pure erasing
TEST(utilization, erase)
{
    for (randomness_t r : { MIN_VALUE, MAX_VALUE, MID_VALUE, RANDOM_VALUE }) {
        for (int i = 0; i < 25; ++i)
            randomEraseTest(i, FRONT, r, "front");
        
        for (int i = 0; i < 25; ++i)
            randomEraseTest(i, BACK, r, "back");
    }
}
#endif


// Called if the test runs too long.
static void timeout_handler(int)
{
    // We go super-low-level here, because we can't trust anything in
    // the C/C++ library to really be working right.
    write(STDERR_FILENO, "Timeout occurred!\n", 18);
    abort();
}


/// Run tests
int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
#if LOAD_GENERIC_STRING
    // Load the desired string implementation, if that's what we're doing.

    if (argc != 2) {
        cerr << "Usage ./stringtest [Google Test options] plugin" << endl;
        exit(0);
    }

    GenericString::loadImplementation(argv[1]);
#else
    // No setup to do if we're using ChunkyString directly
#endif

    // To handle tests that run much too long, we have set a limit on how
    // long the program is allowed to execute for.  If it goes beyond that
    // time, the program will call our timeout handler function, which will
    // forcibly terminate the program.
    //
    // But, we only enable it if we're not running with 
    //          --gtest_break_on_failure
    // because if we've got that argument, we're probably running in the
    // debugger, and we'll probably want some time to think about what is going
    // on.
    const int MAX_RUNTIME = 10;             // Timeout, in seconds
    if (not ::testing::GTEST_FLAG(break_on_failure)) {
        signal(SIGALRM, timeout_handler);   // What to call when timer expires
        alarm(MAX_RUNTIME);                 // set timer at MAX_RUNTIME seconds
    }
    
    // Determine and set the "constant" for CHUNKSIZE...  we can't assume it's
    // twelve.
    
    TestingString oneChar;
    oneChar.push_back('x');
    double oneCharUtilization = oneChar.utilization();
    CHUNKSIZE = lround(1.0/oneCharUtilization);
    if (CHUNKSIZE < 4 || CHUNKSIZE > 256) {
        cerr << "Couldn't determine a sane value for CHUNKSIZE\n"
                "\toneChar.utilization() is " << oneCharUtilization << endl;
        return 42;
    }
    
    return RUN_ALL_TESTS();
}

/* (Junk to make emacs use Stroustrup-style indentation)
 * Local Variables:
 * c-file-style: "stroustrup"
 * End:
 */
