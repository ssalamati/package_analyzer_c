#include "package_analyzer.h"
#include "uthash.h"

#include <curl/curl.h>
#include <string.h>
#include <stdbool.h>
#include <zlib.h>


typedef struct {
    char package_name[256]; // key
    int file_count;         // value
    UT_hash_handle hh;      // hash handle
} PackageStat;

// Write data received from curl to a file
static size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

// Downloads the contents file using CURL
void download_contents_file(const char* url, const char* filepath) {
    CURL *curl = curl_easy_init();

    if (curl) {
        FILE *fp = fopen(filepath, "wb");

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        CURLcode res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);
        fclose(fp);
    }
}

int compare_package_stat(PackageStat *a, PackageStat *b) {
    return (b->file_count - a->file_count);
}

// Parses the downloaded contents file
void parse_contents_file(const char* filepath, int top_n) {
    gzFile file = gzopen(filepath, "rb");
    if (!file) return;

    PackageStat *stats = NULL, *s, *tmp;
    char buffer[2048];

    while (gzgets(file, buffer, sizeof(buffer)) != Z_NULL) {
        char* token = strtok(buffer, " ");
        char* last_token = NULL;

        // Iterate over all tokens to get the last one
        while (token != NULL) {
            last_token = token;
            token = strtok(NULL, " "); 
        }

        // Now, last_token contains the last word in the line
        if (!last_token) continue;

        // Remove newline
        last_token[strcspn(last_token, "\n")] = 0;

        // Tokenize last_token by comma to get individual packages
        char* package_name = strtok(last_token, ",");
        while (package_name != NULL) {
            // Find the last occurrence of '/' and adjust the pointer to get the package name
            char* name_ptr = strrchr(package_name, '/');
            if (name_ptr) {
                package_name = name_ptr + 1;
            }

            HASH_FIND_STR(stats, package_name, s);
            if (s) {
                s->file_count++;
            } else {
                // Add it to the hash table
                s = (PackageStat*)malloc(sizeof(PackageStat));
                strncpy(s->package_name, package_name, sizeof(s->package_name));
                s->file_count = 1;
                HASH_ADD_STR(stats, package_name, s);
            }

            package_name = strtok(NULL, ",");
        }
    }

    gzclose(file);

    // Sort the hash table and print the top_n entries
    HASH_SORT(stats, compare_package_stat);

    int count = 0;
    for (s = stats; s != NULL && count < top_n; s = (PackageStat*)(s->hh.next)) {
        printf("%-40s %d\n", s->package_name, s->file_count);
        count++;
    }

    // Free the hash table
    HASH_ITER(hh, stats, s, tmp) {
        HASH_DEL(stats, s);
        free(s);
    }
}

// Main function to get package stats
void get_package_stats(const char* arch, const char* formatted_url, int top_n, int validate_lines) {
    char filepath[] = "/tmp/debian_contents.gz";

    printf("Downloading %s for architecture %s...\n", formatted_url, arch);
    download_contents_file(formatted_url, filepath);
    printf("Download completed.\n");

    printf("Parsing the contents file...\n");
    parse_contents_file(filepath, top_n);
    printf("Parsing completed.\n");

    printf("Cleaning up the downloaded file...\n");
    remove(filepath);
    printf("Cleaning up completed.\n");
}
