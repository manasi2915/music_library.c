#  Music Library Manager (C)

A command-line based music playlist manager built in C that allows users to create, manage, and organize playlists with persistent storage.

---

##  Features

* Create and delete playlists
* Load and save playlists
* Add and delete songs
* Sort songs by:

  * Rating (High → Low)
  * Artist (A → Z)
  * Title (A → Z)
* Search songs using Binary Search
* Persistent storage using `.csv` files

---

## Concepts Demonstrated

* Dynamic Memory Allocation (`malloc`, `realloc`, `free`)
* File Handling in C
* Structures and Modular Programming
* Sorting using `qsort`
* Binary Search Algorithm
* Memory-safe programming practices

---

##  Technologies Used

* C Programming Language
* Standard Libraries (`stdio.h`, `stdlib.h`, `string.h`)

---

##  Project Structure

```
music-library-manager/
│── music_library.c
│── README.md
│── playlists.txt
```

---

##  How to Run

### Compile:

```bash
gcc music_library.c -o music
```

### Run:

```bash
./music
```

---

## Notes

* Playlists are stored as `.csv` files
* Ensure read/write permissions are enabled
* Binary search requires sorting (handled internally)

---

##Future Improvements

* Convert to Linked List implementation
* Add search by artist
* Improve input validation
* Add GUI interface



