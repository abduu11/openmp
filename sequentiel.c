/*
1.Faites un programme séquentiel en C qui

    1. Alloue un grand tableau et le remplie d'entiers aléatoires
    2. Cherche le maximum
    3. Cherche le minimum
    4. Donne la somme des éléments du tableau
    5. Donne le nombre d'éléments compris entre 2 bornes (min, max)
    6. Tri le tableau (tri sélection)

Compilation : gcc -O2 sequentiel.c -o runnable/sequentiel
Execution   : ./runnable/sequentiel [taille]     (taille par defaut : 100000)
*/

#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// le tri par selection est en O(N^2) : on ne le lance pas au dela de cette taille
#define TAILLE_MAX_TRI 200000

// temps ecoule en secondes (horloge monotone)
double maintenant() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

// fct remplissage du tableau avec des entiers aleatoires
void remplirTableau(int *tab, int taille) {
    for (int i = 0; i < taille; i++) {
        tab[i] = rand() % 1000;
    }
}

// maximum
int maximum(int *tab, int taille) {
    int max = tab[0];
    for (int i = 1; i < taille; i++) {
        if (tab[i] > max) {
            max = tab[i];
        }
    }
    return max;
}

// minimum
int minimum(int *tab, int taille) {
    int min = tab[0];
    for (int i = 1; i < taille; i++) {
        if (tab[i] < min) {
            min = tab[i];
        }
    }
    return min;
}

// somme
long long somme(int *tab, int taille) {
    long long sum = 0;
    for (int i = 0; i < taille; i++) {
        sum += tab[i];
    }
    return sum;
}

// between min and max
int between(int *tab, int taille, int min, int max) {
    int count = 0;
    for (int i = 0; i < taille; i++) {
        if (tab[i] > min && tab[i] < max) {
            count++;
        }
    }
    return count;
}

// tri selection
void triSelection(int *tab, int taille) {
    for (int i = 0; i < taille - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < taille; j++) {
            if (tab[j] < tab[minIndex]) {
                minIndex = j;
            }
        }
        // Échange des éléments
        int temp = tab[i];
        tab[i] = tab[minIndex];
        tab[minIndex] = temp;
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
    double debut = maintenant();
    double t0, t1;

    t0 = maintenant();
    srand(time(NULL));
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
