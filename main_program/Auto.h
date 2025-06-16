#ifndef INCLUSION_GUARD_6d54dbe3_8a18_4a4c_a7ef_c41521457dd0
#define INCLUSION_GUARD_6d54dbe3_8a18_4a4c_a7ef_c41521457dd0

#ifndef NDEBUG
#    include <cassert>         // assert
#    include <cstdio>          // fprintf(stderr)
#    include <exception>       // exception
#endif

template<class L>
class AtScopeExit_6d54 {
    L &m_lambda;
public:
    explicit AtScopeExit_6d54(L &action) noexcept : m_lambda(action) {}
    ~AtScopeExit_6d54(void) noexcept
    {
        try { m_lambda(); }
#ifdef NDEBUG
        catch(...){}
#else
        catch(std::exception const &e)
        {
            std::fprintf(stderr, "Auto(...) destructor exception what() = %s\n", e.what());
            assert("Auto(...) destructor threw -- check the call stack"==0);
        }
        catch(...)
        {
            assert("Auto(...) destructor threw -- check the call stack"==0);
        }
#endif
    }
};

#define TOKEN_PASTEx(x, y) x ## y
#define TOKEN_PASTE(x, y) TOKEN_PASTEx(x, y)

#define Auto_INTERNAL1_6d54(lname, aname, ...) \
    auto lname = [&](void) noexcept(false) { __VA_ARGS__; }; \
    AtScopeExit_6d54<decltype(lname)> aname(lname);

#define Auto_INTERNAL2_6d54(ctr, ...) \
    Auto_INTERNAL1_6d54(TOKEN_PASTE(Auto_func_, ctr), \
        TOKEN_PASTE(Auto_instance_, ctr), __VA_ARGS__)

#define Auto(...) \
    Auto_INTERNAL2_6d54(__COUNTER__, __VA_ARGS__)

#endif
