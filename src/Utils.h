#ifndef MY_UTILS_H
#define MY_UTILS_H

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>



std::vector<std::string> split(const std::string &s, char delim)
{
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        if(!item.empty())
            elems.push_back(item);
    }
    return elems;
}

bool dirExists(const char *path)
{
    struct stat info;

    if(stat( path, &info ) != 0)
        return false;
    else if(info.st_mode & S_IFDIR)
        return true;
    else
        return false;
}

int createDir(const char *path)
{
    int nError = 0;
    if(!dirExists(path))
    {
#if defined(_WIN32)
        nError = _mkdir(path); // this is for Windows
#else
        nError = mkdir(path,0777); // for Unix-like
#endif
    }
    return nError;
}

bool endsWith(std::string str1, std::string str2)
{
    if (str1.length() >= str2.length()) {
        return (str1.compare (str1.length() - str2.length(), str2.length(), str2) == 0);
    } else {
        return false;
    }
}

#endif  //MY_UTILS_H
