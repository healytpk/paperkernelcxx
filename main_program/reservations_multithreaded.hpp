#ifndef HEADER_INCLUSION_GUARD_RESERVATIONS_MULTITHREADED
#define HEADER_INCLUSION_GUARD_RESERVATIONS_MULTITHREADED

#ifdef __cplusplus
#    if ( (defined(_MSC_VER) && (_MSVC_LANG < 201100L)) || (!defined(_MSC_VER) && (__cplusplus < 201100L)) )
#        error "This header file requires a compiler that supports at least the 2011 C++ Standard for multithreading"
#        include "no_such_header_file_see_previous_error"
#    endif
#else
#    error "This header file must be included in a C++ source file -- not a C or Objective C source file"
#    include "no_such_header_file_see_previous_error"
#endif

#include <mutex>        // recursive_mutex, lock_guard, unique_lock
#include <utility>      // move, forward, declval
#include <type_traits>  // is_pointer, is_reference, conditional, is_same, remove_cv, remove_ref, remove_cvref, remove_pointer
#include <memory>       // addressof
//#include <concepts>     // BasicLockable

#if (__cplusplus >= 201700L) || (defined(__has_cpp_attribute) && __has_cpp_attribute(nodiscard))
#    define attrib_nodiscard [[nodiscard]]
#else
#    define attrib_nodiscard /* nothing */
#endif

#if (__cplusplus >= 201907L) && defined(__cpp_concepts)

template<typename T>
concept BasicLockable = std::is_same_v< T, std::remove_cvref_t<T> > && requires
  {
    std::declval<T>().lock();
    std::declval<T>().unlock();
  };

template<typename T, BasicLockable MutexType = std::recursive_mutex>
requires (!std::is_reference_v<T>)
class Reserved final {

    Reserved(void) = delete;
    Reserved(Reserved const &) = delete;
    Reserved(Reserved &&) = delete;
    Reserved &operator=(Reserved const &) = delete;
    Reserved &operator=(Reserved &&) = delete;
    Reserved const volatile *operator&(void) const volatile = delete;
    template<typename U> void operator,(U&&) = delete;

    T &m_ref;
    std::lock_guard<MutexType> m_locker;

public:
    // Constructor might throw if argM.lock() throws std::system_error
    Reserved(MutexType &argM, T &argR) noexcept( noexcept(argM.lock()) )
      : m_ref(argR), m_locker(argM) {}

    attrib_nodiscard auto operator->(void) noexcept
    {
        if constexpr ( std::is_pointer_v<T> )
            return m_ref;
        else
            return std::addressof(m_ref);
    }

    attrib_nodiscard auto operator->(void) const noexcept
    {
        // Don't dereference a nullptr inside this function! (decltype is ok)
        if constexpr ( std::is_pointer_v<T> )
            return const_cast<decltype(*m_ref) const *>(m_ref);
        else
            return std::addressof(const_cast<T const &>(m_ref));
    }

    attrib_nodiscard auto &operator*(void) noexcept
    {
        if constexpr ( std::is_pointer_v<T> )
            return *m_ref;
        else
            return m_ref;
    }

    attrib_nodiscard auto const &operator*(void) const noexcept
    {
        if constexpr ( std::is_pointer_v<T> )
            return *m_ref;
        else
            return m_ref;
    }
};

#elif (__cplusplus >= 201700L) || defined(__cpp_guaranteed_copy_elision)

template<typename T, class MutexType = std::recursive_mutex>
class Reserved final {

    static_assert( !std::is_reference_v<T>, "T cannot be a reference" );
    static_assert(
        std::is_same_v< MutexType, std::remove_cv_t< std::remove_reference_t<MutexType> > >,
        "MutexType must be a bare type (no cvref)" );

    Reserved(void) = delete;
    Reserved(Reserved const &) = delete;
    Reserved(Reserved &&) = delete;  // not needed because of guaranteed return value optimisation
    Reserved &operator=(Reserved const &) = delete;
    Reserved &operator=(Reserved &&) = delete;
    Reserved const volatile *operator&(void) const volatile = delete;
    template<typename U> void operator,(U&&) = delete;

    T &m_ref;
    std::lock_guard<MutexType> m_locker;

public:
    // Constructor might throw if argM.lock() throws std::system_error
    Reserved(MutexType &argM, T &argR) noexcept( noexcept(argM.lock()) )
      : m_ref(argR), m_locker(argM) {}

    attrib_nodiscard auto operator->(void) noexcept
    {
        if constexpr ( std::is_pointer_v<T> )
            return m_ref;
        else
            return std::addressof(m_ref);
    }

    attrib_nodiscard auto operator->(void) const noexcept
    {
        // Don't dereference a nullptr inside this function! (decltype is ok)
        if constexpr ( std::is_pointer_v<T> )
            return const_cast<decltype(*m_ref) const *>(m_ref);
        else
            return std::addressof(const_cast<T const &>(m_ref));
    }

    attrib_nodiscard auto &operator*(void) noexcept
    {
        if constexpr ( std::is_pointer_v<T> )
            return *m_ref;
        else
            return m_ref;
    }

    attrib_nodiscard auto const &operator*(void) const noexcept
    {
        if constexpr ( std::is_pointer_v<T> )
            return *m_ref;
        else
            return m_ref;
    }
};

#else // just for C++11

template<typename T, class MutexType = std::recursive_mutex>
class Reserved final {

    static_assert( !std::is_reference<T>::value, "T cannot be a reference" );
    static_assert(
        std::is_same<
            MutexType,
            typename std::remove_cv<typename std::remove_reference<MutexType>::type>::type
        >::value , "MutexType must be a bare type (no cvref)" );

    Reserved(void) = delete;
    Reserved(Reserved const &) = delete;
    //Reserved(Reserved &&) = delete; - see below
    Reserved &operator=(Reserved const &) = delete;
    Reserved &operator=(Reserved &&) = delete;
    Reserved const volatile *operator&(void) const volatile = delete;
    template<typename U> void operator,(U&&) = delete;

    T &m_ref;

    std::unique_lock<MutexType> m_locker; // unique_lock is movable

public:
    // Constructor might throw if argM.lock() throws std::system_error
    Reserved(MutexType &argM, T &argR) noexcept( noexcept(argM.lock()) )
      : m_ref(argR), m_locker(argM) {}

    // C++11 doesn't have guaranteed return value optimisation, and so
    // the 'move constructor' on the next line must be accessible (even
    // if it is elided by the compiler). The move-constructor of unique_lock is noexcept.
    Reserved(Reserved &&arg) noexcept : m_ref(arg.m_ref), m_locker( std::move(arg.m_locker) ) {}

    template<bool is_pointer, typename U> class Pointer;

    template<typename U>
    struct Pointer<true , U> {
        static U addr(U &arg) noexcept { return arg; }
        static typename std::remove_pointer<U>::type &obj(U &arg) noexcept { return *arg; }
    };

    template<typename U>
    struct Pointer<false, U> {
        static U *addr(U &arg) noexcept { return std::addressof(arg); }
        static U &obj (U &arg) noexcept { return arg; }
    };

    typename std::conditional<std::is_pointer<T>::value, T, T*>::type operator->(void) noexcept
    {
        return Pointer<std::is_pointer<T>::value, T>::addr(m_ref);
    }

    typename std::conditional<std::is_pointer<T>::value, T, T*>::type operator->(void) const noexcept
    {
        return Pointer<std::is_pointer<T>::value, T>::addr( const_cast<T const &>(m_ref) );
    }

    typename std::conditional<std::is_pointer<T>::value, typename std::remove_pointer<T>::type, T>::type
    &operator*(void) noexcept
    {
         return Pointer<std::is_pointer<T>::value, T>::obj(m_ref);
    }

    typename std::conditional<std::is_pointer<T>::value, typename std::remove_pointer<T>::type, T>::type
    const &operator*(void) const noexcept
    {
         return Pointer<std::is_pointer<T>::value, T>::obj(m_ref);
    }
};

#endif

#if (__cplusplus >= 201907L) && defined(__cpp_concepts)

template<typename T, BasicLockable MutexType = std::recursive_mutex>
class Reservable final {

    // Reservable(void) = delete;  -  see public constructor below
    Reservable(Reservable const &) = delete;
    Reservable(Reservable &&) = delete;
    Reservable &operator=(Reservable const &) = delete;
    Reservable &operator=(Reservable &&) = delete;
    Reservable const volatile *operator&(void) const volatile = delete;
    template<typename U> void operator,(U&&) = delete;

    MutexType mutable m_lock;  // constructor might throw std::system_error
    T m_obj;

public:

    // The constructor on the next line might throw if the constructor
    // of m_lock or m_obj throws
    template<typename... Args>
    explicit Reservable(Args&&... args) noexcept( noexcept(decltype(m_lock)()) && noexcept(T(std::forward<Args>(args)...)))
      : m_obj(std::forward<Args>(args)...) {}

    // The next four methods might throw if m_lock.lock() throws an std::system_error
    attrib_nodiscard Reserved<T> operator->(void) noexcept( noexcept(m_lock.lock()) )
        { return Reserved<T>(m_lock,m_obj); }
    attrib_nodiscard Reserved<T const> operator->(void) const noexcept( noexcept(m_lock.lock()) )
        { return Reserved<T const>(m_lock,m_obj); }

    attrib_nodiscard Reserved<T> Reserve(void) noexcept( noexcept(m_lock.lock()) )
        { return Reserved<T>(m_lock,m_obj); }
    attrib_nodiscard Reserved<T const> Reserve(void) const noexcept( noexcept(m_lock.lock()) )
        { return Reserved<T const>(m_lock,m_obj); }

    attrib_nodiscard Reserved<T> operator*(void) noexcept( noexcept(m_lock.lock()) )
        { return Reserved<T>(m_lock,m_obj); }
    attrib_nodiscard Reserved<T const> operator*(void) const noexcept( noexcept(m_lock.lock()) )
        { return Reserved<T const>(m_lock,m_obj); }

    //T &GetThreadUnsafeAccess(void) { return m_obj; }
};

#else

template<typename T, class MutexType = std::recursive_mutex>
class Reservable final {

    static_assert( !std::is_reference<T>::value, "T cannot be a reference" );
    static_assert(
        std::is_same<
            MutexType,
            typename std::remove_cv<typename std::remove_reference<MutexType>::type>::type
        >::value , "MutexType must be a bare type (no cvref)" );

    // Reservable(void) = delete;  -  see public constructor below
    Reservable(Reservable const &) = delete;
    Reservable(Reservable &&) = delete;
    Reservable &operator=(Reservable const &) = delete;
    Reservable &operator=(Reservable &&) = delete;
    Reservable const volatile *operator&(void) const volatile = delete;
    template<typename U> void operator,(U&&) = delete;

    MutexType mutable m_lock;  // constructor might throw std::system_error
    T m_obj;

public:

    // The constructor on the next line might throw if the constructor
    // of m_lock or m_obj throws
    template<typename... Args>
    explicit Reservable(Args&&... args) noexcept( noexcept(decltype(m_lock)()) && noexcept(T(std::forward<Args>(args)...)))
      : m_obj(std::forward<Args>(args)...) {}

    // The next four methods might throw if m_lock.lock() throws an std::system_error
    attrib_nodiscard Reserved<T> operator->(void) noexcept( noexcept(m_lock.lock()) )
        { return Reserved<T>(m_lock,m_obj); }
    attrib_nodiscard Reserved<T const> operator->(void) const noexcept( noexcept(m_lock.lock()) )
        { return Reserved<T const>(m_lock,m_obj); }

    attrib_nodiscard Reserved<T> Reserve(void) noexcept( noexcept(m_lock.lock()) )
        { return Reserved<T>(m_lock,m_obj); }
    attrib_nodiscard Reserved<T const> Reserve(void) const noexcept( noexcept(m_lock.lock()) )
        { return Reserved<T const>(m_lock,m_obj); }

    attrib_nodiscard Reserved<T> operator*(void) noexcept( noexcept(m_lock.lock()) )
        { return Reserved<T>(m_lock,m_obj); }
    attrib_nodiscard Reserved<T const> operator*(void) const noexcept( noexcept(m_lock.lock()) )
        { return Reserved<T const>(m_lock,m_obj); }

    //T &GetThreadUnsafeAccess(void) { return m_obj; }
};

#endif // check __cplusplus for concepts

#endif // HEADER_INCLUSION_GUARD_RESERVATIONS_MULTITHREADED
