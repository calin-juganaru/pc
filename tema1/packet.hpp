#include <utility>

#include "./link_emulator/lib.h"

// constantele folosite la
//  validarea pachetelor
constexpr char ACK  = 6;
constexpr char NACK = 21;

using namespace std;

// clasă pentru încapsularea mesajului,
//  numărului de ordine, hash-ului
//  și/sau a codului de verificare
struct packet
{
    msg message;

    msg* data()
    {
        return &message;
    }

    packet(): message(msg()) {}

    template <typename T>
    packet(T&& buffer)
    {
        auto length = 0;
        // întai aflu lungimea mesajului
        //  din buffer-ul primit
        while (buffer[length++]);
        // și un pointer la începutul payload-ului
        auto start = begin(message.payload);

        // mut întreg conținutul buffer-ului
        //  în payload-ul mesajului,
        move(begin(buffer), end(buffer), start);
        // îi marchez sfârșitul,
        message.payload[length - 1] = '\0';
        // și-i setez lungimea
        message.len = strlen(message.payload);

        // apoi calculez hash-ul mesajului
        auto aux = to_string(this->compute_hash());
        // și-l mut la finalul payload-ului
        move(begin(aux), end(aux), start + message.len + 1);
        // setez finalul hash-ului și al întregului payload
        message.payload[message.len + aux.size() + 1] = '\0';
        message.payload[MSGSIZE - 1] = '\0';
    }

    void set_ack(const char& ack)
    {
        *get_message() = ack;
        message.len = 1;
    }

    size_t size()
    {
        return message.len;
    }

    char* get_hash()
    {
        return message.payload + size() + 1;
    }

    string get_number()
    {
        // încep cu un string ce conține
        //  doar un caracter de '0',
        auto number = string(1, '0');
        // apoi construiesc numărul
        //  de la începutul payload-ului
        for (auto i = 0; true; ++i)
        {
            // pe primele poziții din payload
            //  garantat vor fi cifre,
            //  apoi un spațiu, apoi
            //  restul conținutului
            if (isdigit(message.payload[i]))
            {
                number.push_back(message.payload[i]);
            }
            else break;
        }
        return number;
    }

    char* get_message()
    {
        auto i = 0;
        // mesajul propriu-zis începe de la
        //  poziția de după ce se termină cifrele
        while (isdigit(message.payload[i++]));
        return message.payload + i;
    }

    // calculez hash-ul pe baza mesajului conținut
    //  folosind o funcție foartă simplă
    //  și rezonabil de eficientă
    size_t compute_hash()
    {
        auto result = 0ul;
        for (auto i = 0; i < message.len; ++i)
        {
            result *= message.len;
            result += message.payload[i];
        }
        return result;
    }

    bool is_corrupt()
    {
        // compar hash-ul din pachet
        auto hash_1 = to_string(this->compute_hash());
        // cu unul calculat acum
        auto hash_2 = this->get_hash();
        for (auto i = 0u; i < hash_1.size(); ++i)
            if (hash_1[i] != hash_2[i])
                return true;

        return false;
    }

    bool verify()
    {
        return (get_message()[0] == ACK);
    }
};