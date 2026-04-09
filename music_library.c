#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256
#define MAX_NAME 100

typedef struct {
    char title[100];
    char artist[100];
    int rating;
} Song;

Song* library = NULL;
int songCount = 0;
char currentPlaylist[MAX_NAME] = "";

// ===== Playlist Management =====

void listPlaylists() {
    FILE* file = fopen("playlists.txt", "r");
    if (!file) {
        printf("No playlists found.\n");
        return;
    }

    char name[MAX_NAME];
    printf("Available Playlists:\n");
    while (fgets(name, sizeof(name), file)) {
        strtok(name, "\n");
        printf(" - %s\n", name);
    }
    fclose(file);
}

void createPlaylist(const char* name) {
    FILE* file = fopen("playlists.txt", "a+");
    if (!file) return;

    fprintf(file, "%s\n", name);
    fclose(file);

    FILE* newPlaylist = fopen(name, "w");
    if (newPlaylist) fclose(newPlaylist);
    printf("Playlist '%s' created.\n", name);
}

void deletePlaylist(const char* name) {
    FILE* orig = fopen("playlists.txt", "r");
    if (!orig) {
        printf("No playlists exist.\n");
        return;
    }

    FILE* temp = fopen("temp.txt", "w");
    char line[MAX_NAME];
    int found = 0;

    while (fgets(line, sizeof(line), orig)) {
        strtok(line, "\n");
        if (strcmp(line, name) != 0) {
            fprintf(temp, "%s\n", line);
        } else {
            found = 1;
        }
    }

    fclose(orig);
    fclose(temp);
    remove("playlists.txt");
    rename("temp.txt", "playlists.txt");

    if (remove(name) == 0 && found) {
        printf("Playlist '%s' deleted.\n", name);
        if (strcmp(currentPlaylist, name) == 0) {
            free(library);
            library = NULL;
            songCount = 0;
            currentPlaylist[0] = '\0';
        }
    } else {
        printf("Playlist not found or couldn't delete.\n");
    }
}

int loadPlaylist(const char* name) {
    FILE* file = fopen(name, "r");
    if (!file) {
        printf("Playlist '%s' not found.\n", name);
        return 0;
    }

    strcpy(currentPlaylist, name);
    songCount = 0;
    free(library);
    library = calloc(1, sizeof(Song));

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), file)) {
        library = realloc(library, (songCount + 1) * sizeof(Song));
        sscanf(line, "%99[^,],%99[^,],%d", library[songCount].title, library[songCount].artist, &library[songCount].rating);
        songCount++;
    }
    fclose(file);
    printf("Loaded playlist: %s\n", name);
    return 1;
}

void saveCurrentPlaylist() {
    if (strcmp(currentPlaylist, "") == 0) return;
    FILE* file = fopen(currentPlaylist, "w");
    for (int i = 0; i < songCount; i++) {
        fprintf(file, "%s,%s,%d\n", library[i].title, library[i].artist, library[i].rating);
    }
    fclose(file);
}

// ===== Song Management =====

void addSong(const char* title, const char* artist, int rating) {
    library = realloc(library, (songCount + 1) * sizeof(Song));
    strcpy(library[songCount].title, title);
    strcpy(library[songCount].artist, artist);
    library[songCount].rating = rating;
    songCount++;
}

void deleteSong(const char* title) {
    for (int i = 0; i < songCount; i++) {
        if (strcmp(library[i].title, title) == 0) {
            for (int j = i; j < songCount - 1; j++) {
                library[j] = library[j + 1];
            }
            songCount--;
            library = realloc(library, songCount * sizeof(Song));
            printf("Song \"%s\" deleted.\n", title);
            return;
        }
    }
    printf("Song \"%s\" not found.\n", title);
}

// ===== Sorting Functions =====

int compareByRating(const void* a, const void* b) {
    return ((Song*)b)->rating - ((Song*)a)->rating;
}

int compareByArtist(const void* a, const void* b) {
    return strcmp(((Song*)a)->artist, ((Song*)b)->artist);
}

int compareByTitle(const void* a, const void* b) {
    return strcmp(((Song*)a)->title, ((Song*)b)->title);
}

void sortSongs() {
    int sortChoice;
    printf("\nSort by:\n");
    printf("1. Rating (High to Low)\n");
    printf("2. Artist Name (A-Z)\n");
    printf("3. Song Title (A-Z)\n");
    printf("Choose an option: ");
    scanf("%d", &sortChoice); getchar();

    switch (sortChoice) {
        case 1:
            qsort(library, songCount, sizeof(Song), compareByRating);
            printf("Songs sorted by rating.\n");
            break;
        case 2:
            qsort(library, songCount, sizeof(Song), compareByArtist);
            printf("Songs sorted by artist name.\n");
            break;
        case 3:
            qsort(library, songCount, sizeof(Song), compareByTitle);
            printf("Songs sorted by song title.\n");
            break;
        default:
            printf("Invalid sort option.\n");
    }
}

// ===== Binary Search =====

void binarySearchSong(const char* title) {
    if (songCount == 0) {
        printf("No songs to search.\n");
        return;
    }

    // Must be sorted by title before binary search
    qsort(library, songCount, sizeof(Song), compareByTitle);

    int left = 0, right = songCount - 1;
    while (left <= right) {
        int mid = (left + right) / 2;
        int cmp = strcmp(title, library[mid].title);

        if (cmp == 0) {
            printf("Found: %s by %s (Rating: %d)\n", library[mid].title, library[mid].artist, library[mid].rating);
            return;
        } else if (cmp < 0) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    printf("Song \"%s\" not found.\n", title);
}

// ===== Display =====

void displaySongs() {
    if (songCount == 0) {
        printf("No songs in the playlist.\n");
        return;
    }
    printf("\nPlaylist: %s\n", currentPlaylist);
    for (int i = 0; i < songCount; i++) {
        printf("%d. %s - %s (Rating: %d)\n", i + 1, library[i].title, library[i].artist, library[i].rating);
    }
}

// ===== Main Menu =====

int main() {
    int choice;
    char title[100], artist[100], playlistName[100];
    int rating;

    while (1) {
        printf("\n=== Music Library ===\n");
        if (strcmp(currentPlaylist, "") != 0)
            printf("Current Playlist: %s\n", currentPlaylist);
        else
            printf("No playlist loaded.\n");

        printf("1. List Playlists\n");
        printf("2. Create New Playlist\n");
        printf("3. Load Playlist\n");
        printf("4. Delete Playlist\n");
        printf("5. View Songs\n");
        printf("6. Add Song\n");
        printf("7. Delete Song\n");
        printf("8. Search Song (Binary Search)\n");
        printf("9. Sort Songs\n");
        printf("10. Save & Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice); getchar();

        switch (choice) {
            case 1:
                listPlaylists(); break;
            case 2:
                printf("Enter new playlist name (with .csv): ");
                fgets(playlistName, sizeof(playlistName), stdin); strtok(playlistName, "\n");
                createPlaylist(playlistName); break;
            case 3:
                printf("Enter playlist name to load (with .csv): ");
                fgets(playlistName, sizeof(playlistName), stdin); strtok(playlistName, "\n");
                loadPlaylist(playlistName); break;
            case 4:
                printf("Enter playlist name to delete (with .csv): ");
                fgets(playlistName, sizeof(playlistName), stdin); strtok(playlistName, "\n");
                deletePlaylist(playlistName); break;
            case 5:
                displaySongs(); break;
            case 6:
                if (strcmp(currentPlaylist, "") == 0) { printf("Please load a playlist first.\n"); break; }
                printf("Enter song title: "); fgets(title, sizeof(title), stdin); strtok(title, "\n");
                printf("Enter artist: "); fgets(artist, sizeof(artist), stdin); strtok(artist, "\n");
                printf("Enter rating (1-5): "); scanf("%d", &rating); getchar();
                addSong(title, artist, rating); break;
            case 7:
                if (strcmp(currentPlaylist, "") == 0) { printf("Load a playlist first.\n"); break; }
                printf("Enter title to delete: "); fgets(title, sizeof(title), stdin); strtok(title, "\n");
                deleteSong(title); break;
            case 8:
                if (strcmp(currentPlaylist, "") == 0) { printf("Load a playlist first.\n"); break; }
                printf("Enter title to search: "); fgets(title, sizeof(title), stdin); strtok(title, "\n");
                binarySearchSong(title); break;
            case 9:
                if (strcmp(currentPlaylist, "") == 0) { printf("Load a playlist first.\n"); break; }
                sortSongs(); break;
            case 10:
                saveCurrentPlaylist();
                printf("Playlist saved. Exiting.\n");
                free(library);
                exit(0);
            default:
                printf("Invalid option.\n");
        }
    }
    return 0;
}

