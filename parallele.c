/*
1.Faites un programme séquentiel en C qui

    1. Alloue un grand tableau et le remplie d'entiers aléatoires
    2. Cherche le maximum
    3. Cherche le minimum
    4. Donne la somme des éléments du tableau
    5. Donne le nombre d'éléments compris entre 2 bornes (min, max)
    6. Tri le tableau (tri sélection)

2.Reprenez votre programme et rendez le parallèle avec OpenMP

Compilation : gcc -fopenmp -O2 parallele.c -o runnable/parallele
Execution   : OMP_NUM_THREADS=4 ./runnable/parallele [taille]   (taille par defaut : 100000)
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

// le tri par selection est en O(N^2) : on ne le lance pas au dela de cette taille
#define TAILLE_MAX_TRI 200000

// temps ecoule en secondes (horloge murale OpenMP)
double maintenant() {
    return omp_get_wtime();
}

// fct remplissage du tableau avec des entiers aleatoires
// rand() n'est pas thread-safe : on utilise rand_r() avec une graine par thread
void remplirTableau(int *tab, int taille) {
    unsigned int base = (unsigned int)time(NULL);
    #pragma omp parallel
    {
        unsigned int seed = base ^ (unsigned int)omp_get_thread_num();
        #pragma omp for
        for (int i = 0; i < taille; i++) {
            tab[i] = rand_r(&seed) % 1000;
        }
    }
}

// maximum
// chaque thread calcule son max local, OpenMP combine les max locaux (reduction)
int maximum(int *tab, int taille) {
    int max = tab[0];
    #pragma omp parallel for reduction(max:max)
    for (int i = 1; i < taille; i++) {
        if (tab[i] > max) {
            max = tab[i];
        }
    }
    return max;
}

// minimum
// meme principe que le maximum avec reduction(min:min)
int minimum(int *tab, int taille) {
    int min = tab[0];
    #pragma omp parallel for reduction(min:min)
    for (int i = 1; i < taille; i++) {
        if (tab[i] < min) {
            min = tab[i];
        }
    }
    return min;
}

// somme
// chaque thread additionne sa part, OpenMP additionne les sommes partielles
long long somme(int *tab, int taille) {
    long long sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < taille; i++) {
        sum += tab[i];
    }
    return sum;
}

// between min and max
// chaque thread compte dans sa part, puis les compteurs sont additionnes
int between(int *tab, int taille, int min, int max) {
    int count = 0;
    #pragma omp parallel for reduction(+:count)
    for (int i = 0; i < taille; i++) {
        if (tab[i] > min && tab[i] < max) {
            count++;
        }
    }
    return count;
}

// tri selection
// la boucle externe est sequentielle (chaque iteration depend de la precedente)
// on parallelise la recherche du minimum de la boucle interne
void triSelection(int *tab, int taille) {
    int minIndex = 0;
    int minValue = 0;

    #pragma omp parallel
    {
        for (int i = 0; i < taille - 1; i++) {

            // un seul thread initialise le minimum partage (barriere implicite a la fin)
            #pragma omp single
            {
                minIndex = i;
                minValue = tab[i];
            }

            // chaque thread cherche le minimum dans sa part de [i+1, taille[
            int localIndex = i;
            int localValue = tab[i];
            #pragma omp for nowait
            for (int j = i + 1; j < taille; j++) {
                if (tab[j] < localValue) {
                    localValue = tab[j];
                    localIndex = j;
                }
            }

            // fusion des minimums locaux dans le minimum partage
            #pragma omp critical
            {
                if (localValue < minValue) {
                    minValue = localValue;
                    minIndex = localIndex;
                }
            }

            // on attend que tous les threads aient fusionne leur resultat
            #pragma omp barrier

            // Échange des éléments (un seul thread, barriere implicite a la fin)
            #pragma omp single
            {
                int temp = tab[i];
                tab[i] = tab[minIndex];
                tab[minIndex] = temp;
            }
        }
    }
}

// afficher tableau
void afficherTableau(int *tab, int taille) {
    for (int i = 0; i < taille; i++) {
        printf("%d ", tab[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    int taille = (argc > 1) ? atoi(argv[1]) : 100000;

    // allocation du tableau
    int *tableau = malloc((size_t)taille * sizeof(int));
    if (tableau == NULL) {
        perror("malloc");
        return 1;
    }

    printf("Taille du tableau: %d\n", taille);
    printf("Nombre de threads: %d\n", omp_get_max_threads());
    double debut = maintenant();
    double t0, t1;

    t0 = maintenant();
    remplirTableau(tableau, taille);
    t1 = maintenant();
    printf("Temps remplissage : %f s\n", t1 - t0);

    t0 = maintenant();
    int max = maximum(tableau, taille);
    t1 = maintenant();
    printf("Maximum: %d\n", max);
    printf("Temps maximum     : %f s\n", t1 - t0);

    t0 = maintenant();
    int min = minimum(tableau, taille);
    t1 = maintenant();
    printf("Minimum: %d\n", min);
    printf("Temps minimum     : %f s\n", t1 - t0);

    t0 = maintenant();
    long long sum = somme(tableau, taille);
    t1 = maintenant();
    printf("Somme: %lld\n", sum);
    printf("Temps somme       : %f s\n", t1 - t0);

    t0 = maintenant();
    int nb = between(tableau, taille, min, max);
    t1 = maintenant();
    printf("Nombre d'éléments entre %d et %d: %d\n", min, max, nb);
    printf("Temps between     : %f s\n", t1 - t0);

    if (taille <= TAILLE_MAX_TRI) {
        t0 = maintenant();
        triSelection(tableau, taille);
        t1 = maintenant();
        printf("Temps tri         : %f s\n", t1 - t0);
    } else {
        printf("Tri ignoré (taille > %d)\n", TAILLE_MAX_TRI);
    }
    //afficherTableau(tableau, taille);

    double fin = maintenant();
    printf("Durée totale: %f seconds\n", fin - debut);

    free(tableau);
    return 0;
}
