#include <assert.h>
#include "srules.h"

#define SERVER_RULE_UPSTREAM_BW "upstream_bw."
#define SERVER_RULE_NON_CLIENT_BW "non_client_bw."
#define SERVER_RULE_ARP_INSPECTION "arp_inspection."

#define STR_UP "up"
#define STR_DOWN "down"


/**
 * Initialize server rules structure.
 * @param[in] rules
 */
void srules_init(struct zsrules *rules)
{
    memset(rules, 0, sizeof(*rules));
}

/**
 * Free internally allocated memory.
 * @param[in] rules
 */
void srules_free(struct zsrules *rules)
{
    (void) rules;
}

/**
 * Parse upstream bandwidth rule.
 * @param rules
 * @param str
 * @return
 */
int parse_upstream_bw(struct zsrules *rules, const char *str)
{
    int i = 0;
    uint32_t uidx = 0;
    uint32_t speed = 0;

    while ((NULL != (str = strchr(str, '.'))) && (i < 3)) {
        str++;

        switch (i) {
            case 0: // upstream id
                if ((0 != str_to_u32(str, &uidx)) || (uidx >= UPSTREAM_COUNT)) {
                    return -1;
                }
                break;

            case 1: // speed
                if (0 != str_to_u32(str, &speed)) {
                    return -1;
                }
                break;

            case 2: // direction
                if (0 == strncmp(STR_DOWN, str, STRLEN_STATIC(STR_DOWN))) {
                    rules->upstream_bandwidth[uidx][DIR_DOWN] = speed * 1024 / 8;
                    rules->have.upstream_bandwidth[uidx][DIR_DOWN] = 1;
                } else if (0 == strncmp(STR_UP, str, STRLEN_STATIC(STR_UP))) {
                    rules->upstream_bandwidth[uidx][DIR_UP] = speed * 1024 / 8;
                    rules->have.upstream_bandwidth[uidx][DIR_UP] = 1;
                } else {
                    return -1;
                }

                return 0;

            default:
                assert(false);
                return -1;
        }
        i++;
    }

    return -1;
}

/**
 * Parse non-client bandwidth rule.
 * @param[in,out] rules
 * @param[in] str Rule string.
 * @return Zero on success.
 */
int parse_non_client_bw(struct zsrules *rules, const char *str)
{
    int i = 0;
    uint32_t speed = 0;

    while ((NULL != (str = strchr(str, '.'))) && (i < 3)) {
        str++;

        switch (i) {
            case 0: // speed
                if (0 != str_to_u32(str, &speed)) {
                    return -1;
                }
                break;

            case 1: // direction
                if (0 == strncmp(STR_DOWN, str, STRLEN_STATIC(STR_DOWN))) {
                    rules->non_client_bandwidth[DIR_DOWN] = speed * 1024 / 8;
                    rules->have.non_client_bandwidth[DIR_DOWN] = 1;
                } else if (0 == strncmp(STR_UP, str, STRLEN_STATIC(STR_UP))) {
                    rules->non_client_bandwidth[DIR_UP] = speed * 1024 / 8;
                    rules->have.non_client_bandwidth[DIR_UP] = 1;
                } else {
                    return -1;
                }
                return 0;

            default:
                assert(false);
                return -1;
        }
        i++;
    }

    return -1;
}

static int parse_arp_inspection(struct zsrules *rules, const char *str)
{
    str = strchr(str, '.') + 1;
    if (0 == str_to_u8(str, &rules->arp_inspection)) {
        rules->have.arp_inspection = 1;
        return 0;
    } else {
        return -1;
    }
}

/**
 * Parse rule.
 * @param[in] rules
 * @param[in] str
 */
int srules_parse(struct zsrules *rules, const char *str)
{
    // upstream_bw.<id>.<speed>Kbit.<up|down>
    if (0 == strncmp(str, SERVER_RULE_UPSTREAM_BW, STRLEN_STATIC(SERVER_RULE_UPSTREAM_BW)))
        return parse_upstream_bw(rules, str);

    // non_client_bw.<speed>Kbit.<up|down>
    if (0 == strncmp(str, SERVER_RULE_NON_CLIENT_BW, STRLEN_STATIC(SERVER_RULE_NON_CLIENT_BW)))
        return parse_non_client_bw(rules, str);

    // arp_inspection.<mode>
    if (0 == strncmp(str, SERVER_RULE_ARP_INSPECTION, STRLEN_STATIC(SERVER_RULE_ARP_INSPECTION)))
        return parse_arp_inspection(rules, str);

    return -1;
}
