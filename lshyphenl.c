#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

// Structure to store file details
typedef struct {
    char name[256];
    struct stat fileStat;
} FileEntry;

// Function to compare file names for sorting
int compare(const void *a, const void *b) {
    FileEntry *entryA = (FileEntry *)a;
    FileEntry *entryB = (FileEntry *)b;
    return strcmp(entryA->name, entryB->name); // Sort alphabetically
}

// Function to print permissions
void print_permissions(mode_t mode) {
    printf((S_ISDIR(mode)) ? "d" : "-");
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");
    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");
    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");
}

// Function to display file details
void display_file_details(FileEntry *entry) {
    struct stat *fileStat = &entry->fileStat;

    // Print permissions
    print_permissions(fileStat->st_mode);

    // Print number of links
    printf(" %ld", fileStat->st_nlink);

    // Print owner and group names
    struct passwd *pw = getpwuid(fileStat->st_uid);
    struct group *gr = getgrgid(fileStat->st_gid);
    printf(" %s %s", pw->pw_name, gr->gr_name);

    // Print file size
    printf(" %ld", fileStat->st_size);

    // Print last modification time
    char timebuf[80];
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&fileStat->st_mtime));
    printf(" %s", timebuf);

    // Print file name
    printf(" %s\n", entry->name);
}

// Function to list files in a directory with sorting
void list_files(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }

    FileEntry entries[1024];
    int count = 0;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] != '.') { // Skip hidden files
            char fullpath[1024];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

            // Get file status
            if (stat(fullpath, &entries[count].fileStat) == -1) {
                perror("Error getting file status");
                continue;
            }

            // Store file name
            strncpy(entries[count].name, entry->d_name, sizeof(entries[count].name) - 1);
            count++;
        }
    }

    closedir(dir);

    // Sort entries alphabetically
    qsort(entries, count, sizeof(FileEntry), compare);

    // Display sorted file details
    for (int i = 0; i < count; i++) {
        display_file_details(&entries[i]);
    }
}

int main(int argc, char *argv[]) {
    const char *path = ".";
    if (argc > 1) {
        path = argv[1];
    }

    list_files(path);
    return 0;
}
