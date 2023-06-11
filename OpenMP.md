# Open MP

### Runtime library funkcije

```cpp
omp_set_num_threads(num);    //setuje broj niti koji se pokreće.
omp_get_num_threads();       //vraća broj niti.
omp_get_thread_num();        //vraća ID niti u timu.
omp_get_wtime();             //vraća double vrednost koja predstavlja trenutno vreme.
```

#### Direktiva parallel se koristi za kreiranje tima niti.

```cpp
#pragma omp parallel {
    blok_naredbi
}
```

#### Može imati sledeće odredbe:

- **if (logički izraz)** - ako uslov u if odredbi nije ispunjen kod se izvršava sekvencijalno
- **private (lista_promenljivih)** - služi za navođenje privatnih promenljivih za svaku nit. Svaka nit ima svoju kopiju privatne promenljive koja nije inicijalizovana na ulasku u paralelni region.
- **firstprivate (lista_promenljivih)** - -ima isto značenje kao private samo što se promenljive inicijalizuju vrednostima koje su imale pre ulaska u paralelnu oblast.
- **shared (lista_promenljivih)** - promenljive koje se navedu kao argumenti ove odredbe, zajedničke su za sve niti u timu.
- **default (shared | none)** - ništa spec.
- **num_threads (celobrojni_izraz)** - postavlja broj niti za izvršenje u paralelnom bloku. Radi isto što i omp_set_num_threads(broj niti).
- **reduction (op: lista_promenljivih)** - vrši redukciju skalarne promenljive koja se javlja u listi promenljivih operatorom op. Kreira se privatna kopija za svaku promenljivu
  koja se nalazi u listi_promenljivih, po jedna za svaku nit, i inicijalizuje u zavisnosti od operatora op(npr. 0 for “+”). **operatori redukcije(+, \*, &, |, ^, &&, ||)**

### Direktive za podelu poslova

- **for** - za distribuciju iteracija for petlje između niti.
- **sections** - za distribuciju nezavisnih radnih jedinica između niti.
- **single** - označava da će taj deo koda izvršavati samo jedna nit u timu.

### Direktiva for

Može imati sledeće odredbe:

- **private(lista_promenljivih)**
- **firstprivate(lista_promenljivih)**
- **lastprivate(lista_promenljivih)** - po izlasku iz paralelne oblasti izvršenja programa, vrednost koju promenljiva ima na kraju poslednje iteracije petlje dodeljuje originalnoj promenljivoj.
- **reduction(operator: lista_promenljivih)**
- **ordered**
- **schedule (kind[, chunk_size])** - specificira kako se iteracije u petlji dele između niti u timu. Ispravnost programa ne sme zavisiti od redosleda izvršavanja niti. **DETALJNO NA PREZENTACIJI**
- **nowait**
