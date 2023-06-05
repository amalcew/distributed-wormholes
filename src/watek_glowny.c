#include "main.h"
#include "watek_glowny.h"

void mainLoop() {
    srandom(rank);
    int perc;

    while (stan != InFinish) {
        switch (stan) {
            case InRun:
                debug("Stan InRun: %d", stan);
                perc = random() % 100;
                if (perc < 25) {
                    if (flagRepeat == 1) debug("Powtarzam prośbę o dopuszczenie do podprzestrzeni");
                    if (flagRepeat == 0) tripSize = random() % 20 + 1;
                    debug("tripSize: %d currentCount: %d flagRepeat: %d", tripSize, currentCount, flagRepeat);
                    println("Ubiegam się o wejsćie do podprzestrzeni");
                    println("W podprzestrzeni jest %d osób", currentCount);
                    while (currentCount > maxCapacity - tripSize) {
                        // TODO: zastanwocić się, czy da się ulepszyć tą sekcję kody żeby wywalić sleepa
                        //println("Nie ma miejsca, czekamy");
                        //sleep(1);
                    }

                    println("Jest miejsce w podprzestrzeni, wysyłam pytania o zgody");
                    packet_t *pkt = malloc(sizeof(packet_t));
                    pkt->tripSize = tripSize;
                    ackCount = 0;
                    for (int i = 0; i <= size - 1; i++) {
                        if (flagRepeat == 0) priority = lamportClock;
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
                    flagRepeat = 0;
                    changeState(InSection);
                } else {
                    updateClock();
                    println("Nie dostałem wymaganej ilości zgód, czekam")
                    //flagRepeat = 1;
                    // TODO [pytanie do Daniego]: czy niezgodnie z algorytmem możemy sobie tutaj odejmować od currentCounta?
                    //changeState(InRun);
                }
                break;
            case InSection:
                debug("Stan InSection: %d", stan);
                if (flagEscape == 0) {
                    println("Przechodzę przez podprzestrzeń!");
                    int sleepTime = (random() % 5) + 1;
                    sleep(sleepTime);
                    flagEscape = 1;
                }
                println("Czekam w kolejce do wyjścia");
                // TODO [pytanie do Daniego]: jak rozwiązać problem pamiętania priorytetów, bez pamiętania całej historii komunikatów
                int canGo = 1;
                for (int i=0; i<size;i++) {
                    //if (<cośtam>) {
                    //    canGo = 0;
                    //    break;
                    //}
                }

                if (canGo == 1) {
                    println("Wychodzę z podprzestrzeni");
                    debug("Zmieniam stan na wysyłanie");
                    packet_t *pkt = malloc(sizeof(packet_t));
                    pkt->tripSize = tripSize;
                    for (int i = 0; i <= size - 1; i++) {
                        priority = -1;
                        sendPacket(pkt, i, RELEASE);
                    }
                    changeState(InRun);
                    free(pkt);
                } else {
                    printf("Nie wychodzę, ktoś ma wyższy priorytet");
                }
                break;
            default:
                break;
        }
        sleep(SEC_IN_STATE);
    }
}
