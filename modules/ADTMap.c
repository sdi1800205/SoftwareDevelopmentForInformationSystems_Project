/////////////////////////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Map μέσω Hash Table με open addressing (linear probing)
//
/////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include "ADTMap.h"
#include "ADTSet.h"


// Οι κόμβοι του map στην υλοποίηση με hash table, μπορούν να είναι σε 3 διαφορετικές καταστάσεις,
// ώστε αν διαγράψουμε κάποιον κόμβο, αυτός να μην είναι empty, ώστε να μην επηρεάζεται η αναζήτηση
// αλλά ούτε occupied, ώστε η εισαγωγή να μπορεί να το κάνει overwrite.
typedef enum {
	EMPTY, OCCUPIED, DELETED
} State;

// Το μέγεθος του Hash Table ιδανικά θέλουμε να είναι πρώτος αριθμός σύμφωνα με την θεωρία.
// Η παρακάτω λίστα περιέχει πρώτους οι οποίοι έχουν αποδεδιγμένα καλή συμπεριφορά ως μεγέθη.
// Κάθε re-hash θα γίνεται βάσει αυτής της λίστας. Αν χρειάζονται παραπάνω απο 1610612741 στοχεία, τότε σε καθε rehash διπλασιάζουμε το μέγεθος.
int prime_sizes[] = {53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613, 393241,
	786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741};

// Χρησιμοποιούμε open addressing, οπότε σύμφωνα με την θεωρία, πρέπει πάντα να διατηρούμε
// τον load factor του  hash table μικρότερο ή ίσο του 0.5, για να έχουμε αποδoτικές πράξεις
#define MAX_LOAD_FACTOR 0.5

// Η δομή η οποία περιέχει τα ζευγάρια key/value και εισάγονται στα set
typedef struct pair {
	Pointer key;
	Pointer value;
	Map owner;
}*Pair;

// Δομή του κάθε κόμβου που έχει το hash table (με το οποίο υλοιποιούμε το map)
struct map_node{
	Set set;		// Το Set που έχει κάθε κελί
};

// Δομή του Map (περιέχει όλες τις πληροφορίες που χρεαζόμαστε για το HashTable)
struct map {
	MapNode array;				// Ο πίνακας που θα χρησιμοποιήσουμε για το map (remember, φτιάχνουμε ένα hash table)
	int capacity;				// Πόσο χώρο έχουμε δεσμεύσει.
	int size;					// Πόσα στοιχεία έχουμε προσθέσει
	CompareFunc compare;		// Συνάρτηση για σύγκρηση δεικτών, που πρέπει να δίνεται απο τον χρήστη
	HashFunc hash_function;		// Συνάρτηση για να παίρνουμε το hash code του κάθε αντικειμένου.
	DestroyFunc destroy_key;	// Συναρτήσεις που καλούνται όταν διαγράφουμε έναν κόμβο απο το map.
	DestroyFunc destroy_value;
};

static int compare_pairs(Pair a, Pair b) {
	return a->owner->compare(a->key, b->key);
}

// Συνάρτηση που καταστρέφει ένα Pair
static void destroy_pair(Pair pair) {
	if (pair->owner->destroy_key != NULL)
		pair->owner->destroy_key(pair->key);

	if (pair->owner->destroy_value != NULL)
		pair->owner->destroy_value(pair->value);

	free(pair);
}


Map map_create(CompareFunc compare, DestroyFunc destroy_key, DestroyFunc destroy_value) {
	// Δεσμεύουμε κατάλληλα τον χώρο που χρειαζόμαστε για το hash table
	Map map = malloc(sizeof(*map));
	map->capacity = prime_sizes[0];
	map->array = malloc(map->capacity * sizeof(struct map_node));
	map->size = 0;
	map->compare = compare;
	map->destroy_key = destroy_key;
	map->destroy_value = destroy_value;

	return map;
}

// Επιστρέφει τον αριθμό των entries του map σε μία χρονική στιγμή.
int map_size(Map map) {
	return map->size;
}

// Συνάρτηση για την επέκταση του Hash Table σε περίπτωση που ο load factor μεγαλώσει πολύ.
static void rehash(Map map) {
	// Αποθήκευση των παλιών δεδομένων
	int old_capacity = map->capacity;
	MapNode old_array = map->array;

	// Βρίσκουμε τη νέα χωρητικότητα, διασχίζοντας τη λίστα των πρώτων ώστε να βρούμε τον επόμενο. 
	int prime_no = sizeof(prime_sizes) / sizeof(int);	// το μέγεθος του πίνακα
	for (int i = 0; i < prime_no; i++) {					// LCOV_EXCL_LINE
		if (prime_sizes[i] > old_capacity) {
			map->capacity = prime_sizes[i]; 
			break;
		}
	}
	// Αν έχουμε εξαντλήσει όλους τους πρώτους, διπλασιάζουμε
	if (map->capacity == old_capacity)					// LCOV_EXCL_LINE
		map->capacity *= 2;								// LCOV_EXCL_LINE

	// Δημιουργούμε ένα μεγαλύτερο hash table
	map->array = malloc(map->capacity * sizeof(struct map_node));

	// Τοποθετούμε ΜΟΝΟ τα entries που όντως περιέχουν ένα στοιχείο (το rehash είναι και μία ευκαιρία να ξεφορτωθούμε τα deleted nodes)
	map->size = 0;
	for (int i = 0; i < old_capacity; i++) {
	 	if (old_array[i].set != NULL) {
			for (SetNode node = set_first(old_array[i].set); node != SET_EOF; node = set_next(old_array[i].set, node)) {
				Pair pair = set_node_value(old_array[i].set, node);
				map_insert(map, pair->key, pair->value);
			}
			//Αποδεσμεύουμε το παλιό set χωρίς να διαγράψουμε τα παλιά στοιχεία που έγιναν copy στον καινούργιο πίνακα ώστε να μήν έχουμε leaks
			set_set_destroy_value(old_array[i].set, (DestroyFunc)free);		// στέλνουμε την free και όχι την destroy_pair για να αποδεσμεύσει το Pair αλλά όχι και τους δείκτες μέσα του
			set_destroy(old_array[i].set);
			old_array[i].set = NULL;
		}
	}
	//Αποδεσμεύουμε τον παλιό πίνακα ώστε να μήν έχουμε leaks
	free(old_array);
}

// Εισαγωγή στο hash table του ζευγαριού (key, item).

void map_insert(Map map, Pointer key, Pointer value) {
	// βρίσκουμε τη θέση μέσα στο Hash Table και εισάγουμε το καινούργιο στοιχείο στο αντίστοιχο set
	uint pos = map->hash_function(key) % map->capacity;
	
	// αν δεν υπάρχει στοιχείο στη θέση pos του πίνακα τότε δημιουργεί ένα set για να εισαχθεί το καινούργιο στοιχείο
	if (map->array[pos].set == NULL)
		map->array[pos].set = set_create((CompareFunc)compare_pairs, (DestroyFunc)destroy_pair);

	// δημιουργούμε το καινούργιο ζευγάρι τιμών που θα εισαχθεί στο set
	Pair pair = malloc(sizeof(*pair));
	pair->key = key;
	pair->value = value;
	pair->owner = map;

	// Κρατάμε το παλιό size του set για να ξέρουμε μετά αν αυξήθηκε το set ή αντικαταστάθηκε μία τιμή
	int old_size = set_size(map->array[pos].set);

	// Το εισάγουμε στο set
	set_insert(map->array[pos].set, pair);

	// Παίρνουμε το καινούργιο size
	int new_size = set_size(map->array[pos].set);

	// αυξάνουμε το size του map, αν αυξήθηκε το set
	map->size = ((old_size - new_size) == 0) ?	map->size : map->size + 1;

	// Αν με την νέα εισαγωγή ξεπερνάμε το μέγιστο load factor, πρέπει να κάνουμε rehash
	float load_factor = (float)map->size / map->capacity;
	if (load_factor > MAX_LOAD_FACTOR)
		rehash(map);
}

// Διαργραφή απο το Hash Table του κλειδιού με τιμή key
bool map_remove(Map map, Pointer key) {
	MapNode mnode = map_find_node(map, key);
	if (mnode == MAP_EOF)
		return false;

	// destroy
	uint pos = map->hash_function(key) % map->capacity;		// βρίσκουμε το κελί/set από το οποίο βρήκε το ζευγάρι
	set_remove(map->array[pos].set, (Pair)mnode);					// και το αφαιρούμε

	// μειώνουμε το size του map
	map->size--;

	return true;
}

// Αναζήτηση στο map, με σκοπό να επιστραφεί το value του κλειδιού που περνάμε σαν όρισμα.

Pointer map_find(Map map, Pointer key) {
	MapNode mnode = map_find_node(map, key);
	if (mnode != MAP_EOF) {
		uint pos = map->hash_function(key) % map->capacity;		// βρίσκουμε το κελί/set από το οποίο βρήκε το ζευγάρι
		SetNode snode = set_find_node(map->array[pos].set, (Pair)mnode);
		if (snode == SET_EOF)
			return NULL;
		Pair pair = set_node_value(map->array[pos].set, snode);
		return pair->value;
	}
	else
		return NULL;
}


DestroyFunc map_set_destroy_key(Map map, DestroyFunc destroy_key) {
	DestroyFunc old = map->destroy_key;
	map->destroy_key = destroy_key;
	return old;
}

DestroyFunc map_set_destroy_value(Map map, DestroyFunc destroy_value) {
	DestroyFunc old = map->destroy_value;
	map->destroy_value = destroy_value;
	return old;
}

// Απελευθέρωση μνήμης που δεσμεύει το map
void map_destroy(Map map) {
	for (int i = 0; i < map->capacity; i++)
		if (map->array[i].set != NULL) {
			set_destroy(map->array[i].set);
			map->array[i].set = NULL;
		}

	free(map->array);
	free(map);
}

/////////////////////// Διάσχιση του map μέσω κόμβων ///////////////////////////
// Οι συναρτήσεις αυτές αντί για MapNode επιστρέφουν Pair

MapNode map_first(Map map) {
	//Ξεκινάμε την επανάληψή μας απο το 1ο στοιχείο, μέχρι να βρούμε κάτι όντως τοποθετημένο
	for (int i = 0; i < map->capacity; i++)
		if (map->array[i].set != NULL) {
			SetNode snode = set_first(map->array[i].set);
			if (snode == SET_EOF)
				return MAP_EOF;
			return (MapNode)set_node_value(map->array[i].set, snode);
		}

	return MAP_EOF;
}

MapNode map_next(Map map, MapNode node) {
	uint pos = map->hash_function(((Pair)node)->key) % map->capacity;	// βρίσκουμε σε ποιο set/κελί του πίνακα βρίσκεται το ζευγάρι
	
	SetNode snode = set_find_node(map->array[pos].set, (Pair)node);		// βρίσκουμε τον κόμβο
	snode = set_next(map->array[pos].set, snode);						// παίρνουμε τον επόμενο, αν υπάρχει
	if (snode != SET_EOF)
		return (MapNode)set_node_value(map->array[pos].set, snode);

	// αν δεν υπάρχει επόμενο ζευγάρι στο set τότε πάρε το 1ο στοιχείο του επόμενου κελιού/set του πίνακα
	while (++pos < map->capacity) {
		if (map->array[pos].set != NULL) {
			SetNode snode = set_first(map->array[pos].set);
			if (snode == SET_EOF)
				return MAP_EOF;
			return (MapNode)set_node_value(map->array[pos].set, snode);
		}
	}

	return MAP_EOF;
}

Pointer map_node_key(Map map, MapNode node) {
	return ((Pair)node)->key;
}

Pointer map_node_value(Map map, MapNode node) {
	return ((Pair)node)->value;
}

MapNode map_find_node(Map map, Pointer key) {
	// βρίσκουμε τη θέση του πίνακα που κάνει hash το key
	uint pos = map->hash_function(key) % map->capacity;

	if (map->array[pos].set != NULL) {
		struct pair pair = {key, NULL, map};
		SetNode snode = set_find_node(map->array[pos].set, &pair);
		if (snode == SET_EOF)
			return MAP_EOF;
		return (MapNode)set_node_value(map->array[pos].set, snode);
	}

	return MAP_EOF;
}

// Αρχικοποίηση της συνάρτησης κατακερματισμού του συγκεκριμένου map.
void map_set_hash_function(Map map, HashFunc func) {
	map->hash_function = func;
}

uint hash_string(Pointer value) {
	// djb2 hash function, απλή, γρήγορη, και σε γενικές γραμμές αποδοτική
    uint hash = 5381;
    for (char* s = value; *s != '\0'; s++)
		hash = (hash << 5) + hash + *s;			// hash = (hash * 33) + *s. Το foo << 5 είναι γρηγορότερη εκδοχή του foo * 32.
    return hash;
}

uint hash_int(Pointer value) {
	return *(int*)value;
}

uint hash_pointer(Pointer value) {
	return (size_t)value;				// cast σε sizt_t, που έχει το ίδιο μήκος με έναν pointer
}