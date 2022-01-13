#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SD_JOURNAL_SUPPRESS_LOCATION
#include <systemd/sd-journal.h>

int main(int argc, char *argv[])
{
    if (argc != 3 
        || !argv[1] 
        || !argv[2])
    {
        fputs("Invalid arguments.\nUsage: some-program | jlogger <priority> <identifier>\n", stderr);
        return -1;
    }

    int priority = atoi(argv[1]);
    if (LOG_PRI(priority) != priority)
    {
        fprintf(stderr, "Invalid priority %d.", priority);
        return -1;
    }

    const size_t PRIORITY_DATA_LENGTH = 8 + 1 + 1;
    char priority_data[PRIORITY_DATA_LENGTH + 1];
    sprintf(priority_data, "PRIORITY=%d", priority);

    const size_t MAX_IDENTIFIER_LENGTH = 64;
    size_t identifier_length = strlen(argv[2]);
    if (identifier_length > MAX_IDENTIFIER_LENGTH)
    {
        identifier_length = MAX_IDENTIFIER_LENGTH;
    }

    const size_t IDENTIFIER_PREFIX_LENGTH = 17 + 1;
    const size_t identifier_data_length = IDENTIFIER_PREFIX_LENGTH + identifier_length;
    char identifier_data[identifier_data_length];
    memcpy(identifier_data, "SYSLOG_IDENTIFIER=", IDENTIFIER_PREFIX_LENGTH);
    memcpy(identifier_data + IDENTIFIER_PREFIX_LENGTH, argv[2], identifier_length);

    const size_t MESSAGE_PREFIX_LENGTH = 7 + 1;
    const size_t MAX_MESSAGE_LENGTH = 524288;
    char message_data[MESSAGE_PREFIX_LENGTH + MAX_MESSAGE_LENGTH];
    memcpy(message_data, "MESSAGE=", MESSAGE_PREFIX_LENGTH);
    const ssize_t read_data_length = read(0, message_data + MESSAGE_PREFIX_LENGTH, MAX_MESSAGE_LENGTH);
    if (read_data_length < 0)
    {
        fputs("Failed to read message from stdin.\n", stderr);
        return -1;
    }
    const ssize_t data_length = MESSAGE_PREFIX_LENGTH + read_data_length;

    const size_t FIELD_COUNT = 3;
    struct iovec fields[FIELD_COUNT];
    fields[0].iov_base = priority_data;
    fields[0].iov_len = PRIORITY_DATA_LENGTH;
    fields[1].iov_base = identifier_data;
    fields[1].iov_len = identifier_data_length;
    fields[2].iov_base = message_data;
    fields[2].iov_len = data_length;
    int result = sd_journal_sendv(fields, FIELD_COUNT);
    if (result != 0)
    {
        fprintf(stderr, "Failed to send message to the journal, code %d: %s.", priority, strerror(result));
        return -result;
    }

    return 0;
}
