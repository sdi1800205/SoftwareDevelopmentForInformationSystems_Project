///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Deque μέσω Dynamic Array.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include "ADTDeque.h"


// Το αρχικό μέγεθος που δεσμεύουμε
#define DEQUE_MIN_CAPACITY 10

// Ένα DequeNode είναι pointer σε αυτό το struct. (το struct περιέχει μόνο ένα στοιχείο, οπότε θα μπροούσαμε και να το αποφύγουμε, αλλά κάνει τον κώδικα απλούστερο)
struct deque_node {
	Pointer value;				// Η τιμή του κόμβου.
};

// Ενα Deque είναι pointer σε αυτό το struct
struct deque {
	DequeNode array;			// Τα δεδομένα, πίνακας από struct deque_node
	int size;					// Πόσα στοιχεία έχουμε προσθέσει
	int capacity;				// Πόσο χώρο έχουμε δεσμεύσει (το μέγεθος του array).
	int max_key;				// Η θέση του μεγαλύτερου σε αξία key μέσα στον πίνακα(ξεκινάει από την αρχή του πίνακα προς το τέλος)
	int min_key;				// Η θέση του μικρότερου σε αξία key μέσα στον πίνακα(ξεκινάει από το τέλος του πίνακα προς τα μπροστά)
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει ένα στοιχείο του deque.
};


Deque deque_create(int size, DestroyFunc destroy_value) {
	// Δημιουργία του struct
	Deque deque = malloc(sizeof(*deque));

	deque->size = size;
	deque->min_key = 0;							// αρχικοποιούμε την μικρότερη θέση να δείχνει στο μικρότερο κελί του πίνακα
	deque->destroy_value = destroy_value;

	// Δέσμευση μνήμης για τον πίνακα. Αρχικά το deque περιέχει max_key
	// μη-αρχικοποιημένα στοιχεία, αλλά εμείς δεσμεύουμε xώρο για τουλάχιστον
	// DEQUE_MIN_CAPACITY για να αποφύγουμε τα πολλαπλά resizes.
	//
	deque->capacity = size < DEQUE_MIN_CAPACITY ? DEQUE_MIN_CAPACITY : size;
	deque->max_key = deque->size > 0 ? size - 1 : deque->capacity - 1;		// αρχικοποιούμε την μεγαλύτερη θέση να δείχνει ή στο τέλος του size(αν υπάρχει) ή στο τέλος του πίνακα
	deque->array = calloc(deque->capacity, sizeof(*deque->array));		// αρχικοποίηση σε 0 (NULL)

	return deque;
}

int deque_size(Deque deque) {
	return deque->size;
}

Pointer deque_get_at(Deque deque, int pos) {
	assert(pos >= 0 && pos < deque->size);	// LCOV_EXCL_LINE (αγνοούμε το branch από τα coverage reports, είναι δύσκολο να τεστάρουμε το false γιατί θα κρασάρει το test)

	// βρίσκουμε την σωστή θέση του δείκτη pos μέσα στον πίνακα
	pos -= deque->capacity - deque->min_key;
	// αν η καινούργια θέση υπάρχει μέσα στον πίνακα τότε την κρατάμε
	// αλλιώς αν πάει σε αρνητικές τιμές τότε πρέπει να πάμε τόσες θέσεις προς πίσω από την χωρητικότητα όσες λέει η θέση pos
	if (pos < 0)
		pos += deque->capacity;
	return deque->array[pos].value;
}

void deque_set_at(Deque deque, int pos, Pointer value) {
	assert(pos >= 0 && pos < deque->size);	// LCOV_EXCL_LINE

	// βρίσκουμε τη σωστή θέση όπως και στην deque_get_at
	pos -= deque->capacity - deque->min_key;
	if (pos < 0)
		pos += deque->capacity;

	// Αν υπάρχει συνάρτηση destroy_value, την καλούμε για το στοιχείο που αντικαθίσταται
	if (value != deque->array[pos].value && deque->destroy_value != NULL)
		deque->destroy_value(deque->array[pos].value);

	deque->array[pos].value = value;
}

// η συνάρτηση αυτή φτιάχνει ένα καινούργιο πίνακα και του περνάει το καινούργιο στοιχείο αλλά και τα στοιχεία του παλιού πίνακα με τη σωστή σειρά
static void array_insert_realloc(Deque deque, Pointer value, bool insert_first) {
	deque->capacity *= 2;

	DequeNode array = calloc(deque->capacity, sizeof(*array));		// δημιουργούμε έναν νέο πίνακα και τον αρχικοποιούμε σε 0(NULL)

	// θα περάσουμε στον νέο πίνακα το καινούργιο στοιχείο ή στην αρχή ή στο τέλος των υπολοίπων ανάλογα με το
	// ποια insert καλεί την συνάρτηση και μετά τα στοιχεία του παλιού,
	// πρώτα το διάστημα[min_key, capacity - 1], ύστερα το διάστημα [0, max_key - 1], αλλίως αν το min_key
	// βρίσκεται στο 0 τότε θα τα βάλει όλα κατευθείαν

	int k;	// δείκτης του νέου πίνακα
	if (insert_first) {		// αν το στοιχείο πρέπει να εισαχθεί στην αρχή
		array[0].value = value;
		k = 1;
	}
	else {					// αλλιώς πρέπει να εισαχθεί στο τέλος των υπολοίπων στοιχείων
		array[deque->size].value = value;
		k = 0;
	}

	int i = deque->min_key - 1;
	do {
		array[k++].value = deque->array[++i].value;
	} while (i < deque->capacity/2 - 1 && i != deque->max_key);	// αν έχει μπεί και το τελευταίο στοιχείο του παλιού πίνακα ή το max_key τότε σταμάτα
		
	// αν το k δεν έφτασε το size του πίνακα, δηλαδή δεν τον αντέγραψε ολόκληρο, σημαίνει ότι πήρε τα στοιχεία από [min_key,size - 1] και περισσεύουν
	// αυτά στις θέσεις [0,max_key]
	if (k < deque->size)
		for (i = 0; i <= deque->max_key; i++)
			array[k++].value = deque->array[i].value;
	deque->max_key = deque->size;	// ορίζουμε το max_key να δείχνει στο τελευταίο στοιχείο
	deque->min_key = 0;				// ορίζουμε το min_key να δείχνει στο πρώτο κελί του πίνακα

	free(deque->array);				// κάνουμε free τον πίνακα
	deque->array = array;			// κάνουμε τον δείκτη μας να δείχνει στον καινούργιο πίνακα
}

void deque_insert_first(Deque deque, Pointer value) {
	// Μεγαλώνουμε τον πίνακα (αν χρειαστεί), ώστε να χωράει τουλάχιστον max_key στοιχεία
	// Διπλασιάζουμε κάθε φορά το capacity (σημαντικό για την πολυπλοκότητα!)

	// αν ο πίνακας έχει γεμίσει τότε δέσμευσε έξτρα χώρο, βάλε το καινούργιο στοιχείο στην αρχή και βάλε τα στοιχεία του παλιού πίνακα με τη σωστή σειρά
	if (deque->size == deque->capacity)
		array_insert_realloc(deque, value, true);
	// αν ο πίνακας δεν είναι γεμάτος τότε πρόσθεσε το καινούργιο στοιχείο μία θέση αριστερότερα από το ελάχιστο και κάνε αυτό ελάχιστο
	else {
		// διακρίνουμε 2 περιπτώσεις
		if (deque->min_key == 0) {		// να βρίσκεται στο 0 άρα βάζουμε το καινούργιο στο τέλος
			deque->min_key = deque->capacity - 1;
			deque->array[deque->min_key].value = value;
		}
		else {							// να βρίσκεται κάπου αλλόυ μέσα στον πίνακα άρα έχει θέση αριστρερότερα
			deque->min_key--;
			deque->array[deque->min_key].value = value;
		}
	}
	deque->size++;		// αυξάνουμε το μέγεθος του πίνακα
}

void deque_insert_last(Deque deque, Pointer value) {
	// Μεγαλώνουμε τον πίνακα (αν χρειαστεί), ώστε να χωράει τουλάχιστον max_key στοιχεία
	// Διπλασιάζουμε κάθε φορά το capacity (σημαντικό για την πολυπλοκότητα!)

	// αν ο πίνακας έχει γεμίσει τότε δέσμευσε έξτρα χώρο, βάλε τα στοιχεία του παλιού πίνακα με τη σωστή σειρά
	// και βάλε το καινούργιο στοιχείο στο τέλος των υπολοίπων
	if (deque->size == deque->capacity)
		array_insert_realloc(deque, value, false);
	// αν ο πίνακας δεν είναι γεμάτος τότε πρόσθεσε το καινούργιο στοιχείο μία θέση αριστερότερα από το ελάχιστο και κάνε αυτό ελάχιστο
	else {
		// διακρίνουμε 2 περιπτώσεις
		if (deque->max_key == deque->capacity - 1) {		// να βρίσκεται στο τελος άρα βάζουμε το καινούργιο στήν αρχή
			deque->max_key = 0;
			deque->array[0].value = value;
		}
		else {							// να βρίσκεται κάπου αλλόυ μέσα στον πίνακα άρα έχει θέση δεξιότερα
			deque->max_key++;
			deque->array[deque->max_key].value = value;
		}
	}
	deque->size++;		// αυξάνουμε το μέγεθος του πίνακα
}

// αντίστοιχα με την array_insert_realloc
static void array_remove_realloc(Deque deque) {
	deque->capacity /= 2;

	DequeNode array = calloc(deque->capacity, sizeof(*array));		// δημιουργούμε έναν νέο πίνακα και τον αρχικοποιούμε σε 0(NULL)

	int k = 0;
	int i = deque->min_key - 1;
	do {
		array[k++].value = deque->array[++i].value;
	} while (i < deque->capacity*2 - 1 && i != deque->max_key);	// αν έχει μπεί και το τελευταίο στοιχείο του παλιού πίνακα ή το max_key τότε σταμάτα
		
	// αν το k δεν έφτασε το size του πίνακα, δηλαδή δεν τον αντέγραψε ολόκληρο, σημαίνει ότι πήρε τα στοιχεία από [min_key,size - 1] και περισσεύουν
	// αυτά στις θέσεις [0,max_key]
	if (k != deque->size)
		for (i = 0; i <= deque->max_key; i++)
			array[k++].value = deque->array[i].value;
	deque->max_key = deque->size - 1;	// ορίζουμε το max_key να δείχνει στο τελευταίο στοιχείο
	deque->min_key = 0;				// ορίζουμε το min_key να δείχνει στο πρώτο κελί του πίνακα

	free(deque->array);				// κάνουμε free τον πίνακα
	deque->array = array;			// κάνουμε τον δείκτη μας να δείχνει στον καινούργιο πίνακα
}

void deque_remove_first(Deque deque) {
	assert(deque->size != 0);		// LCOV_EXCL_LINE

	// Αν υπάρχει συνάρτηση destroy_value, την καλούμε για το στοιχείο που αφαιρείται
	if (deque->destroy_value != NULL)
		deque->destroy_value(deque->array[deque->min_key].value);

	// Αφαιρούμε στοιχείο οπότε το μέγεθος μειώνεται και το min_key ανεβαίνει θέση
	deque->size--;
	deque->min_key++;
	if (deque->min_key == deque->capacity)	// αν το min_key περάσει έξω από τον πίνακα τότε μεταφέρεται στην αρχή
		deque->min_key = 0;

	// Μικραίνουμε τον πίνακα αν χρειαστεί, ώστε να μην υπάρχει υπερβολική σπατάλη χώρου.
	// Για την πολυπλοκότητα είναι σημαντικό να μειώνουμε το μέγεθος στο μισό, και μόνο
	// αν το capacity είναι τετραπλάσιο του size (δηλαδή το 75% του πίνακα είναι άδειος).
	//
	if (deque->capacity > deque->size * 4 && deque->capacity > 2*DEQUE_MIN_CAPACITY)
		array_remove_realloc(deque);
}

void deque_remove_last(Deque deque) {
	assert(deque->size != 0);		// LCOV_EXCL_LINE

	// Αν υπάρχει συνάρτηση destroy_value, την καλούμε για το στοιχείο που αφαιρείται
	if (deque->destroy_value != NULL)
		deque->destroy_value(deque->array[deque->max_key].value);

	// Αφαιρούμε στοιχείο οπότε το μέγεθος μειώνεται και το max_key κατεβαίνει θέση
	deque->size--;
	deque->max_key--;
	if (deque->max_key < 0)	// αν το max_key περάσει έξω από τον πίνακα τότε μεταφέρεται στο τέλος
		deque->max_key = deque->capacity - 1;

	// Μικραίνουμε τον πίνακα αν χρειαστεί, ώστε να μην υπάρχει υπερβολική σπατάλη χώρου.
	// Για την πολυπλοκότητα είναι σημαντικό να μειώνουμε το μέγεθος στο μισό, και μόνο
	// αν το capacity είναι τετραπλάσιο του size (δηλαδή το 75% του πίνακα είναι άδειος).
	//
	if (deque->capacity > deque->size * 4 && deque->capacity > 2*DEQUE_MIN_CAPACITY)
		array_remove_realloc(deque);
}

Pointer deque_find(Deque deque, Pointer value, CompareFunc compare) {
	assert(deque->size != 0);

	// Διάσχιση του deque
	int i = deque->min_key - 1;
	do {
		if (compare(deque->array[++i].value, value) == 0)
			return deque->array[i].value;		// βρέθηκε
	} while (i < deque->capacity - 1 && i != deque->max_key);	// αν έφτασε και το τελευταίο στοιχείο του πίνακα ή το max_key τότε σταμάτα
	
	if (i != deque->max_key)		// αν το i δεν έφτασε το μέγιστο στοιχείο σημαίνει ότι βρισκόταν μετά από αυτό άρα συνέχισε από την αρχή μέχρι να το βρεις
		for (i = 0; i <= deque->max_key; i++)
			if (compare(deque->array[i].value, value) == 0)
				return deque->array[i].value;		// βρέθηκε

	return NULL;				// δεν υπάρχει
}

DestroyFunc deque_set_destroy_value(Deque deque, DestroyFunc destroy_value) {
	DestroyFunc old = deque->destroy_value;
	deque->destroy_value = destroy_value;
	return old;
}

void deque_destroy(Deque deque) {
	// Αν υπάρχει συνάρτηση destroy_value, την καλούμε για όλα τα στοιχεία
	if (deque->destroy_value != NULL && deque->size != 0) {
		int i = deque->min_key - 1;
		do {
			deque->destroy_value(deque->array[++i].value);
		} while (i < deque->capacity - 1 && i != deque->max_key);	// αν έφτασε και το τελευταίο στοιχείο του πίνακα ή το max_key τότε σταμάτα
		if (deque->min_key != 0)
			for (i = 0; i <= deque->max_key; i++)
				deque->destroy_value(deque->array[++i].value);
	}

	// Πρέπει να κάνουμε free τόσο τον πίνακα όσο και το struct!
	free(deque->array);
	free(deque);			// τελευταίο το deque!
}


// Συναρτήσεις για διάσχιση μέσω node /////////////////////////////////////////////////////

DequeNode deque_first(Deque deque) {
	if (deque->size == 0)
		return DEQUE_EOF;
	else	
		return &deque->array[deque->min_key];
}

DequeNode deque_last(Deque deque) {
	if (deque->size == 0)
		return DEQUE_EOF;
	else
		return &deque->array[deque->max_key];
}

DequeNode deque_next(Deque deque, DequeNode node) {
	if (node == &deque->array[deque->max_key])
		return DEQUE_EOF;
	else
		return node + 1;
}

DequeNode deque_previous(Deque deque, DequeNode node) {
	if (node == &deque->array[deque->min_key])
		return DEQUE_EOF;
	else
		return node - 1;
}

Pointer deque_node_value(Deque deque, DequeNode node) {
	return node->value;
}

DequeNode deque_find_node(Deque deque, Pointer value, CompareFunc compare) {
	assert(deque->size != 0);

	// Διάσχιση του deque
	int i = deque->min_key - 1;
	do {
		if (compare(deque->array[++i].value, value) == 0)
			return &deque->array[i];		// βρέθηκε	
	} while (i < deque->capacity - 1 && i != deque->max_key);	// αν έφτασε και το τελευταίο στοιχείο του πίνακα ή το max_key τότε σταμάτα
	if (deque->min_key != 0)
		for (i = 0; i <= deque->max_key; i++)
			if (compare(deque->array[i].value, value) == 0)
				return &deque->array[i];		// βρέθηκε

	return DEQUE_EOF;				// δεν υπάρχει
}