#include "main.h"
#include "watek_komunikacyjny.h"

pthread_mutex_t counterMut = PTHREAD_MUTEX_INITIALIZER;

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *startKomWatek(void *ptr) {
    MPI_Status status;
    int is_message = FALSE;
    packet_t pakiet;
    /* Obrazuje pętlę odbierającą pakiety o różnych typach */
    while (stan != InFinish) {
        debug("czekam na recv");
        MPI_Recv(&pakiet, 1, MPI_PAKIET_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch (status.MPI_TAG) {
            case REQUEST:
                //debug("priority: %d\npakiet.ts: %d\nrank: %d\npakiet.src: %d", priority, pakiet.ts, rank, pakiet.src)
                if (
                    // jeśli nie ubiegasz się o sekcję, puszczaj wszystko
                    (stan != InWant) ||
                    // w innym wypadku puszczaj tylko wątki o wyższym priorytecie lub wyższym identyfikatorze
                    ((stan = InWant) && ( priority < pakiet.ts || (priority == pakiet.ts && rank < pakiet.src) || rank == pakiet.src ))
                    //|| (1)
                ) {
                    //if (currentCount + pakiet.tripSize < maxCapacity) {
                        debug("Akceptuję wycieczkę o wielkości %d od procesu %d", pakiet.tripSize, status.MPI_SOURCE);
                        pthread_mutex_lock(&counterMut);
                        debug("CurrentCount przed: %d", currentCount);
                        currentCount += pakiet.tripSize;
                        debug("CurrentCount po: %d", currentCount);
                        pthread_mutex_unlock(&counterMut);
                        sendPacket(0, status.MPI_SOURCE, ACK);
                    } else {
                        debug("Odrzucam prośbę od procesu %d", status.MPI_SOURCE);
                    //}
                }
                break;
            case ACK:
                debug("Dostałem ACK od %d, mam już %d", status.MPI_SOURCE, ackCount);
                ackCount++; /* czy potrzeba tutaj muteksa? Będzie wyścig, czy nie będzie? Zastanówcie się. */
                break;
            case RELEASE:
                debug("Dostałem RELEASE od %d, wycieczka miała rozmiar %d", status.MPI_SOURCE, pakiet.tripSize);
                pthread_mutex_lock(&counterMut);
                debug("CurrentCount przed: %d", currentCount);
                currentCount -= pakiet.tripSize;
                debug("CurrentCount po: %d", currentCount);
                pthread_mutex_unlock(&counterMut);
                break;
            default:
                break;
        }
    }
}
