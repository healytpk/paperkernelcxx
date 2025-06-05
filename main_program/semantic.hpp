#ifndef HEADER_INCLUSION_GUARD_6566546542654262147615168198682544948
#define HEADER_INCLUSION_GUARD_6566546542654262147615168198682544948

#include <functional>
#include <optional>
#include <string_view>
#include <xapian.h>

class SemanticSearcher {
protected:
    int fd;
    std::optional< Xapian::Database > db;

public:
    void Init( std::function<void(unsigned,unsigned)> SetProgress ) noexcept(false);
    void Search(std::string_view);
};

#endif
