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
