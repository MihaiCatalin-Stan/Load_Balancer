#315CA Stan Mihai-Catalin


## Introducere generala
	In acest readme va vor fi prezentate conceptele generale ale acestei
teme si informatii cu privire la implementarea functiilor. Mai multe detalii
pot fi regasite in comentariile functiilor regasite in fisierele header ale
acestei teme.


## load_balancer.c
	Primul program 'load_balancer' contine functiile care vor lucra pe
structura de load_balancer si vor prelucra valorile de pe asa-zisul hashring.
Cu functiile incluse aici aduagam si eliminam servere, stocam si verificam
elemente, si eliberam memoria alocata de hashring.

# hash_function_servers/key
	Functii de hash folosite pentru a determina pozitia pe hashring a
serverelor si a cheilor elementelor de adaugat.

# init_load_balancer
	Functie ce declara si initializa o structura de tipul load_balancer,
echivalentul hashring-ului nostru pe care vom stoca serverele.
	Acesta contine un array de pointeri catre servere, numarul de servere
la momentul curent, dar si pointeri catre functiile folosite. De asemenea,
functia returneaza load_balancer-ul alocat si initializat.

# new_sort
	Functie ce primeste un hash si hashring-ul si intoarce un intreg ce
reprezinta pozitia serverului cu cel mai mic hash mai mare decat cel primit
prin parametrii.

# loader_add_server
	Procedura ce adauga un server cu un id primit prin parametru pe
hashring. Daca hashring-ul este gol, se va face hash-ul noului server si se
va stoca in ordinea corespunzatoare.
	In orice alt caz, se vor verifica elementele de pe serverul urmator,
iar elementele de pe acesta vor fi redistribuite conform conditiei data de
consistent hashing (cele cu un hash mai mic decat al serverului nou, vor fi
mutate pe acesta).

# loader_remove_server
	Procedura ce elimina un server de pe hashring pe baza unui id primit,
si ii redistribuie elementele pe serverul urmator. La final, serverul este
golit, iar memoria ocupata de acesta si de elementele sale sunt eliberate.
	De asemenea, toate serverele de dupa acesta vor fi mutate o pozitie
la stanga pe hashring, iar pointerul ultimului element este eliberat.

# loader_store
	Procedura ce stocheaza o pereche cheie-valoare pe un server in hashring
Se foloseste functia de hashing pentru cheie, iar apoi cu ajutorul functiei
de sortare se determina serverul pe care trebuie stocat.
	Se da update variabilei de server_id si se va stoca perechea in server
cu ajutorul procedurii de server_store.

# loader_retrieve
	Functie asemanatoare procedurii anterioare. Se hashuieste cheia, se
determina serverul pe care se afla cu functia de new_sort, se da update la
server_id si se intoarce valoare asociata cheii respective cu ajutorul functiei
de server_retrieve.

# free_load_balancer
	Procedura ce parcurge serverele de pe hashring si elibereaza toate
elementele stocate pe acestea. Ulterior se elibereaza array-ul de pointeri si
hashring-ul.


## server

# init_server_memory
	Functie ce declara si initializeaza o variabila de tip server_memory.
Ulterior cu ajutorul functiei de ht_create se declara si initializeaza
hashtable-ul asociat serverului, mai apoi se seteaza pe 0 id-ul si hash-ul
variabilei de server si se returneaza.

# server_store
	Procedura ce adauga o pereche key-value intr-un server trimis printr-un
pointer ca si parametru functiei. Functia se foloseste de procedura de ht_put
pentru a adauga elementul in hashtable-ul serverului.

# server_retrieve
	Functie ce primeste ca parametrii doi pointeri, unul catre cheia a
carei valoare asociata se doreste a fi returnata si unul catre serverul pe care
aceasta pereche key-value poate fi gasita. Cu ajutorul functiei modificate de
ht_get se intoarce valoarea dorita.

# server_remove
	Procedura ce primeste ca parametrii aceiasi doi pointeri ca functia de
server_retrieve si scoate perechea key-value de pe serverul primit ca parametru
cu ajutorul functiei de ht-remove-entry

# free_server_memory
	Procedura ce elibereaza toate datele din hashtable-ul asociat unui
server primit ca parametru cu ajutorul functiei de ht_free. Ulterior se
elibereaza si pointer-ul asociat serverului.


## hashtables

# compare_function_ints/strings
	Functii de comparatie a hash-urilor primite ca parametrii.

# hash_function_string
	Functie de hash pentru siruri de caractere.
	
# key_val_free_function
	Functie pentru eliberarea unei perechi key-value a unei intrari in
hashtable.

# ht_create
	Functie ce declara si initializeaza un hashtable si toate campurile
asociate pe baza parametrilor. Se ocupa spatiu pentru structura de hashtable_t,
se seteaza size-ul pe 0, numarul de bucket-uri existente. Se aloca array-ul
de bucket-uri, apoi cu functia de ll_create se initializeaza cate o lista
pentru fiecare bucket.
	In final se atribuie o serie de functii necesare unui hashtable:
	1) Functia de hashing
	2) Functia de comparatie
	3) Functia de free a perechilor key-value
	
# ht_has_key
	Functie ce verifica pozitia in vectorul de liste a cheii cautate pe
baza functiei de hash si care apoi parcurge lista in cautarea sa. Cand aceasta
cheie este gasita in lista cautata se returneaza valoarea 1, daca lista nu are
niciun nod al carei chei pastreaza valoarea cautata, se va returna valoarea 0.

# ht_get
	Functie asemanatoare celei de ht-has-key, dar care in loc sa returneze
valorile de 0 si de 1 va returna valoarea asociata cheii in cazul in care
aceasta exista, sau NULL in caz contrar. Initial functia returna intreaga
structura de info, dar ulterior am modificat-o pentru a fi mai utila functiei
de server_retrieve.

# ht_put
	Procedura ce adauga o pereche key-value intr-un hashtable al carui
pointer este primit prin parametrii. Procedura cauta pozitia in hashtable
la care cheia trebuie pusa (pe baza functiei de hash a hashtable-ului).
	In cazul in care nu exista deja o valoarea asociata acelei chei, se va
adauga in bucket-ul corespunzator. In caz contrar, se va gasi nodul ce retine
deja aceasta cheie, i se va realoca campul de value si ii va fi atribuita
noua valoare.

# ht_remove_entry
	Procedura ce cauta o cheie intr-un hashtable al carui pointer este
primit ca parametru si elibereaza intrarea asociata ei din structura. Daca
nu exista o intrare cu cheia dorita se va afisa un mesaj de eroare si se va
iesi din procedura, in caz contrar se va cauta nodul asociat acestei chei si
cu ajutorul functiei de key-val-free-function se va elibera memoria asociata
structurii de info. Mai apoi se va elibera nodul din lista si se va da update
numarului de intrari din hashtable.

# ht_free
	Procedura ce parcurge bucket-urile unui hashtable si elibereaza toata
memoria asociata acesteia (nodurile si data-ul din el). Mai apoi se va elibera
memoria ocupata de pointer-ul catre bucket-uri si de hashtable.

Multumesc mult pentru timpul acordat!

                              ,'"`.,./.
                             ,'        Y',"..
                           ,'           \  | \
                          /              . |  `
                         /               | |   \
            __          .                | |    .
       _   \  `. ---.   |                | j    |
      / `-._\   `Y   \  |                |.     |
     _`.    ``    \   \ |..              '      |,-'""7,....
     l     '-.     . , `|  | , |`. , ,  /,     ,'    '/   ,'_,.-.
     `-..     `-.  : :     |/ `   ' "\,' | _  /          '-'    /___
      \""' __.,.-`.: :        /   /._    l'.,'
       `--,   _.-' `".           /__ `'-.' '         .
       ,---..._,.--"""""""--.__..----,-.'   .  /    .'   ,.--
       |                          ,':| /    | /     ;.,-'--      ,.-
       |     .---.              .'  :|'     |/ ,.-='"-.`"`' _   -.'
       /    \    /               `. :|--.  _L,"---.._        "----'
     ,' `.   \ ,'           _,     `''   ``.-'       `-  -..___,'
    . ,.  .   `   __     .-'  _.-           `.     .__    \
    |. |`        "  ;   !   ,.  |             `.    `.`'---'
    ,| |C\       ` /    | ,' |(]|            -. |-..--`
   /  "'--'       '      /___|__]        `.  `- |`.
  .       ,'                   ,   /       .    `. \
    \                      .,-'  ,'         .     `-.
     x---..`.  -'  __..--'"/"""""  ,-.      |   |   |
    / \--._'-.,.--'     _`-    _. ' /       |     -.|
   ,   .   `-..__ ...--'  _,.-' | `   ,.-.  ;   /  '|
  .  _,'         '"-----""      |    `   | /  ,'    ;
  |-'  .-.    `._               |     `._// ,'     /
 _|    `-'   _,' "`--.._________|        `,'    _ /.
//\   ,-._.'"/\__,.   _,"     /_\__/`. /'.-.'.-/_,`-' 
`-"`"' v'    `"  `-`-"              `-'`-`  `'
