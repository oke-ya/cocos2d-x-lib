#ifndef __central__NonCopyable__
#define __central__NonCopyable__

template <class T>
class NonCopyable
{
public:
    NonCopyable () = default;
    virtual ~NonCopyable () = default;
    NonCopyable (const NonCopyable &) = delete;
    NonCopyable(const NonCopyable &&) = delete;
    T& operator=(const T &) = delete;
    T& operator=(const T&&) = delete;
};

#endif /* defined(__central__NonCopyable__) */
