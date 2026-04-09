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

Song* safeRealloc(Song* ptr, int newSize) {
    Song* temp = realloc(ptr, newSize);
    if (!temp) {
        printf("Memory allocation failed!\n");
        free(ptr);
        exit(1);
    }
    return temp;
}



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
    if (!file) {
        printf("Error creating playlist.\n");
        return;
    }

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
        if (strcmp(line, name) != 0)
            fprintf(temp, "%s\n", line);
        else
            found = 1;
    }

    fclose(orig);
    fclose(temp);

    remove("playlists.txt");
    rename("temp.txt", "playlists.txt");

    if (found && remove(name) == 0) {
        printf("Playlist '%s' deleted.\n", name);
    } else {
        printf("Playlist not found.\n");
    }
}

int loadPlaylist(const char* name) {
    FILE* file = fopen(name, "r");
    if (!file) {
        printf("Playlist '%s' not found.\n", name);
        return 0;
    }

    strcpy(currentPlaylist, name);
    free(library);
    library = NULL;
    songCount = 0;

    char line[MAX_LINE];

    while (fgets(line, sizeof(line), file)) {
        library = safeRealloc(library, (songCount + 1) * sizeof(Song));

        sscanf(line, "%99[^,],%99[^,],%d",
               library[songCount].title,
               library[songCount].artist,
               &library[songCount].rating);

        songCount++;
    }

    fclose(file);
    printf("Loaded playlist: %s\n", name);
    return 1;
}

void saveCurrentPlaylist() {
    if (strcmp(currentPlaylist, "") == 0) return;

    FILE* file = fopen(currentPlaylist, "w");
    if (!file) {
        printf("Error saving playlist.\n");
        return;
    }

    for (int i = 0; i < songCount; i++) {
        fprintf(file, "%s,%s,%d\n",
                library[i].title,
                library[i].artist,
                library[i].rating);
    }

    fclose(file);
}



void addSong(const char* title, const char* artist, int rating) {
    library = safeRealloc(library, (songCount + 1) * sizeof(Song));

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
            library = safeRealloc(library, songCount * sizeof(Song));

            printf("Song deleted.\n");
            return;
        }
    }
    printf("Song not found.\n");
}



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
    int choice;
    printf("1. Rating\n2. Artist\n3. Title\nChoose: ");
    scanf("%d", &choice); getchar();

    if (choice == 1)
        qsort(library, songCount, sizeof(Song), compareByRating);
    else if (choice == 2)
        qsort(library, songCount, sizeof(Song), compareByArtist);
    else if (choice == 3)
        qsort(library, songCount, sizeof(Song), compareByTitle);
    else
        printf("Invalid choice\n");
}



void binarySearchSong(const char* title) {
    if (songCount == 0) {
        printf("No songs.\n");
        return;
    }

    qsort(library, songCount, sizeof(Song), compareByTitle);

    int left = 0, right = songCount - 1;

    while (left <= right) {
        int mid = (left + right) / 2;
        int cmp = strcmp(title, library[mid].title);

        if (cmp == 0) {
            printf("Found: %s by %s (%d)\n",
                   library[mid].title,
                   library[mid].artist,
                   library[mid].rating);
            return;
        } else if (cmp < 0)
            right = mid - 1;
        else
            left = mid + 1;
    }

    printf("Not found.\n");
}



void displaySongs() {
    if (songCount == 0) {
        printf("No songs.\n");
        return;
    }

    for (int i = 0; i < songCount; i++) {
        printf("%d. %s - %s (%d)\n",
               i + 1,
               library[i].title,
               library[i].artist,
               library[i].rating);
    }
}



int main() {
    int choice;
    char title[100], artist[100], name[100];
    int rating;

    while (1) {
        printf("\n1.List 2.Create 3.Load 4.Delete Playlist\n");
        printf("5.View 6.Add 7.Delete Song 8.Search 9.Sort 10.Exit\n");
        scanf("%d", &choice); getchar();

        switch (choice) {
            case 1: listPlaylists(); break;
            case 2:
                fgets(name, 100, stdin); strtok(name, "\n");
                createPlaylist(name); break;
            case 3:
                fgets(name, 100, stdin); strtok(name, "\n");
                loadPlaylist(name); break;
            case 4:
                fgets(name, 100, stdin); strtok(name, "\n");
                deletePlaylist(name); break;
            case 5: displaySongs(); break;
            case 6:
                fgets(title, 100, stdin); strtok(title, "\n");
                fgets(artist, 100, stdin); strtok(artist, "\n");
                scanf("%d", &rating); getchar();
                addSong(title, artist, rating); break;
            case 7:
                fgets(title, 100, stdin); strtok(title, "\n");
                deleteSong(title); break;
            case 8:
                fgets(title, 100, stdin); strtok(title, "\n");
                binarySearchSong(title); break;
            case 9: sortSongs(); break;
            case 10:
                saveCurrentPlaylist();
                free(library);
                exit(0);
        }
    }
}
