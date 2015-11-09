/*********************************************************************
 * ChunkyString::Iterator class.
 *********************************************************************
 *
 * Implementation for the templated ChunkyString iterator
 *
 */

#include <stdexcept>

template <bool const_it>
ChunkyString::Iterator<const_it>::Iterator()
{
    // Nothing to do here..
}

template <bool const_it>
ChunkyString::Iterator<const_it>::Iterator(list_iterator_type chunk,
                                             size_t charIndex)
{
    chunk_ = chunk;
    charInd_ = charIndex;
}

template <bool const_it>
ChunkyString::Iterator<const_it>::Iterator(const Iterator<false>& i)
    : chunk_{i.chunk_}, charInd_{i.charInd_}
{
    // Nothing to do here!
}

template <bool const_it>
ChunkyString::Iterator<const_it>& ChunkyString::Iterator<const_it>::operator++()
{
    // sets the iterator to point to the next char in the ChunkyString

    // case for iterator points to last char in Chunk
    if(charInd_ == chunk_->length_-1)
    {
        // set iterator to point to first char of next Chunk
        // if iterator pointed to last char, it will be equal to the
        // end iterator
        ++chunk_;
        charInd_ = 0;
    }
    else
    {
        ++charInd_;
    } 
    return *this;
}

template <bool const_it>
ChunkyString::Iterator<const_it>& 
    ChunkyString::Iterator<const_it>::operator--()
{
    // sets the iterator to point to the previous char in ChunkyString

    if (charInd_ == 0)
    {
        --chunk_;
        charInd_ = chunk_->length_-1;
    }
    else
    {
        --charInd_;
    }
    return *this;
}

template <bool const_it>
typename ChunkyString::Iterator<const_it>::reference 
    ChunkyString::Iterator<const_it>::operator*() const
{
    // Return the char curr_ points to
    return chunk_->chars_[charInd_];
}

template <bool const_it>
bool ChunkyString::Iterator<const_it>::operator==(const Iterator& rhs) const
{
    // Checks if two iterators hold the same Chunk address and same
    // location within the array
    return chunk_ == rhs.chunk_ && charInd_ == rhs.charInd_;  
}

template <bool const_it>
bool ChunkyString::Iterator<const_it>::operator!=(const Iterator& rhs) const
{
    // leverage == to implement !=
    return !(*this == rhs); 
}
