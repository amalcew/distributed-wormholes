#include "main.h"
#include "watek_komunikacyjny.h"

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *startKomWatek(void *ptr) {
    MPI_Status status;
    int is_message = FALSE;
    packet_t pakiet;
    pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while (stan != InFinish) { debug("czekam na recv");
        MPI_Recv(&pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        trips[pakiet.src] = pakiet;

        switch (status.MPI_TAG) {
            case REQUEST:debug("Ktoś coś prosi. A niech ma!")
//            zgadzamy się zawsze, kiedy my nie ubiegamy się o zasób lub kiedy otrzymany pakiet ma wyższy priorytet
                if (stan != InWant || pakiet.ts > lamportClock) {
                    sendPacket(0, status.MPI_SOURCE, ACK);
//                    zwiększamy licznik osób w podprzestrzeni, kiedy zgadzamy się na wejście nowej wycieczki
                    currentCount = currentCount + pakiet.tripSize;
                }
                break;
            case ACK:debug("Dostałem ACK od %d, mam już %d", status.MPI_SOURCE, ackCount);
                // zaktualizuj zegar, gdy dostaniesz ACK
                pthread_mutex_lock(&clockMut);
                if (pakiet.ts > lamportClock) {
                    lamportClock = pakiet.ts;
                }
                lamportClock++;
                pthread_mutex_unlock(&clockMut);

                pthread_mutex_lock(&mut);
                ackCount++; /* czy potrzeba tutaj muteksa? Będzie wyścig, czy nie będzie? Zastanówcie się. */
                pthread_mutex_unlock(&mut);
                break;
            case: RELEASE:debug("Dostałem RELEASE, ktoś wyszedł z podprzestrzeni")
                currentCount = currentCount - pakiet.tripSize;
                trips[pakiet.src] = NULL;
                break;
            default:
                break;
        }
    }
}
