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
    SemVer(const SemVer&& other) {major = other.major; minor = other.minor; bugfix = other.bugfix;}
    SemVer& operator=(const SemVer&& other) {major = other.major; minor = other.minor; bugfix = other.bugfix; return *this;}
    SemVer()  {major = -1; minor = -1; bugfix = -1;}
    int major;
    int minor;
    int bugfix;
};
#endif
