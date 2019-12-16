
	Task 1:
		Trimit un GET Request către server-ul cu ip-ul din enunț și url-ul '/task1/start', apoi
	primesc mesajul cu format JSON, pe care îl parsez pentru a obține datele necesare pentru etapa
	următoare.

	Task 2:
		Trimit un POST Request la același server, dar cu url-ul, tipul și datele obținute la prima
	etapă, apoi primesc următorul mesaj.

	Task 3:
		Trimit un GET Request la server, dar cu url-ul și token-ul parsate la etapa a doua și
	răspunsurile la cele două ghicitori. Apoi, primesc un nou mesaj, din care parsez JSON-ul și
	cookie-urile.

	Task 4:
		Trimit din nou un GET Request, cu url-ul și cookie-urile parsate la etapa 3 și token-ul de
	la etapa 2, apoi primesc alt mesaj, din care parsez datele și cookie-urile.

	Task 5:
		Întâi trimit un GET Request la serverul cu datele parsate la etapa 4 și portul 80, pentru
	a obține mesajul în format JSON de la stația meteo, apoi îl trimit la server-ul inițial cu un
	POST Request, folosind url-ul și tipul de la etapa 4 și token-ul de la etapa 2. La final,
	afișez ultimul mesaj primit de la server.

	Observații:
		* Tema a fost implementată în C++, în conformitate cu standardul 11.
		* Pentru parsarea mesajelor în fomat JSON am folosit o bibliotecă open-source de pe github,
	nlohmann/json. (https://github.com/nlohmann/json)
		* Fiecare task a fost rezolvat separat în main, deschizând o conexiune la începutul său și
	închizând-o la final de fiecare dată.
		* Pentru datele obținute din JSON am creat câte o clasă specifică fiecărui task și am
	supraîncarcat funcțiile de parsare to_json() și from_json() pentru fiecare în parte. Cum
	câmpurile enunt, url și method apar la fiecare task, există o clasă de bază care le conține
	doar pe acestea și toate celelalte o moștenesc.

	Makefile:
		* build: compilează și generează executabilul client
		* run: ./client
		* clean: șterge executabilul