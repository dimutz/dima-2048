# Programarea Calculatoarelor, seria CC
### Tema2 - 2048
#### Dima Alexandru, 312 CC
Jocul a fost implementat in aproximativ 9 zile.
Am pornit de la urmatoarea schema:
## In meniu:
- isIngame este 0;
- isInprogress este 0 daca nu exista un joc deja inceput si 1 in caz contrar;
- quit este semnalul de inchidere al jocului;
- la intrare in joc, isIngame devine 1;
- daca se intra prin optiunea NEW GAME isInprogress devine 0;
- daca se intra prin optiunea RESUME isInprogress ramane 1;
## In joc:
- isIngame este 1;
- daca isInprogress este 0 inseamna ca s-a intrat prin optiunea NEW GAME;
- in caz contrar s-a intrat prin optiunea RESUME;
- la iesirea in meniu, daca jocul s-a incheiat isInprogress devine 0;
- in caz contrar, devine 1;
## Ghid meniu:
- navigarea se face cu sagetile UP/DOWN;
- alegerea optiunii se face cu ENTER;
- optiunea RESUME este disponibila doar daca exista un joc deja inceput;
## Ghid joc:
Asa cum indica si legenda din panoul de control, sunt disponibile 6 optiuni.
- W - mutarea in sus
- A - mutarea la stanga
- S - mutarea in jos
- D - mutarea la dreapta
- U - undo; aduce tabla de joc la cea mai recenta stare
- Q - iesirea in meniu
## Implementare meniu:
- Dupa ce s-a creat fereastra, se afiseaza textul si optiunile.
- Optiunea RESUME apare diferit cand este indisponibila.
- Cu ajutorul unei sageti si variabilei option, se alege optiunea dorita.
- Dupa alegerea optiunii se continua (sau nu) in functie de aceasta
## Implementare joc:
- Dupa ce s-au creat ferestrele, se pregatesc tabla de joc si panoul de control.
- Daca este cazul, tabla de joc este initializata.
- Pentru implementarea culorilor si asocierea cu numerele, am ales
sa lucrez folosind puterile lui 2, iar la afisare se folosesc operatii pe biti.
- Folosind select, se preia input de la tastatura cu timeout la o secunda,
astfel ca data si ora vor fi actualizate constant.
- Prin variabila timer, se retine cat timp a trecut de la ultima mutare facuta
de jucator.
- Inainte de fiecare mutare, se verifica validitatea acesteia astfel -
folosind o copie se verifica posibilele mutari/adunari.
### Pentru mutari, se efectueaza urmatorul procedeu:
- se extrage fiecare linie(LEFT, RIGHT) sau coloana(UP, DOWN) intr-un vector;
- vectorul se muta la stanga incepand cu prima valoare nula;
- se verifica existenta numerelor egale adiacente;
- se aduna unde este cazul si se tine minte(merged) pentru scor;
- se reia procesul pana toate valorile nule sunt la final;
- vectorul se muta inapoi pe tabla de joc.
- La fiecare mutare valida, se memoreaza starea anterioara a jocului,
cat si scorul, pentru optiunea UNDO.
- Daca mutarea este valida si mai exista locuri disponibile, se adauga
un numar nou pe tabla, cu sansa de 10% sa fie 4.
- Dupa fiecare mutare valida se calculeaza scorul, se verifica daca s-a terminat
jocul si se actualizeaza toate informatiile de pe ecran.
- Daca expira timer-ul, se determina mutarea/mutarile care elibereaza cele mai
multe celule si se alege una din ele la intamplare, iar timerul este resetat.
#### Pentru compilare: make
#### Pentru rulare: make run
