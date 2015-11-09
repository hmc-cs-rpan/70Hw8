/**
 * \file chunkystring.hpp
 *
 * \authors CS 70 given code, with additions by ... your names here ...
 *
 * \brief Declares the ChunkyString class.
 */

#ifndef CHUNKYSTRING_HPP_INCLUDED
#define CHUNKYSTRING_HPP_INCLUDED 1

#include <cstddef>
#include <string>
#include <list>
#include <iterator>
#include <iostream>
#include <type_traits>

/**
 * \class ChunkyString
 * \brief Efficiently represents strings where insert and erase are
 *    constant-time operations.
 *
 * \details This class is comparable to a linked-list of characters,
 *   but more space efficient.
 *
 * \remarks
 *   reverse_iterator and const_reverse_iterator aren't
 *   supported. Other than that, we use the STL container typedefs
 *   such that STL functions are compatible with ChunkyString.
 */
class ChunkyString {
    // Forward declaration of private class.
    template <bool const_iter>
    class Iterator;

public:
    // Standard STL container type definitions
    using value_type      = char;
    using size_type       = size_t;
    using difference_type = ptrdiff_t;
    using reference       = value_type&;
    using const_reference = const value_type&;

    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;

    // reverse_iterator and const_reverse_iterator aren't supported

    /**
     * \brief Default constructor
     *
     * \note constant time
     */
    ChunkyString();

    ~ChunkyString() = default;

    /**
     * \brief Copy constructor
     */
    ChunkyString(const ChunkyString& orig);

    /// Return an iterator to the first character in the ChunkyString.
    iterator begin();
    /// Return an iterator to "one past the end"
    iterator end();

    /// Return a const iterator to the first character in the ChunkyString.
    const_iterator begin() const;
    /// Return a const iterator to "one past the end"
    const_iterator end() const;

    /**
     * \brief Inserts a character at the end of the ChunkyString.
     *
     * \param c     Character to insert
     * 
     * \note constant time
     */
    void push_back(char c);

    // Standard string functions: size, append, equality, less than    
    size_t size() const;    ///< String size \note constant time
    static const size_t CHUNKSIZE = 12;
    
    ChunkyString& operator+=(const ChunkyString& rhs); ///< String concatenation

    bool operator==(const ChunkyString& rhs) const;    ///< String equality
    bool operator!=(const ChunkyString& rhs) const;    ///< String inequality

    /// Lexicographical string comparison
    bool operator<(const ChunkyString& rhs) const; 

    /**
     * \brief Insert a character before the character at i.
     * \details
     *   What makes ChunkyString special is its ability to insert and
     *   erase characters quickly while remaining space efficient.
     *
     * \param i     iterator to specify insertion point
     * \param c     character to insert
     *
     * \returns an iterator pointing to the newly inserted character.
     *
     * \note constant time
     *
     * \warning invalidates all iterators except the returned iterator
     */
    iterator insert(iterator i, char c);

    /**
     * \brief Erase a character at i
     * \details
     *   What makes ChunkyString special is its ability to insert and
     *   erase characters quickly while remaining space efficient.
     *
     * \param i     iterator pointing to the character to erase
     *
     * \returns an iterator pointing to the character after the one
     *   that was deleted.
     *
     * \note constant time
     *
     * \warning invalidates all iterators except the returned iterator
     */
    iterator erase(iterator i);

    /**
     * \brief Average capacity of each chunk, as a percentage
     * \details 
     *   This function computes the fraction of the ChunkyString's character
     *   cells that are in use. It is defined as 
     *
     *   \f[\frac{\mbox{number of characters in the string}}
     *           {\mbox{number of chunks}\times\mbox{CHUNKSIZE}}  \f] 
     *   
     *   For reasonably sized strings (i.e., those with more than one or two 
     *   characters), utilization should never fall to near one character per 
     *   chunk; otherwise the data structure would be wasting too much space.
     *
     *   The utilization for an empty string is undefined (i.e., any value is
     *   acceptable).
     */
    double utilization() const;

private:
    /***
     * \struct Chunk
     *
     * \brief The string is stored as a linked-list of Chunks.
     *        The class is private so only ChunkyString knows about it.
     *
     */
    struct Chunk {

       size_t length_;
       char chars_[CHUNKSIZE];

       Chunk(size_t length_, size_t CHUNKSIZE);
    };

    std::list<Chunk> chunks_; 
    size_t size_; // Current size of ChunkyString

    /**
     * \class Iterator
     * \brief STL-style iterator for ChunkyString.
     *
     * \details Synthesized copy constructor, destructor, and assignment
     *          operator are okay.
     *
     *          The five typedefs and the member functions are such that
     *          the iterator works properly with STL functions (e.g., copy).
     *
     *          Since this is a bidirectional_iterator, `operator--`
     *          is provided and meaningful for all iterators except
     *          ChunkyString::begin.
     *
     *  \remarks The design of the templated iterator was inspired by these
     *           two sources:
     *  www.drdobbs.com/the-standard-librarian-defining-iterato/184401331
     *  www.sj-vs.net/c-implementing-const_iterator-and-non-const-iterator-without-code-duplication
     */
    template <bool const_iter>
    class Iterator {
    public:
        
        ///< Default constructor
        Iterator();

        ///< Convert a non-const iterator to a const-iterator, if necessary
        Iterator(const Iterator<false>& i);  

        // Make Iterator STL-friendly with these typedefs:
        using value_type = char;
        using reference = typename std::conditional<const_iter, 
                                                    const value_type&, 
                                                    value_type&>::type;
        using pointer = typename std::conditional<const_iter, 
                                                  const value_type*, 
                                                  value_type*>::type;
        using list_iterator_type = typename std::conditional<const_iter, 
                                    std::list<Chunk>::const_iterator, 
                                    std::list<Chunk>::iterator>::type;
        using difference_type   = ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;
        using const_reference   = const value_type&;

        // Operations
        Iterator& operator++();
        Iterator& operator--();
        reference operator*() const;
        bool operator==(const Iterator& rhs) const;
        bool operator!=(const Iterator& rhs) const;

    private:
        friend class ChunkyString;
        friend struct Chunk;
        Iterator(list_iterator_type chunk_, size_t charInd_);
        list_iterator_type chunk_;
        size_t charInd_;
    };
};

/**
 * \brief Print operator: displays a ChunkyString on the given stream
 * 
 * \param out   the display stream
 * \param text  a ChunkyString to display
 * 
 * \returns the display stream
 */
std::ostream& operator<<(std::ostream& out, const ChunkyString& text);

#include "iterator-private.hpp"

#endif // CHUNKYSTRING_HPP_INCLUDED
