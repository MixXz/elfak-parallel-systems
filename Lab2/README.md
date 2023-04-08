# Grupe

### MPI_Comm_group

- Vraća grupu procesa za zadati komunikator

```cpp
MPI_Comm_group(MPI_Comm comm, MPI_Group *group);
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

- Kreira se nova grupa tako što se iz stare grupe isključe procesi sa
  identifikatorima koji su definisani sa nonmembers, i kojih ima
  count. Redosled procesa u novoj grupi prati redosled procesa
  u staroj grupi.
- Skraćeno, kreira novu grupu od stare, samo izbaci određene članove...

```cpp
MPI_Group_incl(MPI_Group oldGroup, int count, int *members, MPI_Group *newGroup)
```

### MPI_Group_incl

- Kreira se nova grupa tako što procesi sa identifikatorima iz stare
  grupe koji su definisani sa members čine novu grupu.
  Procesa u novoj grupi ima count. Proces members[i] u novoj
  grupi ima rang i.
- Skraćeno, kreira novu grupu sa odabranim procesima iz stare grupe...

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

- Kreira novi komunikator, newComm, od procesa iz zadatke grupe, koji se nalaze u oldComm komunikatoru.

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
