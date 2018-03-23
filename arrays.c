#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "sequences.h"


#define INIT_LMAX 10

sequence* nouvelle_sequence (void)
{
    sequence* seq = (sequence*) malloc (sizeof(sequence));
    assert (seq != NULL);

    seq->LMAX = INIT_LMAX;
    seq->longueur = 0;
    seq->tab = (char*) malloc (INIT_LMAX * sizeof(char));
    assert (seq->tab != NULL);

    return seq;
}

void libere_sequence(sequence* seq)
{
    assert (seq != NULL);
    free (seq->tab);
    free (seq);
}

/* fonction interne pour double la taille du tableau s'il devient trop petit
 * pour la séquence */
void double_taille (sequence* seq)
{
    assert (seq != NULL);
    char* new_tab;

    /* on pourrait utiliser "realloc" ici mais cela "cache" la recopie 
     * des éléments en mémoire
     * */

    new_tab = (char*) malloc (seq->LMAX*2 * sizeof(char));
    assert (new_tab != NULL);

    for (unsigned int i = 0; i<seq->longueur; i++) {
        new_tab[i] = seq->tab[i];
    }
    free (seq->tab);
    seq->tab = new_tab;
    seq->LMAX = seq->LMAX*2;
}

unsigned int longueur(sequence* seq)
{
    assert (seq != NULL);
    return seq->longueur;
}

bool est_vide (sequence *seq)
{
    return (seq->longueur == 0);
}

void affiche(sequence* seq)
{
    unsigned int i;
    assert (seq != NULL);
    for (i=0; i<seq->longueur; i++) {
        printf ("%c", seq->tab[i]);
    }
    printf ("\n");
}


void ajout_debut (sequence* seq, char c)
{
    assert (seq != NULL);
    if (seq->longueur == seq->LMAX) {
        double_taille (seq);
    }
    seq->longueur++;
    for (int i=seq->longueur-1; i>0; i--) {
        seq->tab[i] = seq->tab[i-1];
    }
    seq->tab[0] = c;
}


void ajout_fin   (sequence* seq, char c)
{
    assert (seq != NULL);
    if (seq->longueur == seq->LMAX) {
        double_taille (seq);
    }
    seq->longueur++;
    seq->tab[seq->longueur-1] = c;
}

int recherche (sequence *seq, char c)
{
  unsigned int i = 0;
  for (; i<seq->longueur; i++) {
    if (seq->tab[i] == c) {
      return i;
    }
  }
  return -1;
}
