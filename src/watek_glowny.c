#include "main.h"
#include "watek_glowny.h"
#include "util.h"

void mainLoop() {
    srandom(rank);
    int tag;
    int perc;
    int courier;


    while (stan != InFinish) {
        switch (stan) {
            case InRun:
                perc = random() % 100;
                courier = random() % 100;

                if (perc < 25) { debug("Perc: %d", perc);
                    println("Ubiegam się o sekcję krytyczną")debug("Zmieniam stan na wysyłanie");
                    packet_t *pkt = malloc(sizeof(packet_t));

                    ackCount = 0;
                    if (courier > courierPercThreshold) {
                        // kurier
                        println("Przybył do mnie kurier");
                        payload = 1;  // ustaw payload na 'kuriera'
                        tripSize = 1;  // ustaw
                        /*
                         * TODO: rozpisać algorytm obsługi kurierów
                         * nope! my na kuriera nie reagujemy w specjalny sposób, zawsze dajemy mu ack na normalnych zasadach
                         * jeśli to my jesteśmy kurierem, to przed wejściem do podprzestrzeni musimy sprawdzić, czy nie ma w niej
                         * żadnej wycieczki (np. iterując się po tablicy trips?)
                         */
                    } else {
                        // zwykła wycieczka
                        println("Przygotowuję się do przyjęcia wycieczki");
                        payload = 0;  // ustaw payload na 'zwykłą wycieczkę'
                        tripSize = random() % (maxCapacity / 2);  // wylosuj wielkość wycieczki
                        // zapisz dane do struktury pakietu
                        pkt->tripSize = tripSize;
                        pkt->payload = payload;
//                        currentCount = NULL;
                        // sprawdź, czy możesz wsadzić wycieczkę do podprzestrzeni
                        if (currentCount < maxCapacity - tripSize) {
                            for (int i = 0; i <= size - 1; i++)
                                if (i != rank)
                                    sendPacket(pkt, i, REQUEST);
                            changeState(InWant);
                            free(pkt);
                        } else {
                            break;
                        }
                    }
                }
                debug("Skończyłem myśleć");
                break;
            case InWant:
                println("Czekam na wejście do sekcji krytycznej")
                // tutaj zapewne jakiś semafor albo zmienna warunkowa
                // bo aktywne czekanie jest BUE
                if (ackCount == size - 1)
//                   jakie porónanie procesów? hmmm tu chyba musielibyśmy po prostu poczekać, aż będziemy mieć wszystkie ack i tyle?
                    changeState(InSection);
                break;
            case InSection:
                // tutaj zapewne jakiś muteks albo zmienna warunkowa
                println("Przechodzę przez podprzestrzeń!")
                sleep(5);
                //if ( perc < 25 ) {
                debug("Perc: %d", perc);
//              przeiterować się po tablicy trips i jeśli nie ma żadnego ze statusem ACK i wyższym priorytetem niż my, to możemy iść
//              TODO: chciałabym po dostaniu RELEASE dawać trips[i] na null, ale nie mogę znaleźć obsługi odbioru release
                int canGo = 1;
                for (int i=0; i<sizeof(trips); i++){
                    if (trips[i] != NULL && trips[i].ts > lamportClock){
                        canGo = 0;
                        break;
                    }
                }
                if (canGo == 1) {
                    println("Wychodzę z sekcji krytycznej")
                    debug("Zmieniam stan na wysyłanie");
                    packet_t *pkt = malloc(sizeof(packet_t));
                    pkt->tripSize = tripSize;
                    for (int i = 0; i <= size - 1; i++)
                        if (i != rank)
                            sendPacket(pkt, (rank + 1) % size, RELEASE);
                    changeState(InRun);
                    free(pkt);
                }
                //}
                break;
            default:
                break;
        }
        sleep(SEC_IN_STATE);
    }
}
