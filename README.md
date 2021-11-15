# Project_exercise1

There is a entry_test.c file to test the project. 
In test, it reads a test.txt file to get some words and test the functions.
Functions are declared in entry_list.h, BK_tree.h and BK_List.h and defined in entry_list.c, BK_tree.c and BK_List.c

1. cd tests (go to directory tests)
2. Compile with command: make
3. Run : ./entry_test
<br>
<br>
<b>struct entry</b><br>
![entry](https://user-images.githubusercontent.com/61864130/141796761-f798c463-6127-4d9c-8a80-da6f30238160.jpg)
<br>
<b>struct entry_list</b><br>
![entry_list](https://user-images.githubusercontent.com/61864130/141796795-779aac62-7520-4feb-8f27-8e3c82aac92c.jpg)
<br>
Συγκεκριμένα, χρησιμοποιήθηκε μία έξτρα δομή η entry_list_node η οποία αντιπροσωπεύει ένα κόμβο λίστας και δέχεται ένα entry.
Η εισαγωγή μίας τέτοιας δομής είναι απαραίτητη καθώς ένα entry αν γίνει κόμβος λίστας και προσθέσουμε μέσα στη δομή του και ένα pointer σε άλλο entry, τότε αν αυτό το entry εισαχθεί σε 2 entry_list, θα χαθεί η σύνδεση των λιστών(πχ η αρχική λίστα και η λίστα με τα results στην lookup_entry_index). Οπότε εισάγουμε τον κόμβο λίστας στην υλοποίηση και επίσης συναρτήσεις όπως η get_first και get_next επιστρέφουν και παίρνουν όρισμα "κόμβο λίστας", προκειμένου η διάσχιση να γίνεται σε Ο(n) και όχι σε Ο(n^2)(καθώς αν παίρνουν entry θα πρέπει κάθε φορά να γίνεται αναζήτηση στη λίστα για να βρεθεί το entry και έπειτα να λάβουμε το επόμενο).<br>
Έξτρα συνάρτηση: entry_list_node_value -> επιστρέφει το entry ενός κόμβου λίστας.
<br>
<b>struct BK_tree</b><br>
![BK_tree](https://user-images.githubusercontent.com/61864130/141796806-70fdc372-8f2b-429b-81ca-5a75dd198dcf.jpg)
<br>

<br>


<b>DestroyFunc</b><br>
Δείκτης σε συναρτηση που χρησιμοποιείται για να καταστρέψει τα στοιχεία μία δομής, πιο συγκεκριμένα όταν δημιουργείται μία entry_list ή ένα BK_tree, μία τέτοια συνάρτηση παιρνιέται σαν όρισμα και αποθηκεύεται στην δομή και όταν έρθει η ώρα για την διαγραφή στοιχείου, τότε αν υπάρχει συνάρτηση τέτοια μες τη δομή και όχι NULL στη θέση της, τότε χρησιμοποιείται για να καταστρέψει το στοιχείο.<br>
Ο σχεδιασμός των struct με αυτό τον τρόπο είναι αναγκαίος καθώς πρέπει η κάθε δομή να είναι ενημερωμένη για το αν θα διαγράψει τα στοιχεία της ή όχι καθώς αν δεν γίνει με αυτό τον τρόπο μπορέι να έχουμε memory leaks ή double free() σενάρια.<br>
<br>

<b>BK_List</b><br>
Η δομή αυτή είναι υλοποιημένη στο .h αρχείο έτσι ώστε όπου χρησιμοποιείται, επειδή είναι βοηθητική δομή, να μπορούν τα υπόλοιπα αρχεία να έχουν πρόσβαση στα δεδομένα της χωρίς έξτρα συναρτήσεις.