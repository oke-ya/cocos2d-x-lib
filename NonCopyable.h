#ifndef __central__NonCopyable__
#define __central__NonCopyable__

template <class T>
class NonCopyable
{
protected:
    NonCopyable () {}
    ~NonCopyable () {}
private:
    NonCopyable (const NonCopyable &);
    NonCopyable(const NonCopyable &&);
    T & operator = (const T &);
    T& operator =(T&&);
};

#endif /* defined(__central__NonCopyable__) */
