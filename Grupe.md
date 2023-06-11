# Grupe

### MPI_Comm_group

- Vraća grupu procesa za zadati komunikator

```cpp
MPI_Comm_group(MPI_Comm comm, MPI_Group *group)
```

### MPI_Group_rank

- Vraća identifikator procesa unutar zadate grupe

```cpp
MPI_Group_rank(MPI_Group group, int *rank)
```

### MPI_Group_size

- Vraća broj procesa unutar grupe

```cpp
MPI_Group_size(MPI_Group group, int *size)
```

### MPI_Group_excl

- Skraćeno, kreira novu grupu od stare, samo izbaci određene članove...
- Kreira se nova grupa tako što se iz stare grupe isključe procesi sa
  identifikatorima koji su definisani sa nonmembers, i kojih ima
  count.
- Redosled procesa u novoj grupi prati redosled procesa
  u staroj grupi.

```cpp
MPI_Group_excl(MPI_Group group, int count, int *nonmembers, MPI_Group *new_group)
```

### MPI_Group_incl

- Skraćeno, kreira novu grupu sa odabranim procesima iz stare grupe...
- Kreira se nova grupa tako što procesi sa identifikatorima iz stare
  grupe koji su definisani sa members čine novu grupu.
- Procesa u novoj grupi ima count.
- Proces members[i] u novoj grupi ima rang i.

```cpp
MPI_Group_incl(MPI_Group oldGroup, int count, int *members, MPI_Group *newGroup)
```

### MPI_Group_intersection

- Nova grupa koja se sastoji od procesa koji predstavljaju presek prve i druge prosleđene grupe. Redosled u novoj grupi je isti kao u group1.

```cpp
MPI_Group_intersection(MPI_Group group1, MPI_Group group2, MPI_Group *newGroup)
```

### MPI_Group_union

- Nova grupa koja se sastoji od svih procesa iz group1 i procesa iz group2 koji se ne nalaze u group1.

```cpp
MPI_Group_union(MPI_Group group1, MPI_Group group2, MPI_Group *newGroup)
```

### MPI_Group_difference

- Nova grupa koja se sastoji od procesa iz group1 koji se ne nalaze u group2. Redosled u novoj grupi je isti kao u group1.

```cpp
MPI_Group_difference(MPI_Group group1, MPI_Group group2, MPI_Group *newGroup)
```

# Komunikatori

### MPI_Comm_create

- Kreira novi komunikator, newComm, od procesa iz zadate grupe, koji se nalaze u oldComm komunikatoru.

```cpp
MPI_Comm_create(MPI_Comm oldComm, MPI_Group group, MPI_Comm *newComm)
```

### MPI_Comm_split

- Razdvaja komunikator oldComm na više podkomunikatora tako što svi procesi koji imaju istu vrednost za color pripadaju istom podkomunikatoru.
- Key određuje redosled procesa u podkomuniktorima.
- Ako je key isti za sve procese onda se redosled procesa preuzima iz oldComm.

```cpp
MPI_Comm_split(MPI_Comm oldComm, int color, int key, MPI_Comm *newComm)
```

# Cartesian topologije

### MPI_Cart_create

- Za kreiranje Cartesian struktura proizvoljnog broja dimenzija.

```cpp
MPI_Cart_create(MPI_Comm oldComm, int nDims, int *dimSize, int *periods, int reorder MPI_Comm *commCart)
```

### MPI_Cart_coords

- Preslikavanje ranka procesa u koordinate procesa.

```cpp
MPI_Cart_coords( MPI_Comm comm, int rank, int maxDims, int *coords )
```

### MPI_Cart_rank

- Preslikavanje koordinate procesa u rank procesa.

```cpp
 MPI_Cart_rank (MPI_Comm comm, int *coords, int *rank)
```

### MPI_Cart_shift

- Izračunava rankove susednih procesa, za proces koji je poziva. Ovi rankovi će biti iskorišćeni u pomeranju prilikom poziva funkcija za komunikaciju koji se
  dešavaju nakon poziva ove funkcije.
- Ako za neki proces sused nije definisan onda je povratna vrednost MPI_PROC_NULL.

```cpp
MPI_Cart_shift(MPI_Comm comm, int direction, int disp, int *rankSource, int *rankDest)
```

### MPI_Sendrecv

- Kombinuje blokirajuće Send i Recv u jednu operaciju koja se odvija bez deadloka.
- Proces koje je izvršava šalje max 1 poruku i
  prima max 1 poruku.
- Dest i source mogu biti različiti, ali i isti, po
  potrebi.

### MPI_Sendrecv_replace

- Varijanta MPI_Sendrecv koja koristi isti bafer za slanje i primanje podataka.
- Primljeni podaci u buf se kopiraju na mesta, odakle su poslati podaci iz buf.
