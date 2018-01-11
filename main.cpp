#include <iostream>
#include <cmath>
#include <cstring>
#include <vector>
#include <list>

#define LOG if(view)
int number_application = 0; // don't touch
int number_subscriber = 64;
bool fight_against_occupation_channel = true; // true - есть борьба с захватом канала
uint32_t ttl = 200000;
double probability_min = 1.0 / number_subscriber;
bool view = false;

/*            if (existence_occupation_channel) {
                prob[hero] = 1;
            } else {
                if (appl[hero] > 1) {
                    prob[hero] = 0.5;
                }
            }*/

uint32_t
get_puasson_value(double lambda) {
    double z = (double) rand() / RAND_MAX;
    uint32_t n = 0;
    double s;
    double r;
    s = r = exp(-lambda);
    while (z > s) {
        n++;
        r = r * (lambda / n);
        s = s + r;
    }
    return n;
}

std::vector<uint32_t>
*generation_queue_of_applications(double lambda) {
    std::vector<uint32_t> *queue_of_applications = new std::vector<uint32_t>;

    int counter_application = 0;
    uint32_t tmp;
    uint32_t time = 0;
    while (time < ttl) {
        tmp = get_puasson_value(lambda);
        counter_application += tmp;
        queue_of_applications->push_back(tmp);
        time++;
//        std::cout << tmp << std::endl;
    }
    number_application = counter_application;
//    std::cout << "Size of counter_application = " << counter_application << std::endl;
    return queue_of_applications;
}

void
gen_timepoint(uint32_t *timepoint, uint32_t count_units) {
    memset(timepoint, 0, sizeof(timepoint[0]) * number_subscriber);
    double r;
    int id;
    for (uint32_t i = 0; i < count_units; i++) {
        r = number_subscriber * ((double) rand() / RAND_MAX);
        id = (int) floor(r);
        if (id == number_subscriber) id--;
        timepoint[id]++;
    }
//    do {
//        r = number_subscriber * ((double) rand() / RAND_MAX);
//        id = (int) round(r);
//        timepoint[id] = 1;
//    } while (weight_vector(timepoint) != count_units);
}

void
print_subscriber(uint32_t *subscriber, u_long length, int id) {
    std::cout << "subscriber №" << id << " : ";
    for (int i = 0; i < length; i++) {
        std::cout << subscriber[i] << " ";
    }
    std::cout << std::endl;
}

int
send(double probability) {
    double r = ((double) rand() / RAND_MAX);
    if (r < probability) {
        return 1;
    }
    return 0;
}

void
simulation_of_queue_application(uint32_t **&subscribers, u_long &quantity_of_windows, double lambda) {
    std::vector<uint32_t> *queue_of_applications = generation_queue_of_applications(lambda);
    uint32_t *timepoint = new uint32_t[number_subscriber];
    subscribers = new uint32_t *[number_subscriber];

    for (int i = 0; i < number_subscriber; i++) {
        subscribers[i] = new uint32_t[queue_of_applications->size()];
    }

//    for (int t = 0; t < queue_of_applications->size(); t++) {
//        printf("%d ", queue_of_applications->at(t));
//    }
//    printf("\n");

    for (int t = 0; t < queue_of_applications->size(); t++) {
        gen_timepoint(timepoint, queue_of_applications->at(t));
        for (int l = 0; l < number_subscriber; l++) {
            subscribers[l][t] = timepoint[l];
        }
    }
    quantity_of_windows = queue_of_applications->size();

//    std::cout << "\t\tSuccessful end of simulation of queue application" << std::endl;
}

void
binary_exponential_backof(double lambda, double probability_min, double &stream_out, double &average_delay) {
    int number_application_out = 0;
    uint32_t **subscribers = NULL;
    u_long quantity_of_windows;
    average_delay = 0;

    simulation_of_queue_application(subscribers, quantity_of_windows, lambda);

    uint32_t **out = new uint32_t *[number_subscriber];
    for (int i = 0; i < number_subscriber; i++) {
        out[i] = new uint32_t[quantity_of_windows];
        memset(&out[i][0], 0, sizeof(out[0][0]) * quantity_of_windows);
    }

    LOG {
        std::cout << "\t\t\t\t";
        for (int i = 0; i < quantity_of_windows; i++) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
        std::cout << "Input app:\n";
        for (int i = 0; i < number_subscriber; i++) {
            print_subscriber(subscribers[i], quantity_of_windows, i);
        }
        std::cout << "Output app:\n";
        for (int i = 0; i < number_subscriber; i++) {
            print_subscriber(out[i], quantity_of_windows, i);
        }
    }

    int tmp;
    double *prob = new double[number_subscriber];
    for (int i = 0; i < number_subscriber; i++) {
        prob[i] = 1;
    }
    uint32_t *appl = new uint32_t[number_subscriber];
    for (int i = 0; i < number_subscriber; i++) {
        appl[i] = 0;
    }
    uint32_t *senders = new uint32_t[number_subscriber];
    for (int i = 0; i < number_subscriber; i++) {
        senders[i] = 0;
    }
    int hero = -1;
    for (int window = 0; window < quantity_of_windows; window++) {
        tmp = 0;
        LOG std::cout << "Timepoint = " << window << "\t\ttmp = " << tmp;
        for (int i = 0; i < number_subscriber; i++) {
            senders[i] = 0;
        }
        LOG std::cout << "\t\tprobab = ";
        for (int j = 0; j < number_subscriber; j++) {
            LOG std::cout << prob[j] << " ";
            appl[j] += subscribers[j][window];
            if (appl[j] != 0) {
                int result = send(prob[j]);
                tmp += result;
                if (result == 1) {
                    senders[j] = 1;
                    hero = j;
                }
            }
        }
        LOG {
            std::cout << "\t\tappl = ";
            for (int j = 0; j < number_subscriber; j++) {
                std::cout << appl[j] << " ";
            }
            std::cout << std::endl;
        }
        if (tmp == 0) {
            continue;
        }
        if (tmp == 1) {
            LOG {
                std::cout << "-----------------------------------------------------------------------------------"
                          << std::endl;
                std::cout << "Timepoint = " << window << "\t\tsubscriber №" << hero
                          << " sent the message with probability = " << prob[hero] << std::endl;
                std::cout << "-----------------------------------------------------------------------------------"
                          << std::endl << std::endl;
            }
            out[hero][window] = 1;
            number_application_out++;
            prob[hero] = 1;
            appl[hero]--;
        }

        if (tmp > 1) {
            LOG std::cout << "Timepoint = " << window << "\t\tconflict for: ";
            for (int k = 0; k < number_subscriber; k++) {
                if (appl[k] == 0) {
                    continue;
                }
                LOG {
                    if (senders[k] != 0) {
                        std::cout << k << " ";
                    }
                }
                prob[k] = prob[k] / 2;
                if (prob[k] < probability_min) {
                    prob[k] = probability_min;
                }
            }
            LOG std::cout << std::endl;
        }
    }
    LOG std::cout << std::endl;
    stream_out = (double) number_application_out / quantity_of_windows;

    LOG {
        std::cout << "\t\t\t\t";
        for (int i = 0; i < quantity_of_windows; i++) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
        for (int i = 0; i < number_subscriber; i++) {
            print_subscriber(subscribers[i], quantity_of_windows, i);
        }

        std::cout << "Delay" << std::endl;
        for (int i = 0; i < number_subscriber; i++) {
            print_subscriber(out[i], quantity_of_windows, i);
        }
    }

    int in_id = 0;
    int out_id = 0;
    int count_app = 0;

    for (int subs = 0; subs < number_subscriber; subs++) {
        out_id = 0;
        for (int window = 0; window < quantity_of_windows; window++) {
            if (subscribers[subs][window] == 0) {
                continue;
            }
            in_id = window;
            count_app = subscribers[subs][window];
            do {
                if (out[subs][out_id] != 0) {
                    count_app--;
                    average_delay += out_id - in_id + 1;
                    LOG printf("Sub: %d\tin: %d\tout: %d\taverage_delay: %d\n", subs, in_id, out_id,
                               out_id - in_id + 1);
                }
                out_id++;
                if (count_app == 0) break;
            } while (out_id + 1 <= quantity_of_windows - 1);

            while (count_app > 0) {
                average_delay += quantity_of_windows - 1 - in_id + 1;
                LOG printf("Sub: %d\tin: %d\tout: %d\taverage_delay: %d\n", subs, in_id, quantity_of_windows - 1,
                           quantity_of_windows - 1 - in_id + 1);
                count_app--;

            }
        }
    }
    LOG std::cout << "average average_delay = " << (double) average_delay / number_application << std::endl;
    average_delay = (double) average_delay / number_application;

    for (int i = 0; i < number_subscriber; i++) {
        delete[] subscribers[i];
        delete[] out[i];
    }
    delete[] out;
    delete[] subscribers;
    delete[] appl;
    delete[] senders;
    delete[] prob;

    LOG std::cout << "\t\tSuccessful termination of an algorithm" <<
                  std::endl;
}

int main() {
    srand((uint) time(NULL));
    double stream_out = 0;
    bool test = false;
    double delay;

    if (test) {
        view = true;
        binary_exponential_backof(0.8, probability_min, stream_out, delay);
    } else {
        system("mkdir -p streams/");
        system("mkdir -p delays/");
        FILE *fileStream = fopen("streams/stream.csv", "w");
        FILE *fileDelay = fopen("delays/delay.csv", "w");
        double lambda_max = 0.38;

        for (double lambda = 0.01; lambda < lambda_max; lambda += 0.01) {
            fprintf(fileStream, "%f,", lambda);
            fprintf(fileDelay, "%f,", lambda);
        }
        fprintf(fileStream, "\n");
        fprintf(fileDelay, "\n");
        for (double lambda = 0.01; lambda < lambda_max; lambda += 0.01) {
            stream_out = 0;
            delay = 0;
            binary_exponential_backof(lambda, probability_min, stream_out, delay);
            fprintf(fileStream, "%f,", stream_out);
            fprintf(fileDelay, "%f,", delay);
            std::cout << "stream in = " << lambda << "\t\t\tstream out = " << stream_out <<
                      "\t\tdelay = " << delay << std::endl;
        }
        fclose(fileStream);
        fclose(fileDelay);

        std::cout << "\t\tSuccessful end of all";
        system("csv_charts -xy streams/ &");
        system("csv_charts -xy delays/ &");
    }
    return 0;
}