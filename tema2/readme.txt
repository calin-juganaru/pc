
@Descrierea generală a algoritmului:

		La pornirea server-ului, acesta va deschide 3 sockeți (pentru citire de la stdin, clienți
	UDP și clienți TCP), permițând un număr maxim de abonați stabilit în constanta MAX_SUBSCRIBERS
	din fișierul server.hpp, apoi va cicla până când primește comanda 'exit' de la tastatură sau
	este întrerupt forțat (inclusiv în cazul erorilor fatale). În buclă, va itera prin mulțimea de
	sockeți activi și se va comporta diferit pentru fiecare dintre următoarele cazuri:

	1) Citirea de la stdin:
		În cazul în care primește comanda 'exit', va trimite un semnal de închidere către toți
	clienții TCP, apoi îi va șterge din mulțimea de sockeți, împreună cu datele corespunzătoare
	acestora din structurile auxiliare, va închide socket-ul TCP și va returna 0. În cazul oricărei
	alte comenzi primite de la stdin, nu se va întâmpla nimic.

	2) Socket-ul TCP:
		Pe acest socket va primi cereri de conexiune din partea clienților TCP, adică abonați, și,
	dacă un nou client a fost acceptat cu succes, va primi de la acesta numele (ID-ul) unic și
	va fi adăugat în mulțimea de sockeți și în structurile de date auxiliare. Dacă ID-ul există
	deja, cererea va fi respinsă, dar clientul existent cu acel id nu va fi afectat.

	3) Socket-ul UDP:
		Un client UDP nu necesită stabilirea unei conexiuni precum cel TCP, și doar transmite
	mesajele cu formatul specificat în enunț. Serverul apoi va parsa mesajul, punându-l în forma
	pe care îl așteaptă un subscriber, apoi îl va transmite tuturor abonaților (activi) la topicul
	respectiv și-l va stoca pentru cei ce nu mai sunt abonați dar au setat flag-ul pentru store &
	forward (pe care îi vom numi inactivi), cu scopul de a le fi transmis după reabonare. Deoarece
	clienții UDP pot transmite mesajele cu viteză foarte mare, iar cei TCP le pot primi alipite,
	am folosit un delay (system call-ul usleep()) foarte mic, pentru a asigura transmisia corectă.

	4) Oricare alt socket (abonații):
		Serverul așteaptă mesaje de la abonați conținând comenzi de tipul 'exit', 'subscribe' sau
	'unsubscribe'. Este garantat de la parsarea comenzilor primite de clienții TCP că mesajele
	trimise către server conțin doar comenzi valide. În primul caz, serverul va închide conexiunea
	cu abonatul ca în situația 1), diferența fiind că acum este aplicată pe un singur client, iar
	acesta nu mai necesită primirea unui semnal de sfârșit.
		În cazul abonării, clientul va fi adăugat în structurile de date auxiliare pentru
	stabilirea legăturii între ID-ul său și topic, iar, dacă acesta a fost abonat în trecut,
	având flag-ul SF setat, va primi instant toate mesajele transmise în absența sa. Când se
	dezabonează, va fi eliminat din unele structuri de date, ramânând numai în cea pentru stocarea
	mesajelor din topicurile pentru care a ales SF. Un client care se deconectează (exit) este
	automat dezabonat de la toate topicurile (dar pastrându-i-se mesajele cu SF).

		Clientul încearcă să se conecteze la server cu un id, un ip și un port, apoi, dacă operația
	a fost realizată cu succes ciclează până la primirea comenzii 'exit' de la tastatură sau de la
	server. Acesta poate primi comenzi de 'subscribe' sau 'unsubscribe', pe care le va parsa, și
	va trimite serverului mesaje de forma nume_topic, urmat de spațiu, apoi 0 sau 1 și spațiu, dacă
	dorește store & forward la abonare (la dezabonare nu există acest flag), apoi 1 sau 0, pentru
	abonare, respectiv dezabonare de la acest topic. Spațiile sunt folosite pentru funcția de
	parsare get_word() din utils.hpp. Orice altă comandă nu este acceptată și va afișa un mesaj de
	eroare, dar nu se va opri din execuție. În cazul primirii unui mesaj de la server (abonament),
	acesta va fi afișat așa cum este, deoarece a fost formatat în prealabil de către server, după
	specificațiile din enunț.


@Structurile de date:

	- subscribers[topic][id] = {subscribe, store_forward}:
		un hash_map de hash_map-uri folosit pentru a reține toți abonații (id) la un anumit topic,
		și câte două flag-uri pentru aceștia, dacă sunt activi, respectiv, dacă au store & forward

	- id_to_socket[id] = socket:
		un hash_map ce leagă id-ul abonatului la socket-ul pe care este conectat la server

	- socket_to_id[socket] = id:
		un hash_map ce realizează operația inversă celui precedent

	- store_buffer[id][topic] = messages[]:
		tot un hash_map, ce se comportă ca un buffer, stocând mesajele pentru abonații inactivi

	- topics[id] = topics[]:
		încă un hash_map, ce reține o listă de topicuri la care este abonat id

@Observație: programele au fost implementate cu standardul C++11.