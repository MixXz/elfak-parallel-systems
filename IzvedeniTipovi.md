## Izvedeni tipovi

### Formira novi tip podataka kopiranjem na uzastopne memorijske lokacije.

```cpp
MPI_Type_contiguous(int count, MPI_Datatype oldtype, MPI_Datatype *newtype);
```

### Omogućava kreiranje novog tipa podatka na osnovu starog tipa podatka, gde se blokovi nalaze na odvojenim memorijskim lokacijama i gde se svaki blok sastoji od istog broja podataka starog tipa.

```cpp
MPI_Type_vector(int count, int blocklength, int stride, MPI_Datatype oldtype, MPI_Datatype *newtype)
```

- **_count_** - broj blokova.
- **_blocklength_** - broj elemenata starog tipa u svakom bloku.
- **_stride_** - broj elemenata starog tipa između početaka dva bloka.

### Omogućava da blokovi budu različitih dužina kao i da razmaci između početaka blokova budu različitih dužina.

```cpp
MPI_Type_indexed(int count, int *array_of_blocklengths, int *array_of_displacements,  MPI_Datatype oldtype, MPI_Datatype *newtype)
```

- **_count_** - broj blokova
- **_array_of_blocklengths_** – niz čiji su elementi brojevi elemenata starog tipa u svakom bloku.
- **_array_of_displacements_** – niz koji sadrži pomeraje za svaki blok izražen u elementima starog tipa.

### Kreira izvedeni tip podatka opisom N-dimenzionalnog podpolja unutar N-dimenzionalnog polja

```cpp
MPI_Type_create_darray(int size, int rank, int ndims, int gsizes[], int distribs[], int dargs[], int psizes[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype)
```

- **_size_** – Broj procesa među kojima je niz distribuiran.
- **_ndims_** – Broj dimenzija polja i podpolja (N)
- **_gsizes_** - Broj elemenata starog tipa (oldtype) u svakoj dimenziji polja (niz pozitivnih celih brojeva)
- **_distribs_** – Način distribucije - MPI_DISTRIBUTE_BLOCK
- **_dargs_** – Parametar distribucije za svaku dimenziju - MPI_DISTRIBUTE_DFLT_DARG
- **_psizes_** – Broj procesa u svakoj dimenziji među kojima je polje distribuirano. Uzima se da grid procesa ima isti broj dimenzija kao polje i podpolje. Ako niz po nekoj dimenziji nije distribuiran, broj procesa u toj dimenziji je 1!
- **_order_** - Način predstavljanja polja u memoriji, ili MPI_ORDER_C ili MPI_ORDER_FORTRAN

### Kreira izvedeni tip podatka definisanjem početka i dimenzija podpolja

```cpp
MPI_Type_create_subarray(int ndims, int *sizes, int *subsizes, int *offsets, int order, MPI_Datatype oldtype, MPI_Datatype *newtype)
```

- **_ndims_** - broj dimenzija polja (N)(pozitivan broj)
- **_sizes_** - broj elemenata starog tipa (oldtype) u svakoj dimenziji polja (niz pozitivnih celih brojeva)
- **_subsizes_** - broj elemenata starog tipa (oldtype) u svakoj dimenziji podpolja(niz
  pozitivnih celih brojeva)
- **_offsets_** - početne koordinate podpolja u svakoj dimenziji(niz nenegativnih brojeva)
- **_order_** - način predstavljanja polja u memoriji, ili MPI_ORDER_C ili MPI_ORDER_FORTRAN
