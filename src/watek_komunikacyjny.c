#include "main.h"
#include "watek_komunikacyjny.h"

pthread_mutex_t counterMut = PTHREAD_MUTEX_INITIALIZER;

/* wątek komunikacyjny; zajmuje się odbiorem i reakcją na komunikaty */
void *startKomWatek(void *ptr) {
    MPI_Status status;
    int is_message = FALSE;
    packet_t pakiet;
    /*
    packet_t trips[size];
    packet_t *nullPkt = malloc(sizeof(packet_t));
    nullPkt->tripSize = -1;
    nullPkt->src = -1;
    nullPkt->ts = -1;
    for (int i = 0; i < size; i++) {
        trips[i] = *nullPkt;
    }
    */
    while (stan != InFinish) {
        /*
        for (int i=0; i < size; i++) {
            if (trips[i].tripSize == -1) continue;
                if (
                        (stan != InWant) ||
                        ((stan = InWant) &&
                         (priority < pakiet.ts || (priority == pakiet.ts && rank < pakiet.src) || rank == pakiet.src))
                        ) {
                    debug("Akceptuję wycieczkę o wielkości %d od procesu %d", pakiet.tripSize, status.MPI_SOURCE);
                    pthread_mutex_lock(&counterMut);
                    debug("CurrentCount przed: %d", currentCount);
                    currentCount += pakiet.tripSize;
                    debug("CurrentCount po: %d", currentCount);
                    pthread_mutex_unlock(&counterMut);
                    sendPacket(0, status.MPI_SOURCE, ACK);
                    trips[i] = *nullPkt;
                } else {
                    debug("Ponownie odrzucam prośbę od procesu %d z powodu niskiego priorytetu", status.MPI_SOURCE);
                }
        }
        */
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
                    debug("Odrzucam prośbę od procesu %d z powodu niskiego priorytetu", status.MPI_SOURCE);
                    //debug("Odrzucam prośbę od procesu %d z powodu braku miejsca", status.MPI_SOURCE);
                    //trips[status.MPI_SOURCE] = pakiet;
                }
                //} else {
                //    debug("Odrzucam prośbę od procesu %d z powodu niskiego priorytetu i zapamiętuję ją", status.MPI_SOURCE);
                //    trips[status.MPI_SOURCE] = pakiet;
                //}
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
