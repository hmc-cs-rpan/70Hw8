/*
 * \file chunkystring.cpp
 * \authors Ricky Pan, Iris Liu
 * \brief Implementation of ChunkyString and its private classes
 */

#include "chunkystring.hpp"

#include <cassert>

ChunkyString::ChunkyString()
    : size_{0}
{
    std::list<Chunk> chunks;
    chunks_ = chunks;
}

ChunkyString::ChunkyString(const ChunkyString& orig)
{
    // initialize default values for a ChunkyString
    size_ = 0;
    std::list<Chunk> chunks;
    chunks_ = chunks;

    // pushes all of the elements in orig into our ChunkyString
    for(const_iterator i = orig.begin(); i != orig.end(); ++i)
    {
        push_back(*i);
    }
}

void ChunkyString::swap(ChunkyString& rhs)
{
    using std::swap;

    swap(chunks_, rhs.chunks_);
    swap(size_, rhs.size_);
}

ChunkyString::iterator ChunkyString::begin() 
{
    return Iterator<false>(chunks_.begin(), 0);
}

ChunkyString::iterator ChunkyString::end() 
{
    return Iterator<false>(chunks_.end(), 0);
}

ChunkyString::const_iterator ChunkyString::begin() const
{
    return Iterator<true>(chunks_.begin(), 0);
}

ChunkyString::const_iterator ChunkyString::end() const
{
    return Iterator<true>(chunks_.end(), 0);
}

ChunkyString& ChunkyString::operator+=(const ChunkyString& rhs)
{
    ChunkyString deepCopy = ChunkyString(rhs);

    // push_back each char from the deepCopy of rhs
    for (iterator i = deepCopy.begin(); i != deepCopy.end(); ++i)
    {
        this->push_back(*i);
    }
    return *this;
}


void ChunkyString::push_back(char c)
{
    // adds a char c to the end of our ChunkyString
    if (size_ == 0 || chunks_.back().length_ == CHUNKSIZE)
    {
        // create a newChunk
        Chunk newChunk = Chunk(0, CHUNKSIZE);
        // push_back newChunk to our chunks_ list
        chunks_.push_back(newChunk);
        chunks_.back().chars_[0] = c;
        chunks_.back().length_ += 1;
    }
    else
    {
        // place in next available array index
        size_t nextInd = chunks_.back().length_;
        chunks_.back().chars_[nextInd] = c;
        chunks_.back().length_ += 1;
    }
    ++size_;    
}

ChunkyString::iterator ChunkyString::insert(iterator i, char c)
{
    // if iterator points to end
    if(i==end())
    {
        // use predefined function push_back()
        push_back(c);
        iterator toReturn = end();
        --toReturn;
        return toReturn;
    }

    // if current Chunk is full
    if(i.chunk_->length_ == CHUNKSIZE)
    {
        // create a Chunk iterator to the Chunk after current Chunk
        std::list<Chunk>::iterator nextChunk = i.chunk_;
        ++nextChunk;

        Chunk newChunk = Chunk(0, CHUNKSIZE);
        nextChunk = chunks_.insert(nextChunk, newChunk);

        // copying over the last half of chars in current Chunk to new Chunk
        for(size_t ind = 0; ind < CHUNKSIZE/2; ++ind)
        {
            // fills newChunk with chars from current Chunk
            nextChunk->chars_[ind] = i.chunk_->chars_[CHUNKSIZE/2 + ind];

            // adjusts Chunk lengths
            --i.chunk_->length_;
            ++nextChunk->length_;
        } 

        // check to see if iterator changed from copying elements
        if(i.charInd_ > CHUNKSIZE/2)
        {
            i = iterator(nextChunk, i.charInd_ - CHUNKSIZE/2);
        }

        // inserts char c into the proper space in the Chunk's char[]
        helperInsert(i, c);
    }

    else // if Chunk iterator points to is not full
    {
        // inserts char c into the proper space in the Chunk's char[]
        helperInsert(i, c);
    }

    ++size_;
    return i;
}

ChunkyString::iterator ChunkyString::erase(iterator i)
{
    if(i == end())
    {
        std::cout << "Invalid iterator, please try again" << std::endl;
    }

    else if(i.chunk_->length_ <= 1)
    {
        if(i.chunk_->length_ == 1)
        {
            --size_;
        }
        // erase the one char chunk we are pointing to
        std::list<Chunk>::iterator delChunk = chunks_.erase(i.chunk_);
        
        // fix iterator to point to first element of nextChunk
        i = ChunkyString::iterator(delChunk, 0);
    }

    // if the iterator points to the last char in an array
    else if(i.charInd_ == i.chunk_->length_ - 1)
    {
        // decrement length by 1
        --i.chunk_->length_;
        --i;
        --size_;
    }

    else if(i.charInd_ > 1 && i.charInd_ < i.chunk_->length_ - 1)
    // iterator will be pointing inside an array
    {
        for(size_t ind = i.charInd_; ind < i.chunk_->length_ - 1; ++ind)
        {
            // shifts all the elements after iterator position back 1 index
            i.chunk_->chars_[ind] = i.chunk_->chars_[ind + 1];
        }
        --i.chunk_->length_;
        --size_;
    }

    if(utilization() < 0.25)
    {
        i=reflow(i);
    }

    return i;
}

ChunkyString::iterator ChunkyString::reflow(iterator i)
{
    std::list<Chunk>::iterator lastChunk = chunks_.end();
    --lastChunk;

    if(i.chunk_ == lastChunk)
    {
        // now points to the Chunk before current chunk
        --lastChunk;
        for(size_t ind = 0; ind < i.chunk_->length_; ++ind)
        {
            // append the elements of current chunk to prev chunk
            lastChunk->chars_[lastChunk->length_ + ind] = 
            i.chunk_->chars_[ind];
        }
        
        // fix iterator
        ChunkyString::iterator toReturn = 
        iterator(lastChunk, lastChunk->length_ + i.charInd_);

        // accounts for the change in length from adding chars
        lastChunk->length_ += i.chunk_->length_;
        
        lastChunk = chunks_.erase(i.chunk_);

        return toReturn;

    }

    else
    {
        // creates iterator to access chars of nextChunk
        std::list<Chunk>::iterator nextChunk = i.chunk_;
        ++nextChunk;

        for(size_t ind = 0; ind < nextChunk->length_; ++ind)
        {
            // append the elements of next chunk to current chunk
            i.chunk_->chars_[i.chunk_->length_ + ind] = 
            nextChunk->chars_[ind];
        }
        
        // accounts for the change in length from adding chars
        i.chunk_->length_ += nextChunk->length_;
        
        chunks_.erase(nextChunk);

        // iterator stays the same
        return i;
    }

}


void ChunkyString::helperInsert(iterator& i, char c)
{
    size_t length = i.chunk_->length_;
    size_t charInd = i.charInd_;
    // making room for extra element in array by shifting all elements
    // after insert position down by 1 index
    for(size_t ind = 0; ind < length - charInd; ++ind)
    {
        i.chunk_->chars_[length - ind] = i.chunk_->chars_[length - ind - 1];
    }

    // finally, insert the character into the Chunk
    i.chunk_->chars_[charInd] = c;
    ++i.chunk_->length_;
}

size_t ChunkyString::size() const
{
    return size_;
}

ChunkyString& ChunkyString::operator=(const ChunkyString& rhs)
{
    // Assignment is implemented idiomatically using the "swap trick"
    ChunkyString copy = rhs;
    swap(copy);
    return *this;
}

bool ChunkyString::operator==(const ChunkyString& rhs) const
{
    if(size_ != rhs.size_)
    {
        return false;
    }

    // Initializes 1 iterator to loop through each ChunkyString
    const_iterator a = this->begin();
    const_iterator b = rhs.begin();

    for(size_t i = 0; i < size_; ++i)
    {
        if(*a != *b)
        {
            return false;
        }
        else
        {
            // increments iterators
            ++a;
            ++b;
        }
    }
    return true;
}

bool ChunkyString::operator!=(const ChunkyString& rhs) const
{
    // Idiomatic code: leverage == to implement !=
    return !(*this == rhs);
}

bool ChunkyString::operator<(const ChunkyString& rhs) const
{
    return std::lexicographical_compare(this->begin(), this->end(),
                                         rhs.begin(), rhs.end());
}

std::ostream& operator<<(std::ostream& out, 
    const ChunkyString& text)
{
    for(ChunkyString::const_iterator i = text.begin(); i != text.end(); ++i)
    {
        out << *i;
    }
    
    return out;
}

double ChunkyString::utilization() const
{
    return double(size_)/(chunks_.size()*CHUNKSIZE);
}

// ---------------------------------------------
// Implementation of ChunkyString::Chunk
// ---------------------------------------------
//
ChunkyString::Chunk::Chunk(size_t length, size_t CHUNKSIZE)
    : length_{0}
{
    char chars_[CHUNKSIZE]; 
}
