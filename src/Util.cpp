#include "Util.h"

QString process_two_dots_in_path(QStringView path) {
    assert(!path.empty());
    assert(path[0] == '/');
    assert(path.back() == '/');

    if (path.size() == 1)
        return path.toString();

    struct {
        bool after_slash = false;
        int dots = 0;
    } state;
    std::vector<QStringView> dirs;
    auto was_never_double_dots = true;
    for (auto from = std::cbegin(path), it = std::cbegin(path), end = std::cend(path); it != end; ++it) {
        const QChar& ch = *it;
        if (ch == '/') {
            if (state.after_slash && state.dots == 2) {
                assert(!dirs.empty());
                dirs.pop_back();
                was_never_double_dots = false;
            } else {
                if (from != it)
                    dirs.emplace_back(QStringView(from, it));
            }
            state.after_slash = true;
            state.dots = 0;
            from = it;
        } else if (ch == '.') {
            ++state.dots;
        } else {
            state.after_slash = false;
        }
    }
    if (was_never_double_dots)
        return path.toString();

    QString ret;
    std::for_each(std::cbegin(dirs), std::cend(dirs), [&ret](const auto& dir) { ret += dir; });
    ret += '/';
    return ret;
}
