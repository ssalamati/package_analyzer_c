#include "package_analyzer.h"
#include <stdio.h>
#include <stdlib.h>


// Define constants (top_n and validate_lines for simplicity)
#define TOP_N 10
#define VALIDATE_LINES 0


int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <architecture>\n", argv[0]);
        return 1;
    }

    const char *arch = argv[1];

    // Format the mirror URL with the provided architecture
    char* mirror_url = "http://ftp.uk.debian.org/debian/dists/stable/main/Contents-%s.gz";
    char formatted_url[256];
    snprintf(formatted_url, sizeof(formatted_url), mirror_url, arch);

    get_package_stats(arch, formatted_url, TOP_N, VALIDATE_LINES);

    return 0;
}
