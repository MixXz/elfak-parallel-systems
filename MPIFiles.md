# MPI I/O

### Osnovne funkcije

- Povratna vrednost svih funkcija je error code.

```cpp
MPI_File_open(MPI_Comm comm, const char *filename, int amode, MPI_Info info, MPI_File *fh)
MPI_File_seek(MPI_File fh, MPI_Offset offset, int whence)
MPI_File_read(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
MPI_File_write(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
MPI_File_close(MPI_File *fh)
```

Modovi otvaranja fajla (moguća kombinacija više njih korišćenjem "|" operatora):

- MPI_MODE_RDONLY
- MPI_MODE_WRONLY
- MPI_MODE_RDWR
- MPI_MODE_CREATE

Za MPI_INFO prosleđujemo MPI_INFO_NULL

Vrednosti za whence parametar seek funckije:

- MPI_SEEK_SET - pointer se postavlja na vrednost offseta od početka fajla.
- MPI_SEEK_CUR - pointer se postavlja na trenutnu poziciju pokazivača + offset.
- MPI_SEEK_END - pointer se postavlja na kraj fajla + offset.

### Funkcije sa eksplicitnim pomerajem

- Funkcijama se direktno prosleđuje offset.

```cpp
MPI_File_read_at(MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
MPI_File_write_at(MPI_File fh, MPI_Offset offset, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
```

### File view

```cpp
MPI_File_set_view(MPI_File fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, const char * datarep, MPI_Info info)
```

- **_disp_** – broj bajtova koji se preskače od početka fajla
- **_etype_** – tip podataka (može biti osnovni ili izvedeni tip). Predstavlja osnovnu jedinicu pristupa podacima. Svi pomeraji se specificiraju u odnosu na broj etype jedinica
- **_filetype_** – tip podatka (može biti osnovni ili izvedeni tip). Mora biti ili istog tipa kao etype, ili izvedeni tip koji se sastoji od više jedinica tipa etype.
- **_datarep_** (data representation) – podrazumevana vrednost native – reprezentacija
  podataka u fajlu je ista kao u memoriji
- **info\*** – MPI_INFO_NULL

### Grupne I/O operacije

```cpp
MPI_File_read_all(MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
MPI_File_write_all(MPI_File fh, const void *buf, int count, MPI_Datatype datatype, MPI_Status *status)
```
