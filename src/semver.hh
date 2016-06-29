#ifndef TINYTHING_SEMVER_HH
#define TINYTHING_SEMVER_HH
#include <string>
struct SemVer {
    SemVer(const std::string& version) {
        size_t dots[2];
        dots[0] = version.find('.');
        dots[1] = version.find('.', dots[0]+1);
        major = std::stoi(version, nullptr);
        minor = std::stoi(version.substr(dots[0]+1, dots[1]-dots[0]-1), nullptr);
        bugfix = std::stoi(version.substr(dots[1]+1), nullptr);
    }

    /* In today's cool 'wtf gcc' news, somewhere there's a #define gnu_c_major() major() which
       means that trying to use initializer-lists to construct major and minor will give you
       compiler errors. What the fuck
     */
    SemVer(const SemVer& other) {major = other.major; minor = other.minor; bugfix = other.bugfix;}
    SemVer& operator=(const SemVer& other) {major = other.major; minor = other.minor; bugfix = other.bugfix; return *this;}
    SemVer()  {major = -1; minor = -1; bugfix = -1;}
    SemVer(int major_in, int minor_in, int bugfix_in) {
        major = major_in; minor = minor_in; bugfix = bugfix_in;
    }
    int major;
    int minor;
    int bugfix;
};

bool operator>(SemVer a, SemVer b) {
    if (a.major > b.major) {
        return true;
    } else if (a.major < b.major) {
        return false;
    } else {
        if (a.minor > b.minor) {
            return true;
        } else if (a.minor < b.minor) {
            return false;
        } else {
            return a.bugfix > b.bugfix;
        }
    }
}

bool operator==(SemVer a, SemVer b) {
    return a.major == b.major && a.minor == b.minor && a.bugfix == b.bugfix;
}

bool operator<(SemVer a, SemVer b) {
    return !(a>b) && !(a==b);
}

bool operator!=(SemVer a, SemVer b) {
    return !(a==b);
}

bool operator>=(SemVer a, SemVer b) {
    return (a > b) || (a == b);
}

bool operator<=(SemVer a, SemVer b) {
    return (a < b) || (a==b);
}
#endif
