#include "main.h"
#include "watek_glowny.h"

void mainLoop() {
    srandom(rank);
    int tag;
    int perc;

    while (stan != InFinish) {
        switch (stan) {
            case InRun:
                debug("Stan InRun: %d", stan);
                perc = random() % 100;
                if (perc < 25) {
                    tripSize = random() % 20 + 1;
                    //debug("Perc: %d", perc);
                    debug("tripSize: %d currentCount: %d", tripSize, currentCount);
                    println("Ubiegam się o wejsćie do podprzestrzeni");
                    println("W podprzestrzeni jest %d osób", currentCount);
                    while (currentCount > maxCapacity - tripSize) {
                        // TODO: zastanwocić się, czy da się ulepszyć tą sekcję kody żeby wywalić sleepa
                        println("Nie ma miejsca, czekamy");
                        sleep(1);
                    }

                    println("Jest miejsce w podprzestrzeni, wysyłam pytania o zgody");
                    packet_t *pkt = malloc(sizeof(packet_t));
                    pkt->tripSize = tripSize;
                    ackCount = 0;
                    for (int i = 0; i <= size - 1; i++) {
                        //if (i != rank)
                        priority = lamportClock;
                        sendPacket(pkt, i, REQUEST);
                    }
                    changeState(InWant);
                    free(pkt);
                }
                debug("Skończyłem myśleć");
                break;
            case InWant:
                debug("Stan InWant: %d", stan);
                println("Czekam na wejście do podprzestrzeni")
                // tutaj zapewne jakiś semafor albo zmienna warunkowa
                // bo aktywne czekanie jest BUE
                if (ackCount == size) {
                    println("Mam niezbędne zgody, wchodzę!")
                    changeState(InSection);
                } else {
                    println("Nie dostałem wymaganej ilości zgód, czekam")
                }
                break;
            case InSection:
                debug("Stan InSection: %d", stan);
                // tutaj zapewne jakiś muteks albo zmienna warunkowa
                println("Przechodzę przez podprzestrzeń!")
                sleep(5);
                println("Wychodzę z podprzestrzeni")
                debug("Zmieniam stan na wysyłanie");
                packet_t *pkt = malloc(sizeof(packet_t));
                pkt->tripSize = tripSize;
                for (int i = 0; i <= size - 1; i++) {
                    //if (i != rank)
//                        sendPacket(pkt, (rank + 1) % size, RELEASE);
                    priority = -1;
                    sendPacket(pkt, i, RELEASE);
                }
                changeState(InRun);
                free(pkt);
                break;
            default:
                break;
        }
        sleep(SEC_IN_STATE);
    }
}
