#ifndef _RANGE_KEY_H_
#define _RANGE_KEY_H_

// Behavior: A range is defined mathematically as (min_, max_]

template <class Type>
class RangeKey {
public:
  Type min_;
  Type max_;

  RangeKey(const Type & min, const Type & max)
  {
    min_ = min;
    max_ = max;
  };

  RangeKey(const Type & val)
  {
    min_ = val;
    max_ = val;
  };

  int compare(const RangeKey rhs)
  {
    // 1 if this > rhs
    // 0 if this == rhs
    //-1 if this < rhs
    if (rhs.min_ == rhs.max_ && min_ == max_)
    {
      /*if (min_ > rhs.min_)
              return 1;
      else if (min_ == rhs.min_)
              return 0;
      else
              return -1;*/
      throw "You should not be comparing values like this. :(\n";
    }
    else if (rhs.max_ == rhs.min_)
    {
      if (min_ > rhs.min_) 
              return 1;
      else if (min_ <= rhs.min_ && max_ > rhs.min_)
              return 0;
      else // (max_ <= rhs.min_)
              return -1;
    }
    else if (min_ == max_)
    {
      if (min_ >= rhs.max_)
              return 1;
      else if (min_ < rhs.max_ && min_ >= rhs.min_)
              return 0;
      else // if (min_ < rhs.min_
              return -1;
    }

    // Check if the two ranges are equal:
    if (rhs.min_ == min_ && rhs.max_ == max_)
    {
      return 0;
    }
    else if (rhs.min_ < min_ && rhs.max_ <= min_)
    {
      // This is what happens if rhs is fully lower than this one.
      return 1;
    }
    else if (rhs.min_ > min_ && rhs.min_ >= max_)
    {
      return -1;
    }
    else
    {
      // This means there's an undefined case. Ranges are overlapping, 
      // so comparisons don't work quite nicely.

      throw "Ranges are overlapping weirdly. :(\n";
    }
  };

  int compare(const RangeKey rhs) const
  {
    // 1 if this > rhs
    // 0 if this == rhs
    //-1 if this < rhs
    if (rhs.min_ == rhs.max_ && min_ == max_)
    {
      /*if (min_ > rhs.min_)
              return 1;
      else if (min_ == rhs.min_)
              return 0;
      else
              return -1;*/
      throw "You should not be comparing values like this. :(\n";
    }
    else if (rhs.max_ == rhs.min_)
    {
      if (min_ > rhs.min_) 
        return 1;
      else if (min_ <= rhs.min_ && max_ > rhs.min_)
        return 0;
      else // (max_ <= rhs.min_)
        return -1;
    }
    else if (min_ == max_)
    {
      if (min_ >= rhs.max_)
        return 1;
      else if (min_ < rhs.max_ && min_ >= rhs.min_)
        return 0;
      else // if (min_ < rhs.min_
        return -1;
    }

    // Check if the two ranges are equal:
    if (rhs.min_ == min_ && rhs.max_ == max_)
    {
      return 0;
    }
    else if (rhs.min_ < min_ && rhs.max_ <= min_)
    {
      // This is what happens if rhs is fully lower than this one.
      return 1;
    }
    else if (rhs.min_ > min_ && rhs.min_ >= max_)
    {
      return -1;
    }
    else
    {
      // This means there's an undefined case. Ranges are overlapping, 
      // so comparisons don't work quite nicely.

      throw "Ranges are overlapping weirdly. :(\n";
    }
  };

  bool operator== (const RangeKey rhs ) {return (*this).compare(rhs)==0;};
  bool operator== (const RangeKey rhs ) const {return (*this).compare(rhs)==0;};
  bool operator!= (const RangeKey rhs ) {return (*this).compare(rhs)!=0;};
  bool operator!= (const RangeKey rhs ) const {return (*this).compare(rhs)!=0;};
  bool operator< (const RangeKey rhs ) {return (*this).compare(rhs)<0;};
  bool operator< (const RangeKey rhs ) const {return (*this).compare(rhs)<0;};
  bool operator> (const RangeKey rhs ) {return (*this).compare(rhs)>0;};
  bool operator> (const RangeKey rhs ) const {return (*this).compare(rhs)>0;};
  bool operator<= (const RangeKey rhs ) {return (*this).compare(rhs)<=0;};
  bool operator<= (const RangeKey rhs ) const {return (*this).compare(rhs)<=0;};
  bool operator>= (const RangeKey rhs ) {return (*this).compare(rhs)>=0;};
  bool operator>= (const RangeKey rhs ) const {return (*this).compare(rhs)>=0;};
};

#endif // _RANGE_KEY_H_

