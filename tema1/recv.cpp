#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <map>

extern "C"
{
    #include "./link_emulator/lib.h"
}

#include "./packet.hpp"

using namespace std;

constexpr auto PORT  = 10001;
constexpr auto DELAY = 1000;

int main(int argc, char** argv)
{
    init((char*)"127.0.0.1", PORT);
    // pachetele folosite la primire
    //  și trimiterea confirmării
    auto recv_pack = packet();
    auto send_pack = packet();
    // dimensiunea ferestrei
    auto window = 0;
    // descriptorul fișierului de ieșire
    auto output = 0;
    // numărul de ordine al mesajului
    //  așteptat la pasul curent
    auto expected_id = 1;
    // numărul ultimului mesaj scris în fișier
    auto last_printed = 1;
    // structură auxiliară pentru reținerea
    //  în ordine a mesajelor primte
    auto verified_packs = map<int, string>();
    // numărul de ordine al pachetului curent primit
    auto number = string(1, '0');

    // ciclez până primesc un prim pachet valid
    while (true)
    {
        // și mă aștept să-l primesc în timp util
        auto ok = recv_message_timeout(recv_pack.data(), DELAY);
        // dacă am primit un pachet cu succes și este valid
        if (ok && !recv_pack.is_corrupt())
        {
            // setez dimensiunea ferestrei
            window = stoi(recv_pack.get_number());
            // deschid fișierul de ieșire
            output = open(recv_pack.get_message(), O_WRONLY | O_CREAT | O_TRUNC, 0);
            send_pack = packet(number);
            // și trimit confirmarea
            send_pack.set_ack(ACK);
            send_message(send_pack.data());
            break;
        }
    }

    // indicii pachetelor din fereastra curentă
    auto first_pack = 1;
    auto last_pack = first_pack + window;

	do  // cât timp nu am primit pachetul de final
	{
        // aștept să primesc pachetele din fereastra curentă
        for (first_pack = expected_id; first_pack < last_pack; ++first_pack)
        {
            // dacă am primit ultimul pachet
            if(recv_pack.verify()) break;

            auto ok = recv_message_timeout(recv_pack.data(), DELAY);

            // dacă recepționarea n-a fost cu succes
            //  trimit un mesaj cu NACK
            if (ok < 1)
            {
                number = to_string(expected_id) + "  ";
                send_pack = packet(number);
                send_pack.set_ack(NACK);
            }
            else
            {
                number = recv_pack.get_number() + "  ";
                send_pack = packet(number);

                // la fel și dacă este corupt
                if (recv_pack.is_corrupt())
                {
                    send_pack.set_ack(NACK);
                }
                else
                {
                    // altfel, trimit confirmarea
                    send_pack.set_ack(ACK);
                    auto pack_id = stoi(recv_pack.get_number());

                    // și deplasez fereastra
                    ++last_pack;

                    // dacă pachetul primit are numărul
                    //  de ordine egal cu cel așteptat
                    if (pack_id == expected_id)
                    {
                        // la următorul ciclu îl
                        //  aștept pe următorul
                        ++expected_id;
                    }
                    else
                    {
                        // altfel îi cer sender-ului să-l retrimită pe cel
                        //  așteptat (util în cazul pachetelor pierdute)
                        auto unexpected_id = to_string(expected_id) + "  ";
                        auto out_of_order_pack = packet(unexpected_id);
                        out_of_order_pack.set_ack(NACK);
                        send_message(out_of_order_pack.data());
                    }

                    // mă asigur că nu suprascriu un mesaj validat
                    if (verified_packs.find(pack_id) == end(verified_packs))
                    {
                        // și inserez mesajul în arbore pentru a-l scrie apoi în fișier
                        verified_packs.insert({pack_id, recv_pack.get_message()});
                    }
                }
            }
            // indiferent de caz, voi trimite la sender
            //  un mesaj de (in)validare
            send_message(send_pack.data());
        }

        // în cazul în care am strans un bloc de mesaje consecutive,
        //  începând cu ultimul scris în fișier,
        //  le voi scrie și elimina din arbore
        for (auto pack_id = last_printed; pack_id < last_pack; ++pack_id)
        {
            auto pack = verified_packs.find(pack_id);
            // dacă am gasit un mesaj lipsă, ies din buclă
            if (pack != end(verified_packs))
            {
                write(output, pack->second.c_str(), pack->second.size());
                verified_packs.erase(pack_id);
                ++last_printed;
            }
            else break;
        }
	}
  	while (!recv_pack.verify());

    // la final, scriu și mesajele rămase în arbore
    for (auto&& message: verified_packs)
    {
         write(output, message.second.c_str(), message.second.size());
    }

    // închid fișierul de ieșire
    close(output);
}
