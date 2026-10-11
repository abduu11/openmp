# TP1 – Programmation parallèle avec OpenMP

Programme en C qui manipule un grand tableau d'entiers aléatoires, en version séquentielle puis parallélisée avec OpenMP (INFO932, Polytech Annecy-Chambéry).

## Opérations réalisées

1. Allocation d'un grand tableau et remplissage avec des entiers aléatoires
2. Recherche du maximum
3. Recherche du minimum
4. Somme des éléments
5. Nombre d'éléments strictement compris entre le minimum et le maximum
6. Tri du tableau (tri par sélection)

Chaque étape est chronométrée séparément. Le tri n'est exécuté que pour N ≤ 200 000, car il est en O(N²).

## Contenu

| Fichier | Description |
|---|---|
| `sequentiel.c` | Version séquentielle |
| `parallele.c` | Version parallèle avec OpenMP (`reduction`, `critical`, `barrier`, `single`) |

## Compilation

```bash
mkdir -p runnable

# version séquentielle
gcc -O2 sequentiel.c -o runnable/sequentiel

# version parallèle (le flag -fopenmp est obligatoire)
gcc -fopenmp -O2 parallele.c -o runnable/parallele
```

## Exécution

L'argument est la taille du tableau (100000 par défaut).

```bash
# séquentiel
./runnable/sequentiel 100000
./runnable/sequentiel 50000000

# parallèle : le nombre de threads se règle avec OMP_NUM_THREADS
OMP_NUM_THREADS=4 ./runnable/parallele 100000

# comparer plusieurs nombres de threads
for t in 1 2 4 8; do OMP_NUM_THREADS=$t ./runnable/parallele 100000; done
for t in 1 2 4 8; do OMP_NUM_THREADS=$t ./runnable/parallele 50000000; done
```

