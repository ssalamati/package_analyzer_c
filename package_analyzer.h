#ifndef PACKAGE_ANALYZER_H
#define PACKAGE_ANALYZER_H

#include <stdlib.h>
#include <stdio.h>

void get_package_stats(const char* arch, const char* mirror_url, int top_n, int validate_lines);

#endif // PACKAGE_ANALYZER_H
