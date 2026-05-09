SISTEME DE OPERARE 
<div align="center">
Cum am folosit AI-ul la proiect(Partea 1)
</div>

**Student**:Vlonga Nicoleta-Dorina  
**Grupa**:C.3.2  
**Proiect**:City Manager

FAZA 1

* 1. Instrument utilizat  
    Am folosit Gemini pentru a ma asista la implementarea modulului de filtrare(--filter)

* 2. Prompt-uri utilizate pentru filtrare  
    Pentru a dezolta logica de filtrare, i-am cerut ajutorul asa:  
     ->"Cum pot parsa eficient un argument de forma field:operator: value in C, folosind sscanf, pentru a extrage campurile separat?"  
     ->"Ajuta-ma sa implementez functia de comparare(match_condition) care trebuie sa verifice datele dintr-o structura in functie de operatori primiti ca string si explica-mi-o clar"  
     ->"Cum pot sa convertesc un string care reprezinta o data pentru a putea face comparari matematice?"

* 3. Ce am modificat eu la codul generat  
     -> AI-ul compara datele ca niste numere simple, asa ca am adaugat atoll() pentru a transforma string-urile in time_t, altfel nu ar fi filtrat corect rapoartele  
     -> Am adaugat strcmp() pentru string-uri  
     -> Am scris bucla de citire din fisier si afisarea rezultatelor pentru a arata cat mai ordonat  
     -> Am sugerat implementarea functiei generate_comparison() pentru a evita folosirea repetitiva a aceleasi bucati de cod  

* 4. Ce am invatat din asta  
     ->desi Ai-ul te ajuta cu un punct de plecare, daca nu esti atent la detalii, programul poate genera rezultate complet gresite  
     ->cum sa parsez string-uri intr-o singura linie, ceea ce ma ajuta din punct de vedere al timpului, altfel as fi stat sa scriu cod pentru separarea cuvintelor si as fi putut avea mai multe linii de cod care puteau genera erori


FAZA 2

* 1. Instrument utilizat
     Am folosit tot Gemini-ul pentru a ma ajuta la crearea programului monitor_reports.c(pentru a-mi explica functiile specifice unix necesare fisierului) si la implementarea comunicarii prin semnale intre el si city_manager.c
* 2. Prompt-uri
     Nu intelegeam exact cum trebuie sa leg .c-urile intre ele si cum sa incep cu crearea monitor_reports.c
     ->"Ce trebuie sa contina mai exact programul monitor_reports si care e ordinea corecta a operatiilor(scriere pid, setare semnale, asteptare)?"
     ->"Explica-mi ce face functia sigaction si cum ar trebui integrata"
     ->"Cum pot sa leg .c-urile intre ele ca city_manager sa stie cand si unde sa trimita semnale catre monitor_reports?"

* 3. Ce am modificat eu
     ->AI-ul mi-a oferit explicatii detaliate despre functii si mi-a generat un schelet de baza pe care l-am adaptat cerintelor 
     ->Am facut logica de citire a pid-ului si trimiterea semnalului direct in comanda "--add"
     ->am adaptat mesajele scrise in "logged_district" pentru a respecta strict cerinta proiectului de a scrie explicit daca monitorul a fost informat sau nu
     ->Am tratat erorile, folosind variabila monitor_notified, pentru a verifica daca semnalul a fost trimis cu succes

* 4. Ce am invatat
     ->am invatat cum se realizeaza comunicarea prin semnale, cum sa folosesc un fisier pid pentru a face legatura intre 2 procese diferite
     ->am aflat cum se folosesc functii precum sigaction