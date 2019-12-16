#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctime>
#include <vector>
#include <unordered_map>
#include <unordered_set>

extern "C"
{
    #include "./link_emulator/lib.h"
}

#include "./packet.hpp"

using namespace std;

constexpr auto PORT  = 10000;
constexpr auto DELAY = 1000;

int main(int argc, char** argv)
{
	init((char*)"127.0.0.1", PORT);
    // dimensiunea ferestrei folosite
    auto window_size = (atoi(argv[2]) * atoi(argv[3])
                        * 1000) / (8 * MSGSIZE);
    // dimensiunea maximă a unui buffer
    //  (rezerv 50 de octeți pentru
    //  numărul de ordine și hash)
    auto buffer_size = MSGSIZE - 50;
    // descriptorul fișierului de intrare
    auto input = open(argv[1], O_RDONLY);
    // pachetele folosite la trimitere
    //  și așteptarea confirmării
    auto send_pack = packet();
    auto recv_pack = packet();
    // structuri auxiliare pentru
    //  a ține evidența pachetelor
    auto sent_packs = unordered_map<int, msg>();
    auto verified_packs = unordered_set<int>();
    // rețin și dacă ultimul pachet primit
    //  a fost recepționat cu succces
    auto recv_ok = true;
    // dimensiunea ultimului pachet trimis
    auto length = 1;
    // indicii pachetelor din fereastra curentă
    auto first_pack = 1;
    auto last_pack = first_pack + window_size;

    // trimit primul pachet (cu metadate) până îi primesc confirmarea
    while (!recv_pack.verify())
    {
        send_pack = packet(to_string(window_size) + " recv_" + string(argv[1]));
        send_message(send_pack.data());
        recv_message_timeout(recv_pack.data(), DELAY);
    }

    // ciclez cât timp n-am terminat de citit din fișier
    //  și nici de trimis și confirmat pachete
    while (true)
    {
        // dacă ultimul mesaj primit înapoi a fost validat
        //  și mai am ce citi din fișier
        if (recv_ok && length > 0)
        {
            // construiesc și trimit pachetele din fereastra curentă
            for (; first_pack < last_pack; ++first_pack)
            {
                // buffer folosit pentru citirea din fișier
                auto buffer = vector<char>(buffer_size);
                // numărul de ordine al pachetului
                auto number = to_string(first_pack) + ' ';
                // deplasament al indicelui de început
                //  pentru mesajul propriu-zius
                auto start = number.size();
                // mut întâi numărul de ordine în buffer
                move(begin(number), end(number), begin(buffer));
                // apoi citesc în continuarea sa mesajul
                length = read(input, buffer.data() + start, buffer_size - start);

                // dacă mesajul citit nu este gol și
                //  n-au existat probleme la citire
                if (length > 0)
                {
                    send_pack = packet(buffer);
                    // atunci trimit pachetul curent
                    send_message(send_pack.data());
                    // și-l rețin într-un hash-map
                    sent_packs.insert({first_pack, *send_pack.data()});
                }
            }
        }

        // aștept un mesaj de confirmare în timp util
        recv_ok = (recv_message_timeout(recv_pack.data(), DELAY) > 0);
        // iau numărul său de ordine sub formă de întreg
        auto pack_id = stoi(recv_pack.get_number());

        // dacă este totul în regulă
        if (recv_ok && recv_pack.verify())
        {
            // îl marchez ca validat
            verified_packs.insert(pack_id);
            // și deplasez fereastra
            ++last_pack;
        }
        else
        {
            // dacă este un pachet corupt și nevalidat
            if (pack_id > 0 && verified_packs.find(pack_id) == end(verified_packs))
            {
                // îl preiau din hash-map
                send_pack.message = sent_packs[pack_id];
                // și-l retrimit
                send_message(send_pack.data());
                // apoi marchez ca nefiind ok,
                //  pentru a nu trimite niciun nou
                //  pachet la următoarea iterație
                recv_ok = false;
            }
        }

        // condiția de oprire a sender-ului
        if (length == 0 && sent_packs.size() == verified_packs.size())
        {
            break;
        }
    }

    // la final trimit un pachet de confirmare
    //  că transmisia de date s-a încheiat cu succes
    auto empty_pack = packet();
    empty_pack.set_ack(ACK);
    send_message(empty_pack.data());

    // închid fișierul de intrare
    close(input);
    // și aștept receiver-ul
    sleep(1);
}
